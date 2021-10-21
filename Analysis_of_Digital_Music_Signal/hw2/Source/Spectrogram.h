/*
  ==============================================================================

    Spectrogram.h
    Created: 20 Oct 2021 1:49:03pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Buffer.h"

namespace spectrogram
{

class Spectrogram : public juce::Component, public juce::Timer
{
public:
    Spectrogram(MySynthAudioProcessor&);
    ~Spectrogram() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
private:
    MySynthAudioProcessor& _processor;
};

}

