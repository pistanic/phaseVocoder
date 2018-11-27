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
class phaseVocoAudioProcessorEditor  : public AudioProcessorEditor, private Slider::Listener, private ComboBox::Listener
{
public:
    phaseVocoAudioProcessorEditor (phaseVocoAudioProcessor&);
    ~phaseVocoAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	void calcRectAreas();
    void initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
                    Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
                    juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener = true);
    
private:
    phaseVocoAudioProcessor& processor;
	
	Rectangle<int> rectArea[11];
	
    Slider slOutGain;

	ComboBox comBoxRoot;
	Label	labelRoot;

	ComboBox comBoxNote0;
	Label	labelNote0;

    void sliderValueChanged(Slider* slider) override;
	void comboBoxChanged(ComboBox* comboBox) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (phaseVocoAudioProcessorEditor)
};

#endif
