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
    auto top = bounds.getY();
    auto bottom = bounds.getHeight();
    auto width = bounds.getWidth();
    
    /* fit samples to the width of UI */
    std::vector<float> points(width);
    for (int i=0; i<width; i++) {
        float pos = samples.size() * i / (float)width;
        int lp = std::floor(pos);
        int rp = std::ceil(pos);
        if (lp == rp || rp == samples.size()) {
            points[i] = samples[lp];
        }
        else {
            points[i] = (pos - lp) * samples[rp] + (rp - pos) * samples[lp];
        }
    }

    /* draw UI */
    juce::Path path;
    auto path_value = util::mapLinear(points, -1.0, 1.0, bottom, top);
    
    path.startNewSubPath(0, path_value[0]);
    for (int i=1; i<points.size(); i++) {
        path.lineTo(i, path_value[i]);
    }
    g.strokePath(path, juce::PathStrokeType(1.0));
}

}
