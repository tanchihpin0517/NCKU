/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Spectrogram.h"
#include "Oscilloscope.h"
#include <vector>

//==============================================================================
/**
*/
class MySynthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    MySynthAudioProcessorEditor (MySynthAudioProcessor&);
    ~MySynthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MySynthAudioProcessor& _audioProcessor;
    oscilloscope::Oscilloscope _oscilloscope;
    spectrogram::Spectrogram _spectrogram;
    std::vector<juce::Component*> _components {&_oscilloscope, &_spectrogram};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MySynthAudioProcessorEditor)
};
