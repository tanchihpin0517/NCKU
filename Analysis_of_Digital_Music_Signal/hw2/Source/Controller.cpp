/*
  ==============================================================================

    Controller.cpp
    Created: 23 Oct 2021 12:57:44pm
    Author:  tanch

  ==============================================================================
*/

#include "Controller.h"
#include "WaveTable.h"

#define SIN 1
#define REC 2
#define TRI 3
#define SAW 4

Controller::Controller(MySynthAudioProcessor &p)
    :_processor(p)
{
    for (auto &comp: _components) {
        addAndMakeVisible(comp);
    }

    /* wave style menu */
    _waveStyleMenu.addItem ("Sine", SIN);
    _waveStyleMenu.addItem ("Rectangle", REC);
    _waveStyleMenu.addItem ("Triangle", TRI);
    _waveStyleMenu.addItem ("Sawtooth", SAW);
    _waveStyleMenu.onChange = [this] { waveStyleMenuChanged(); };
    _waveStyleMenu.setSelectedId (1);

    /* level slider */
    _levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        _processor._state,
        juce::String{"level"},
        _levelSlider
    );

    /* level slider */
    _levelSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
}

Controller::~Controller() {}

void Controller::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour(50, 50, 50));
}

void Controller::resized()
{
    juce::FlexBox flexBox;
    for (auto &comp: _components) {
        juce::FlexItem item{*comp};
        flexBox.items.add(item.withFlex(1.0));
    }
    flexBox.performLayout(getLocalBounds());
    for (auto &comp: _components) {
        comp->setBounds(comp->getBounds().reduced(5));
    }
}

void Controller::waveStyleMenuChanged()
{
    switch (_waveStyleMenu.getSelectedId())
    {
        case SIN: wavetable::sel = &wavetable::sin; break;
        case REC: wavetable::sel = &wavetable::rec; break;
        case TRI: wavetable::sel = &wavetable::tri; break;
        case SAW: wavetable::sel = &wavetable::saw; break;
        default: break;
    }
}
