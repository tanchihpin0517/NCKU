/*
  ==============================================================================

    Util.cpp
    Created: 21 Oct 2021 10:46:15pm
    Author:  tanch

  ==============================================================================
*/

#include "Util.h"
#include <algorithm>

static int fft_order = 10;
static int fft_size = 1 << fft_order;
static juce::dsp::FFT fft_func(fft_order);
static juce::dsp::WindowingFunction<float> win_func(fft_size, juce::dsp::WindowingFunction<float>::blackmanHarris);

namespace util
{

void drawPath(juce::Graphics& g, juce::Rectangle<int> &bounds, std::vector<float> samples)
{
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
    path.startNewSubPath(0, juce::jmap(points[0], -1.0f, 1.0f, (float)bottom, (float)top));
    for (int i=1; i<points.size(); i++) {
        path.lineTo(i, juce::jmap(points[i], -1.0f, 1.0f, (float)bottom, (float)top));
    }
    g.strokePath(path, juce::PathStrokeType(1.0));
}

void fft(std::vector<float> data)
{
    /* zero padding */
    std::vector<float> fft_data(fft_size * 2, 0);
    std::copy(data.begin(), data.end(), fft_data.begin());
    
    /* apply window function */
    win_func.multiplyWithWindowingTable(fft_data.data(), fft_size);
    
    /* fft */
    fft_func.performFrequencyOnlyForwardTransform(fft_data.data());
}

}
