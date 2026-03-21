cmake_minimum_required(VERSION 3.22)
project(ErinSubSynth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.12
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(ErinSubSynth
    COMPANY_NAME "Erin"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    PLUGIN_MANUFACTURER_CODE Erin
    PLUGIN_CODE Esbs
    FORMATS VST3
    PRODUCT_NAME "Erin Sub Synth"
)

target_sources(ErinSubSynth PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/Voice.cpp
    src/LFO.cpp
    src/LookAndFeel.cpp
)

target_include_directories(ErinSubSynth PRIVATE src)

target_compile_definitions(ErinSubSynth PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(ErinSubSynth
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

# Tests
enable_testing()

add_executable(ErinSubSynthTests
    test/DSPTest.cpp
    src/Voice.cpp
    src/LFO.cpp
)

target_include_directories(ErinSubSynthTests PRIVATE src)

target_link_libraries(ErinSubSynthTests PRIVATE
    juce::juce_audio_basics
    juce::juce_core
)

target_compile_definitions(ErinSubSynthTests PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

add_test(NAME DSPTests COMMAND ErinSubSynthTests)
