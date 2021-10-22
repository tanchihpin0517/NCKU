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
#include "Util.h"

namespace spectrogram
{

Buffer buffer;

void init(int block_size)
{
    /* init buffers */
    buffer = Buffer(block_size);
}

Spectrogram::Spectrogram(MySynthAudioProcessor& p)
    : _processor(p)
{
    startTimerHz(2);
}

Spectrogram::~Spectrogram() {}

void Spectrogram::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour(50, 50, 50));
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 3);

    //drawBackgroundGrid(g);

    g.setColour(juce::Colours::dimgrey);
    drawGrid(g);
    //g.strokePath(spectrumPath, juce::PathStrokeType(1.f));

    //drawTextLabels(g);
    g.setColour(juce::Colours::white);
    drawSpectrogram(g);
}

void Spectrogram::resized() {}

void Spectrogram::timerCallback() { repaint(); }

void Spectrogram::drawGrid(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto left = bounds.getX();
    auto right = bounds.getRight();
    auto top = bounds.getY();
    auto bottom = bounds.getBottom();
    
    auto grid_xs = util::mapLog10(_freq_labels, 20, 20000, left, right);
    for (auto &grid_x : grid_xs) {
        g.drawVerticalLine(grid_x, top, bottom);
    }
    
    auto db_ys = util::mapLinear(_db_labels, -48, 0, bottom, top);
    for (auto &db_y : db_ys) {
        g.drawHorizontalLine(db_y, left, right);
    }
}

void Spectrogram::drawSpectrogram(juce::Graphics& g)
{
    auto samples = buffer.read();
    auto bounds = getLocalBounds();
    
    auto fft_data = util::fft(samples, -48.0);
    for (int i=0; i<fft_data.size(); i++) {
        if (i%5 == 0) {
            DBG(fft_data[i]);
        }
    }
    
    util::drawPath(g, bounds, fft_data);
}

}
