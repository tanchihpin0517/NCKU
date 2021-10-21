/*
  ==============================================================================

    SynthSound.h
    Created: 18 Oct 2021 2:30:58pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

class SynthSound : public juce::SynthesiserSound
{

public:
    bool appliesToNote (int midiNoteNumber) override
    {
        return true;
    }

    bool appliesToChannel (int midiNoteNumber) override
    {
        return true;
    }
};
