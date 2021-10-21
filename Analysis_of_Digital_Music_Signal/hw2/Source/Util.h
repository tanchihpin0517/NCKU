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

void fft();

}
