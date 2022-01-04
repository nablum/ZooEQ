/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//Custor rotary slider
struct customRotarySlider : juce::Slider
{
    customRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {
        
    }
};


//==============================================================================
/**
*/
class ZooEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ZooEQAudioProcessorEditor (ZooEQAudioProcessor&);
    ~ZooEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ZooEQAudioProcessor& audioProcessor;
    
    customRotarySlider peakFreqSlider,
    peakGainSlider,
    peakQualitySlider,
    lowCutFreqSlider,
    highCutFreqSlider,
    lowCutSlopeSlider,
    highCutSlopeSlider;
    
    std::vector<juce::Component*> getComps();
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ZooEQAudioProcessorEditor)
};
