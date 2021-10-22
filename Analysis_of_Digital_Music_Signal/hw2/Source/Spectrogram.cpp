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
    startTimerHz(30);
}

Spectrogram::~Spectrogram() {}

void Spectrogram::paint(juce::Graphics& g)
{
    g.fillAll (juce::Colour(50, 50, 50));
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 3);

    g.setColour(juce::Colours::dimgrey);
    drawGrid(g);

    g.setColour(juce::Colours::white);
    drawSpectrogram(g);
    
    g.setColour(juce::Colours::lightgrey);
    g.setFont(10);
    drawLabels(g);
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
    auto left = bounds.getX();
    auto right = bounds.getRight();
    auto top = bounds.getY();
    auto bottom = bounds.getBottom();
    
    auto n_inf = -48;
    auto fft_data = util::fft(samples, n_inf); // with aliasing
    auto w_bins = _processor.getSampleRate() / ((float)fft_data.size() * 2);
    
    juce::Path path;
    auto fft_data_db = util::mapLinear(fft_data, n_inf, 0.0, bottom, top);
    path.startNewSubPath(0, fft_data_db[0]);
    for (int i=0; i<fft_data.size(); i++) {
        auto freq = i * w_bins;
        int x = std::floor(juce::mapFromLog10(freq, 20.0, 20000.0) * (right - left));
        path.lineTo(x, fft_data_db[i]);
    }
    g.strokePath(path, juce::PathStrokeType(1.0));
    //util::drawPath(g, bounds, fft_data);
}

void Spectrogram::drawLabels(juce::Graphics& g)
{
    auto bounds = getAnalysisArea();
    auto left = bounds.getX();
    auto right = bounds.getRight();
    auto top = bounds.getY();
    auto bottom = bounds.getBottom();

    auto freq_xs = util::mapLog10(_freq_labels, 20, 20000, left, right);
    for(int i = 0; i < _freq_labels.size(); i++) {
        auto f = _freq_labels[i];
        auto x = freq_xs[i];
        juce::String str;
        
        if (f >= 1000.0) {
            str << f/1000 << "k Hz";
        }
        else {
            str << f << " Hz";
        }

        auto textWidth = g.getCurrentFont().getStringWidth(str);
        juce::Rectangle<int> r;
        
        r.setSize(textWidth, g.getCurrentFont().getHeight());
        r.setCentre(x, 0);
        r.setY(1);
        
        g.drawFittedText(str, r, juce::Justification::centred, 1);
    }
    
    auto db_ys = util::mapLinear(_db_labels, -48, 0, bottom, top);
    for (int i=0; i<_db_labels.size(); i++) {
        auto db = _db_labels[i];
        auto y = db_ys[i];
        juce::String str;
        
        if (db > 0) {
            str << "+" << db;
        }
        else {
            str << db;
        }
        
        auto textWidth = g.getCurrentFont().getStringWidth(str);
        juce::Rectangle<int> r;
        
        r.setSize(textWidth, g.getCurrentFont().getHeight());
        r.setX(1);
        r.setCentre(r.getCentreX(), y);
        
        g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
    }
}

juce::Rectangle<int> Spectrogram::getRenderArea()
{
    auto bounds = getLocalBounds();
    
    bounds.removeFromTop(12);
    bounds.removeFromBottom(2);
    bounds.removeFromLeft(20);
    bounds.removeFromRight(20);
    
    return bounds;
}


juce::Rectangle<int> Spectrogram::getAnalysisArea()
{
    auto bounds = getRenderArea();
    bounds.removeFromTop(4);
    bounds.removeFromBottom(4);
    return bounds;
}

}
