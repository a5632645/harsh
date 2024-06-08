# Example Audio Plugin CMakeLists.txt

# To get started on a new plugin, copy this entire folder (containing this file and C++ sources) to
# a convenient location, and then start making modifications.

# The first line of any CMake project should be a call to `cmake_minimum_required`, which checks
# that the installed CMake will be able to understand the following CMakeLists, and ensures that
# CMake's behaviour is compatible with the named version. This is a standard CMake command, so more
# information can be found in the CMake docs.

cmake_minimum_required(VERSION 3.22)

# The top-level CMakeLists.txt file for a project must contain a literal, direct call to the
# `project()` command. `project()` sets up some helpful variables that describe source/binary
# directories, and the current project version. This is a standard CMake command.

project(HARSH VERSION 0.0.1)

# If you've installed JUCE somehow (via a package manager, or directly using the CMake install
# target), you'll need to tell this project that it depends on the installed copy of JUCE. If you've
# included JUCE directly in your source tree (perhaps as a submodule), you'll need to tell CMake to
# include that subdirectory as part of the build.

# find_package(JUCE CONFIG REQUIRED)        # If you've installed JUCE to your system
# or
# add_subdirectory(JUCE)                    # If you've put JUCE in a subdirectory called JUCE

# If you are building a VST2 or AAX plugin, CMake needs to be told where to find these SDKs on your
# system. This setup should be done before calling `juce_add_plugin`.

# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

# `juce_add_plugin` adds a static library target with the name passed as the first argument
# (AudioPluginExample here). This target is a normal CMake target, but has a lot of extra properties set
# up by default. As well as this shared code static library, this function adds targets for each of
# the formats specified by the FORMATS arguments. This function accepts many optional arguments.
# Check the readme at `docs/CMake API.md` in the JUCE repo for the full list.

