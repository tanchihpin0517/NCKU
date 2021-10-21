/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Global.h"

#define SINE 1
#define REC 2
#define TRI 3
#define SAW 4

//==============================================================================
Hw1AudioProcessorEditor::Hw1AudioProcessorEditor (Hw1AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(_textLabel);
    _textLabel.setFont(_textFont);

    // add items to the combo-box
    addAndMakeVisible (_waveStyleMenu);
    _waveStyleMenu.addItem ("Sine", SINE);
    _waveStyleMenu.addItem ("Rectangle", REC);
    _waveStyleMenu.addItem ("Triangle", TRI);
    _waveStyleMenu.addItem ("Sawtooth", SAW);

    _waveStyleMenu.onChange = [this] { waveStyleMenuChanged(); };
    _waveStyleMenu.setSelectedId (1);

    setSize (400, 300);
}

Hw1AudioProcessorEditor::~Hw1AudioProcessorEditor()
{
}

//==============================================================================
void Hw1AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
}

void Hw1AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    _textLabel.setBounds (10, 10, getWidth() - 20, 20);
    _waveStyleMenu.setBounds (10, 40, getWidth() - 20, 20);
}

void Hw1AudioProcessorEditor::waveStyleMenuChanged()
{
    switch (_waveStyleMenu.getSelectedId())
    {
        case SINE: Global::waveTable = &Global::sineWaveTable; break;
        case REC: Global::waveTable = &Global::recWaveTable; break;
        case TRI: Global::waveTable = &Global::triWaveTable; break;
        case SAW: Global::waveTable = &Global::sawWaveTable; break;
        default: break;
    }

    _textLabel.setFont(_textFont);
}

