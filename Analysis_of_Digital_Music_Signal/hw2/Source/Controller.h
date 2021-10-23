/*
  ==============================================================================

    Controller.h
    Created: 23 Oct 2021 12:57:44pm
    Author:  tanch

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include <vector>
#include <memory>

class Controller : public juce::Component
{
public:
    Controller(MySynthAudioProcessor &p);
    ~Controller();
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    MySynthAudioProcessor& _processor;
    juce::ComboBox _waveStyleMenu;
    juce::Slider _levelSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _levelAttachment;

    std::vector<juce::Component*> _components {&_waveStyleMenu, &_levelSlider};

    void waveStyleMenuChanged();
};
