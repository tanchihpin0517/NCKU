/*
  ==============================================================================

    Global.h
    Created: 18 Oct 2021 9:40:41pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <vector>

#define RESOLUTION 2048

namespace Global
{

extern std::vector<float> *waveTable;
extern std::vector<float> sineWaveTable;
extern std::vector<float> recWaveTable;
extern std::vector<float> triWaveTable;
extern std::vector<float> sawWaveTable;

void initWaveTable();

}
