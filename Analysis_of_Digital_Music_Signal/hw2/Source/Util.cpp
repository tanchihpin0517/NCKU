/*
  ==============================================================================

    Util.cpp
    Created: 21 Oct 2021 10:46:15pm
    Author:  tanch

  ==============================================================================
*/

#include "Util.h"
#include <algorithm>
#include <memory>

static std::unique_ptr<juce::dsp::FFT> fft_func;
static std::unique_ptr<juce::dsp::WindowingFunction<float>> win_func;

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
    auto path_value = mapLinear(points, -1.0, 1.0, bottom, top);
    
    path.startNewSubPath(0, path_value[0]);
    for (int i=1; i<points.size(); i++) {
        path.lineTo(i, path_value[i]);
    }
    g.strokePath(path, juce::PathStrokeType(1.0));
}

std::vector<float> fft(std::vector<float> data, const float n_inf)
{
    static bool init = false;
    int fft_order = 10;
    int fft_size = 1 << fft_order;
    if (!init) {
        fft_func.reset(new juce::dsp::FFT(fft_order));
        win_func.reset(new juce::dsp::WindowingFunction<float>(fft_size, juce::dsp::WindowingFunction<float>::blackmanHarris));
    }
    
    /* zero padding */
    std::vector<float> fft_data(fft_size * 2, 0);
    std::copy(data.begin(), data.end(), fft_data.begin());
    
    /* apply window function */
    win_func->multiplyWithWindowingTable(fft_data.data(), fft_size);
    
    /* fft */
    fft_func->performFrequencyOnlyForwardTransform(fft_data.data());
    
    int n_bins = fft_size / 2;
    
    /* normalize */
    for (int i=0; i<n_bins; i++) {
        auto v = fft_data[i];
        if (!std::isinf(v) && !std::isnan(v)) {
            fft_data[i] /= n_bins;
        }
        else {
            fft_data[i] = 0;
        }
    }
    
    /* convert to decibels */
    for (int i=0; i<n_bins; i++) {
        fft_data[i] = juce::Decibels::gainToDecibels(fft_data[i], n_inf);
    }
    
    //return std::vector<float>(fft_data.cbegin(), fft_data.cbegin() + fft_data.size()/2);
    return fft_data;
}

/* Map data from (source_min, source_max) to (target_min, target_max). */
std::vector<float> mapLinear(const std::vector<float>& data, float smin, float smax, float tmin, float tmax)
{
    std::vector<float> v(data.size());
    for (int i=0; i<data.size(); i++) {
        v[i] = juce::jmap(data[i], smin, smax, tmin, tmax);
    }
    return v;
}

std::vector<float> mapLog10(const std::vector<float>& data, float smin, float smax, float tmin, float tmax)
{
    std::vector<float> v(data.size());
    for (int i=0; i<data.size(); i++) {
        v[i] = tmin + (tmax - tmin) * juce::mapFromLog10(data[i], smin, smax);
    }
    return v;
}

}
