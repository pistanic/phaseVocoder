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
	void makeVisibleNotes(int num);
    void initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
                    Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
                    juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener = true);
    
private:
    phaseVocoAudioProcessor& processor;
	
	Rectangle<int> rectArea[12];
	
    Slider slOutGain;
	Label labelGain;

	ComboBox comBoxRoot, comBoxShiftDir, comBoxNumNotes;
	Label	labelRoot, labelShiftDir, labelNumNotes;

	ComboBox comBoxNote0, comBoxNote1, comBoxNote2, comBoxNote3, comBoxNote4, comBoxNote5;
	Label	labelNote0, labelNote1, labelNote2, labelNote3, labelNote4, labelNote5;

    void sliderValueChanged(Slider* slider) override;
	void comboBoxChanged(ComboBox* comboBox) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (phaseVocoAudioProcessorEditor)
};

#endif
