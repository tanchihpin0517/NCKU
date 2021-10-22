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

extern Buffer buffer;
void init(int block_size);

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
    
    std::vector<float> _freq_labels {
        20, /*30, 40,*/ 50, 100,
        200, /*300, 400,*/ 500, 1000,
        2000, /*3000, 4000,*/ 5000, 10000,
        20000
    };
    std::vector<float> _db_labels {
        0, -12, -24, -36, -48
    };
    
    void drawGrid(juce::Graphics& g);
    void drawSpectrogram(juce::Graphics& g);
};

}

