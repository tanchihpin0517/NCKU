/*
  ==============================================================================

    Global.cpp
    Created: 20 Oct 2021 11:45:43am
    Author:  tanch

  ==============================================================================
*/

#include "WaveTable.h"
#include <cmath>
#include <JuceHeader.h>

namespace wavetable
{

std::vector<float> *sel;
std::vector<float> sin;
std::vector<float> rec;
std::vector<float> tri;
std::vector<float> saw;

void init() {
    sel = &sin;

    int resolution = RESOLUTION;
    int hres = resolution / 2;
    int qres = resolution / 4;

    /* sine */
    for (int i=0; i<resolution; i++) {
        float rad = juce::MathConstants<float>::twoPi * (i / (float)resolution);
        sin.push_back(std::sin(rad));
    }

    /* rectangle */
    for (int i=0; i<hres; i++) {
        rec.push_back(1.0);
    }
    for (int i=0; i<resolution/2; i++) {
        rec.push_back(-1.0);
    }

    /* triangle */
    for (int i=0; i<qres; i++) {
        tri.push_back(i / (float)qres);
    }
    for (int i=0; i<qres; i++) {
        tri.push_back(1.0 - (i / (float)qres));
    }
    for (int i=0; i<qres; i++) {
        tri.push_back(-i / (float)qres);
    }
    for (int i=0; i<qres; i++) {
        tri.push_back(-1.0 + (i / (float)qres));
    }

    /* sawtooth */
    for (int i=0; i<hres; i++) {
        saw.push_back(i / (float)hres);
    }
    for (int i=0; i<hres; i++) {
        saw.push_back(-1.0 + i / (float)hres);
    }
}

} // wavetable
