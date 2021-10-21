/*
  ==============================================================================

    SynthVoice.cpp
    Created: 18 Oct 2021 2:31:08pm
    Author:  tanch

  ==============================================================================
*/

#include "SynthVoice.h"
#include "SynthSound.h"
#include "Global.h"

bool SynthVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    _midiNoteNumber = midiNoteNumber;
    _frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    _period = 1 / _frequency;
    _tailOff = 0.0;
    _sampleCount = 0;
    _samplePeriod = 1 / (float)getSampleRate();
    _noteOn = true;
}

void SynthVoice::stopNote (float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        _tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        _noteOn = false;
    }
}

void SynthVoice::pitchWheelMoved (int newPitchWheelValue)
{
    // handle pitch wheel moved midi event
}

void SynthVoice::controllerMoved (int controllerNumber, int newControllerValue)
{
    // handle midi control change
}

void SynthVoice::renderNextBlock (juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples)
{
    if (_noteOn) {
        for (int i = startSample; i < (startSample + numSamples); i++) {
            float elapsedTime = _sampleCount * _samplePeriod;
            float pos = Global::waveTable->size() * (elapsedTime / _period - std::floor(elapsedTime / _period));
            int lp = std::floor(pos);
            int rp = std::ceil(pos);
            float v, lv, rv;

            //DBG(_sampleCount << "," << _samplePeriod);
            //DBG(elapsedTime << "," << pos);
            //DBG(lp << "," << rp);

            if (lp == rp) {
                v = (*Global::waveTable)[lp];
            }
            else {
                lv = (*Global::waveTable)[lp];
                /* avoid overflow */
                rv = (rp == Global::waveTable->size()) ? (*Global::waveTable)[0] : (*Global::waveTable)[rp];
                v = (pos - lp) * rv + (rp - pos) * lv;
            }
            //v = (*Global::waveTable)[lp];
            _sampleCount += 1;


            /* level */
            v *= _level;

            /* tail off */
            if (_tailOff > 0.0) {
                v *= _tailOff;
                _tailOff *= 0.99;

                if (_tailOff <= 0.01) {
                    //DBG(_tailOff);
                    clearCurrentNote();
                    _noteOn = false;
                }
            }

            /* write into buffer */
            outputBuffer.addSample(0, i, v);
            outputBuffer.addSample(1, i, v);
            //DBG(v);
        }
        //DBG(_sampleCount);
    }
}

void SynthVoice::setLevel(float newLevel)
{
    _level = newLevel;
}
