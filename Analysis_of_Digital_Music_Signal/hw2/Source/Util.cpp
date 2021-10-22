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
    
    /* normalize */
    for (int i=0; i<fft_size; i++) {
        auto v = fft_data[i];
        if (!std::isinf(v) && !std::isnan(v)) {
            fft_data[i] /= fft_size;
        }
        else {
            fft_data[i] = 0;
        }
    }
    
    /* convert to decibels */
    for (int i=0; i<fft_size; i++) {
        fft_data[i] = juce::Decibels::gainToDecibels(fft_data[i], n_inf);
    }
    
    return std::vector<float>(fft_data.cbegin(), fft_data.cbegin() + fft_size);
    //return fft_data;
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
