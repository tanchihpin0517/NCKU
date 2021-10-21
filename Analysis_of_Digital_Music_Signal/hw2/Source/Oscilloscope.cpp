/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 20 Oct 2021 1:49:36pm
    Author:  tanch

  ==============================================================================
*/

#include "Oscilloscope.h"
#include <cmath>
#include "Util.h"

namespace oscilloscope
{

Buffer buffer;

void init(int block_size)
{
    /* init buffers */
    buffer = Buffer(block_size);
}

Oscilloscope::Oscilloscope(MySynthAudioProcessor& p)
    : _processor(p)
{
    startTimerHz(30);
}

Oscilloscope::~Oscilloscope() {}

void Oscilloscope::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour(50, 50, 50));
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 3);

    g.setColour(juce::Colours::white);
    drawWave(g);
    //g.strokePath(spectrumPath, juce::PathStrokeType(1.f));

    //drawTextLabels(g);
}

void Oscilloscope::resized() {}

void Oscilloscope::timerCallback() { repaint(); }

void Oscilloscope::drawWave(juce::Graphics& g)
{
    auto samples = buffer.read();
    auto bounds = getLocalBounds();
    
    util::drawPath(g, bounds, samples);
}

}
