/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MySynthAudioProcessorEditor::MySynthAudioProcessorEditor (MySynthAudioProcessor& p)
    : AudioProcessorEditor (&p), _audioProcessor (p), _oscilloscope(p), _spectrogram(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

    for (auto& comp : _components) {
        addAndMakeVisible(comp);
    }
}

MySynthAudioProcessorEditor::~MySynthAudioProcessorEditor()
{
}

//==============================================================================
void MySynthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void MySynthAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds();

    juce::FlexBox upFlexBox;
    upFlexBox.flexDirection = juce::FlexBox::Direction::row;
    upFlexBox.items.add(juce::FlexItem(_oscilloscope).withFlex(1.0f));
    upFlexBox.items.add(juce::FlexItem(_spectrogram).withFlex(1.0f));

    juce::FlexBox bottomFlexBox;
    bottomFlexBox.flexDirection = juce::FlexBox::Direction::row;

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.items.add(juce::FlexItem(upFlexBox).withFlex(3.0f));
    flexBox.items.add(juce::FlexItem(bottomFlexBox).withFlex(1.0f));
    flexBox.performLayout(area.reduced(10));

    for (auto& comp : _components) {
        comp->setBounds(comp->getBounds().reduced(3));
    }
}
