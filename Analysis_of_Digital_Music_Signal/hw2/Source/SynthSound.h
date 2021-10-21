/*
  ==============================================================================

    SynthSound.h
    Created: 20 Oct 2021 11:45:13am
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
