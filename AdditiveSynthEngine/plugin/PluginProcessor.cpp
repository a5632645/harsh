#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "engine/synth.h"
#include "data/juce_param_creator.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     #endif
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
    ) {
    auto creator = std::make_shared<mana::JuceParamCreator>();
    synth_ = std::make_unique<mana::Synth>(creator, 4);
    auto juce_params = creator->MoveJuceParams();
    apvts_ = std::make_unique<juce::AudioProcessorValueTreeState>(*this,
                                                                  nullptr,
                                                                  "parameter",
                                                                  juce::AudioProcessorValueTreeState::ParameterLayout{ juce_params.begin(), juce_params.end() });
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
    #if JucePlugin_WantsMidiInput
    return true;
    #else
    return false;
    #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
    #if JucePlugin_ProducesMidiOutput
    return true;
    #else
    return false;
    #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
    #if JucePlugin_IsMidiEffect
    return true;
    #else
    return false;
    #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth_->Init(samplesPerBlock, sampleRate, 800.0f);
    update_pos_ = synth_->GetUpdateSkip();
    inv_buffer_length_ = 1.0f / (samplesPerBlock * 1000000.0f / sampleRate);
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
    #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
    #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    #endif

    return true;
    #endif
}

struct SimpleTimeReporter {
    SimpleTimeReporter(float inv_buffer_length) : inv_buffer_length_(inv_buffer_length) {
        start = std::chrono::steady_clock::now();
    }

    ~SimpleTimeReporter() {
        auto end = std::chrono::steady_clock::now();
        if (end - last_output_time_ > std::chrono::seconds(1)) {
            auto num_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            DBG(std::format("[time reporter]: {} us, cpu: {}%",
                            num_us,
                            num_us * inv_buffer_length_ * 100.0f));
            last_output_time_ = end;
        }
    }

    std::chrono::time_point<std::chrono::steady_clock> start;
    float inv_buffer_length_{};
    inline static auto last_output_time_ = std::chrono::steady_clock::now();
};

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                             juce::MidiBuffer& midiMessages)
{
    SimpleTimeReporter _{ inv_buffer_length_ };

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    std::scoped_lock audio_lock{ synth_->GetSynthLock() };

    auto num_frame = buffer.getNumSamples();
    auto update_skip = synth_->GetUpdateSkip();
    auto* ptr_buffer = buffer.getWritePointer(0);
    int write_pos = 0;

    auto process_midi = [&midiMessages, this](int begin, int end) {
        auto it_begin = midiMessages.findNextSamplePosition(begin);
        auto it_end = midiMessages.findNextSamplePosition(end);

        for (auto it = it_begin; it != it_end; ++it) {
            auto msg = (*it).getMessage();
            if (msg.isNoteOn()) {
                synth_->NoteOn(msg.getNoteNumber(), msg.getFloatVelocity());
            }
            else if (msg.isNoteOff()) {
                synth_->NoteOff(msg.getNoteNumber(), msg.getFloatVelocity());
            }
        }
    };

    if (left_num_render_ != 0) {
        process_midi(0, left_num_render_);
        synth_->Render(ptr_buffer, left_num_render_);
        write_pos += left_num_render_;
        left_num_render_ = 0;
    }

    while (true) {
        auto next_update_pos = write_pos + update_skip;
        process_midi(write_pos, next_update_pos);
        synth_->update_state(update_skip);
        auto num_rende = std::min(update_skip, num_frame - write_pos);
        synth_->Render(ptr_buffer + write_pos, num_rende);
        write_pos += update_skip;

        if (next_update_pos >= num_frame) {
            left_num_render_ = next_update_pos - num_frame;
            break;
        }
    }

    std::copy(ptr_buffer, ptr_buffer + num_frame, buffer.getWritePointer(1));
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}