/*
  ==============================================================================

    Global.cpp
    Created: 19 Oct 2021 11:17:00am
    Author:  tanch

  ==============================================================================
*/

#include "Global.h"
#include <cmath>
#include <JuceHeader.h>

namespace Global
{

std::vector<float> *waveTable;
std::vector<float> sineWaveTable;
std::vector<float> recWaveTable;
std::vector<float> triWaveTable;
std::vector<float> sawWaveTable;

void initWaveTable() {
    waveTable = &sineWaveTable;

    int resolution = RESOLUTION;
    int hres = resolution / 2;
    int qres = resolution / 4;

    /* sine */
    for (int i=0; i<resolution; i++) {
        float rad = juce::MathConstants<float>::twoPi * (i / (float)resolution);
        sineWaveTable.push_back(std::sin(rad));
    }

    /* rectangle */
    for (int i=0; i<hres; i++) {
        recWaveTable.push_back(1.0);
    }
    for (int i=0; i<resolution/2; i++) {
        recWaveTable.push_back(-1.0);
    }

    /* triangle */
    for (int i=0; i<qres; i++) {
        triWaveTable.push_back(i / (float)qres);
    }
    for (int i=0; i<qres; i++) {
        triWaveTable.push_back(1.0 - (i / (float)qres));
    }
    for (int i=0; i<qres; i++) {
        triWaveTable.push_back(-i / (float)qres);
    }
    for (int i=0; i<qres; i++) {
        triWaveTable.push_back(-1.0 + (i / (float)qres));
    }

    /* sawtooth */
    for (int i=0; i<hres; i++) {
        sawWaveTable.push_back(i / (float)hres);
    }
    for (int i=0; i<hres; i++) {
        sawWaveTable.push_back(-1.0 + i / (float)hres);
    }
}

}
