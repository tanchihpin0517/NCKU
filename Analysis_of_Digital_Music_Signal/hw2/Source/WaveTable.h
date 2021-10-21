/*
  ==============================================================================

    Global.h
    Created: 20 Oct 2021 11:45:43am
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <vector>

#define RESOLUTION 2048

namespace wavetable
{

extern std::vector<float> *sel;
extern std::vector<float> sin;
extern std::vector<float> rec;
extern std::vector<float> tri;
extern std::vector<float> saw;

void init();

}
