/*
  ==============================================================================

    Buffer.h
    Created: 21 Oct 2021 1:47:25pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>

class Buffer
{
public:
    Buffer();
    Buffer(int size);
    void write(std::vector<float>& data);
    std::vector<float>& read();
private:
    std::vector<float> _wbuf[2];
    std::vector<float> _rbuf;
    int _wcur;
    int _rdone;
};
