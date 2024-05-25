#include <chrono>
#include "raylib.h"
#include <cstdint>
#include <limits>
#include <atomic>
#include <mutex>
#include <AudioFile.h>
#include "engine/synth.h"
#include "utli/Keyboard.hpp"
#include "layout/main_window.h"
#include <RtMidi.h>

static mana::Synth synth_;
static mana::KeyBoard keyboard_;
static mana::MainWindow synth_layout_{ synth_ };

static void ThisAudioCallback(void* buffer, unsigned int frames) {
    std::scoped_lock lock{ synth_.GetSynthLock() };
    synth_.Render(frames);
    std::ranges::copy(synth_.getBuffer(), static_cast<float*>(buffer));
    synth_layout_.GetWaveScope().PushBuffer(synth_.getBuffer());
}

static void ThisRtMidiCallback(double timeStamp, std::vector<unsigned char> *message, void *userData) {
    if (message->size() == 3) {
        if ((message->at(0) >> 4) == 9) {
            if (message->at(2) == 0) {
                synth_.NoteOff(message->at(1), message->at(2) / 127.0f);
            }
            else {
                synth_.NoteOn(message->at(1), message->at(2) / 127.0f);
            }
        }
        else if ((message->at(0) >> 4) == 8) {
            synth_.NoteOff(message->at(1), message->at(2) / 127.0f);
        }
    }
}

int main(void) {
    RtMidiIn midi_in{ RtMidi::Api::WINDOWS_MM };
    midi_in.openPort();
    midi_in.setCallback(ThisRtMidiCallback);

    InitWindow(800, 600, "additive synth");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(480);
    AudioStream stream = LoadAudioStream(48000, 32, 1);
    SetAudioStreamCallback(stream, ThisAudioCallback);

    PlayAudioStream(stream);        // Start processing stream buffer (no data loaded currently)
    SetTargetFPS(30);               // Set our game to run at 30 frames-per-second

    // init synth
    synth_.Init(480, 48000.0f, 400.0f);
    synth_layout_.SetBounds(0, 0, 800, 600);

    // link keyboard and synth
    keyboard_.onNoteOn = [](int n) {synth_.NoteOn(n, 1.0f); };
    keyboard_.onNoteOff = [](int n) {synth_.NoteOff(n, 1.0f); };

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // drawing
        BeginDrawing();
        ClearBackground(BLACK);
        {
            {
                std::scoped_lock lock{ synth_.GetSynthLock() };
                keyboard_.checkInFrame();
            }

            synth_layout_.paint();

            /*mana::Partials drawing_partials_;
            {
                std::scoped_lock lock{ synth_.GetSynthLock() };
                drawing_partials_ = synth_.GetDisplayOscillor().GetPartials();
            }
            for (size_t i = 0; i < mana::kNumPartials; ++i) {
                auto x_nor = drawing_partials_.pitches[i] / 140.0f;
                auto y_nor = 0.0f;
                auto y_gain = std::abs(drawing_partials_.gains[i]);
                if (y_gain > 0.0f) {
                    auto y_del = std::clamp(20.0f * std::log10(y_gain), -60.0f, 20.0f);
                    y_nor = (y_del + 60.0f) / 80.0f;
                }
                int x = static_cast<int>(800 * x_nor);
                int y = static_cast<int>(600 * (1.0f - y_nor));
                DrawLine(x, y, x, 600, WHITE);
            }*/
        }
        EndDrawing();
    }

    UnloadAudioStream(stream);   // Close raw audio stream and delete buffers from RAM
    CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)

    CloseWindow();              // Close window and OpenGL context

    midi_in.cancelCallback();
    midi_in.closePort();

    return 0;
}