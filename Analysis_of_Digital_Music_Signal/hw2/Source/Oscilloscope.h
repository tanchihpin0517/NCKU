/*
  ==============================================================================

    Oscilloscope.h
    Created: 20 Oct 2021 1:49:36pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Buffer.h"
#include <vector>

namespace oscilloscope
{

extern Buffer buffer;
void init(int block_size);

class Oscilloscope : public juce::Component, public juce::Timer
{
public:
    Oscilloscope(MySynthAudioProcessor&);
    ~Oscilloscope() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    void timerCallback() override;
private:
    MySynthAudioProcessor& _processor;

    void drawWave(juce::Graphics& g);
};

}
