/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Hw1AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Hw1AudioProcessorEditor (Hw1AudioProcessor&);
    ~Hw1AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void waveStyleMenuChanged();

    Hw1AudioProcessor& audioProcessor;

    juce::Label _textLabel { {}, "Wave style:" };
    juce::Font _textFont { 18.0f };
    juce::ComboBox _waveStyleMenu;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Hw1AudioProcessorEditor)
};
