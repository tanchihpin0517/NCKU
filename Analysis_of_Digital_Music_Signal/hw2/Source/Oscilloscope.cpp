/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 20 Oct 2021 1:49:36pm
    Author:  tanch

  ==============================================================================
*/

#include "Oscilloscope.h"

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
    startTimerHz(10);
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
    auto sample = buffer.read();
    auto bounds = getLocalBounds();
    auto top = bounds.getY();
    auto bottom = bounds.getHeight();

    juce::Path path;
    path.startNewSubPath(0, juce::jmap(sample[0], -1.0f, 1.0f, (float)bottom, (float)top));
    for (int i=1; i<sample.size(); i++) {
        path.lineTo(i, juce::jmap(sample[i], -1.0f, 1.0f, (float)bottom, (float)top));
    }
    g.strokePath(path, juce::PathStrokeType(1.0));
}

}
