/*
  ==============================================================================

    SynthVoice.h
    Created: 20 Oct 2021 11:45:22am
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice(int vid);
    
    bool canPlaySound (juce::SynthesiserSound* sound) override;

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;

    void stopNote (float velocity, bool allowTailOff) override;

    void pitchWheelMoved (int newPitchWheelValue) override;

    void controllerMoved (int controllerNumber, int newControllerValue) override;

    void renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples) override;

    void setLevel(float newLevel);

private:
    float _level;
    float _frequency;
    float _period;
    int _midiNoteNumber;
    float _tailOff;
    int _sampleCount;
    float _samplePeriod;
    bool _noteOn;
    int _vid;
};
