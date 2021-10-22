/*
  ==============================================================================

    Util.h
    Created: 21 Oct 2021 10:46:15pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

namespace util
{

void drawPath(juce::Graphics& g, juce::Rectangle<int> &bounds, std::vector<float> samples);

std::vector<float> fft(std::vector<float> data, const float n_inf);
std::vector<float> mapLinear(const std::vector<float>& data, float smin, float smax, float tmin, float tmax);
std::vector<float> mapLog10(const std::vector<float>& data, float smin, float smax, float tmin, float tmax);

}
