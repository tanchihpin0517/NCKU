/*
  ==============================================================================

    Spectrogram.cpp
    Created: 20 Oct 2021 1:49:03pm
    Author:  tanch

  ==============================================================================
*/

#include "Spectrogram.h"
#include <algorithm>
#include <assert.h>

namespace spectrogram
{

Spectrogram::Spectrogram(MySynthAudioProcessor& p)
    : _processor(p)
{
}

Spectrogram::~Spectrogram() {}

void Spectrogram::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour(50, 50, 50));
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 3);

    //drawBackgroundGrid(g);

    g.setColour(juce::Colours::white);
    //g.strokePath(spectrumPath, juce::PathStrokeType(1.f));

    //drawTextLabels(g);
}

void Spectrogram::resized() {}

void Spectrogram::timerCallback() {}

}
