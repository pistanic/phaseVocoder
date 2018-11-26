/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

#define DETECTION_GROUP 1




//==============================================================================
/***/
class phaseVocoAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener//, private Button::Listener, public Timer
{
public:
    phaseVocoAudioProcessorEditor (phaseVocoAudioProcessor&);
    ~phaseVocoAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
                    Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
                    juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener = true);
    
    //void initButton(Button* btn, String btnName, int buttonGroup);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    phaseVocoAudioProcessor& processor;
    
    //Add some sliders for parameters
    Slider slOutGain; //slRatio, slThreshold, slInGain,slAtkTime, slRelTime, slLevel
    
    //ToggleButton btnPeak, btnRMS;
    
    //Slider callback function (from Slider::Listener)
    void sliderValueChanged(Slider* slider) override;
    
    //Button callback function
    //void buttonClicked(Button* button) override;
    
    //Time to periodically update things on the GUI?
    //void timerCallback() override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (phaseVocoAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