juce_add_plugin(Harsh
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # COMPANY_NAME ...                          # Specify the name of the plugin's author
    # IS_SYNTH TRUE/FALSE                       # Is this a synth or an effect?
    IS_SYNTH TRUE
    # NEEDS_MIDI_INPUT TRUE/FALSE               # Does the plugin need midi input?
    NEEDS_MIDI_INPUT TRUE
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT TRUE/FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    # COPY_PLUGIN_AFTER_BUILD TRUE/FALSE        # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Juce               # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Dem0                            # A unique four-character plugin id with exactly one upper-case character
                                                # GarageBand 10.3 requires the first letter to be upper-case, and the remaining letters to be lower-case
    FORMATS AU VST3 Standalone                  # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "harsh")        # The name of the final executable, which can differ from the target name

# `juce_generate_juce_header` will create a JuceHeader.h for a given target, which will be generated
# into your build tree. This should be included with `#include <JuceHeader.h>`. The include path for
# this header will be automatically added to the target. The main function of the JuceHeader is to
# include all your JUCE module headers; if you're happy to include module headers directly, you
# probably don't need to call this.

# juce_generate_juce_header(AudioPluginExample)

# `target_sources` adds source files to a target. We pass the target that needs the sources as the
# first argument, then a visibility parameter for the sources which should normally be PRIVATE.
# Finally, we supply a list of source files that will be built into the target. This is a standard
# CMake command.

set(UI_SRC
"ui/wrap_drop_box.cpp"
"ui/wrap_slider.cpp"
"layout/synth/dissonance_layout.cpp"
"layout/synth/timber_layout.cpp"
#"ui/oscilloscope.cpp"
 "layout/synth/filter_layout.cpp"
 "layout/effect/sp_effect_layout.cpp"
 "layout/resynthsis/resynthsis_layout.cpp"
 
  "layout/synth/osc_layout.cpp"
  #"ui/curve_editor.cpp"
  #"layout/curve_layout.cpp"
  "layout/main_window.cpp"
  
 "ui/wrap_check_box.cpp"
 "layout/modulators/lfo_layout.cpp"
 "layout/matrix/modulation_matrix_layout.cpp"
 "layout/synth/unison_layout.cpp"
 
 "layout/modulators/envelop_layout.cpp"
 "layout/synth/synth_layout.cpp"
 "layout/synth/phase_layout.cpp"
 "layout/synth/filter_route_layout.cpp"
 "layout/synth/final_filter_layout.cpp"
 "layout/modulators/modulators_layout.cpp"
 "layout/effect/final_fx_layout.cpp"
 "layout/about/about_layout.cpp"
 "layout/master_layout.cpp"
 "ui/spectrum.cpp"
 "layout/modulators/modulator_button.cpp" "layout/cruve/curve_layout.cpp" "ui/quantize_map_editor.cpp" "ui/common_curve_editor.cpp")
set(UI_HEADER
"layout/modulators/envelop_layout.h"
 "layout/synth/synth_layout.h"
 "layout/synth/phase_layout.h"
 "layout/synth/filter_route_layout.h"
 "layout/synth/final_filter_layout.h"
 "layout/modulators/modulators_layout.h"
 
 "layout/effect/final_fx_layout.h"
 "layout/about/about_layout.h"
 "layout/master_layout.h"
 "ui/spectrum.h"
 "layout/synth/dissonance_layout.h"
"ui/wrap_drop_box.h"
"layout/synth/timber_layout.h"
#"ui/oscilloscope.h"
 "layout/synth/filter_layout.h"
 "layout/effect/sp_effect_layout.h"
 "layout/gui_param_pack.h"
 "layout/resynthsis/resynthsis_layout.h"
 
  "layout/synth/osc_layout.h"
#"ui/curve_editor.h"
  #"layout/curve_layout.h"
  "layout/main_window.h"
  "ui/wrap_slider.h"
 "ui/wrap_check_box.h"
 "layout/modulators/lfo_layout.h"
 "layout/matrix/modulation_matrix_layout.h"
 "layout/synth/unison_layout.h"
 
 "layout/modulators/modulator_button.h" "layout/modu_listener.h" "layout/cruve/curve_layout.h" "ui/quantize_map_editor.h" "ui/common_curve_editor.h")

target_sources(Harsh
    PRIVATE
        PluginEditor.cpp
        PluginProcessor.cpp 
        "data/juce_param.h"
        "data/juce_param.cpp"
        "data/juce_param_creator.h"
        "data/juce_param_creator.cpp"
        ${UI_SRC}
        ${UI_HEADER})
set_property(TARGET Harsh PROPERTY CXX_STANDARD 20)
target_include_directories(Harsh PRIVATE ".")

# `target_compile_definitions` adds some preprocessor definitions to our target. In a Projucer
# project, these might be passed in the 'Preprocessor Definitions' field. JUCE modules also make use
# of compile definitions to switch certain features on/off, so if there's a particular feature you
# need that's not on by default, check the module header for the correct flag to set here. These
# definitions will be visible both to your code, and also the JUCE module code, so for new
# definitions, pick unique names that are unlikely to collide! This is a standard CMake command.

target_compile_definitions(Harsh
    PUBLIC
        # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
        JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
        JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
        JUCE_VST3_CAN_REPLACE_VST2=0)

# If your target needs extra binary assets, you can add them here. The first argument is the name of
# a new static library target that will include all the binary resources. There is an optional
# `NAMESPACE` argument that can specify the namespace of the generated binary data class. Finally,
# the SOURCES argument should be followed by a list of source files that should be built into the
# static library. These source files can be of any kind (wav data, images, fonts, icons etc.).
# Conversion to binary-data will happen when your target is built.

# juce_add_binary_data(AudioPluginData SOURCES ...)

# `target_link_libraries` links libraries and JUCE modules to other libraries or executables. Here,
# we're linking our executable target to the `juce::juce_audio_utils` module. Inter-module
# dependencies are resolved automatically, so `juce_core`, `juce_events` and so on will also be
# linked automatically. If we'd generated a binary data target above, we would need to link to it
# here too. This is a standard CMake command.

target_link_libraries(Harsh
    PRIVATE
        # AudioPluginData           # If we'd created a binary data target, we'd link to it here
        juce::juce_audio_utils
        juce::juce_gui_basics
        AdSynthLib
        AudioFile
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
