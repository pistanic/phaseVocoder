/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ctime"


//==============================================================================
phaseVocoAudioProcessorEditor::phaseVocoAudioProcessorEditor (phaseVocoAudioProcessor& p)
: AudioProcessorEditor (&p), processor (p)
{
    setSize (600, 300);

    initSlider(&slOutGain, Slider::LinearHorizontal, "Gain", Slider::TextBoxBelow, false, " db", -20.0, 12.0, 0.1, 0.0);
	sliderValueChanged(&slOutGain);
	labelGain.setText("Output Gain : ", dontSendNotification);
	labelGain.attachToComponent(&slOutGain, false);

	addAndMakeVisible(togshiftupdown);
	togshiftupdown.addListener(this);
	togshiftupdown.setToggleState(true, dontSendNotification);
	labTogButton.setText("Shift pitch up : ", dontSendNotification);
	labTogButton.attachToComponent(&togshiftupdown, true);
	

	labelRoot.setText("Choose the Root Note : ", dontSendNotification);
	labelRoot.attachToComponent(&comBoxRoot, false);
	comBoxRoot.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}, 1);
	comBoxRoot.setSelectedItemIndex(0, false);
	comBoxRoot.setJustificationType(Justification::centred);			
	comBoxRoot.addListener(this);
	addAndMakeVisible(comBoxRoot);
	
	labelNote0.setText("Choose Note - 0 : ", dontSendNotification);
	labelNote0.attachToComponent(&comBoxNote0, false);
	comBoxNote0.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote0.setSelectedItemIndex(0, false);
	comBoxNote0.setJustificationType(Justification::centred);
	comBoxNote0.addListener(this);
	addAndMakeVisible(comBoxNote0);

    //initButton(&btnPeak, "Peak", DETECTION_GROUP);]
    //btnPeak.triggerClick();    
}

phaseVocoAudioProcessorEditor::~phaseVocoAudioProcessorEditor()
{
}

//==============================================================================
void phaseVocoAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll(Colours::navy);    
    g.setColour (Colours::hotpink);
	g.setFont (12.0f);
	for (int i = 0; i < 12; i++)
	{
		g.drawRect(rectArea[i]);
	}
    //g.drawFittedText("Output Gain", 485, 30, 70, 50, Justification::left, 1);   
}

void phaseVocoAudioProcessorEditor::resized()
{
	calcRectAreas();
    comBoxRoot.setBounds(rectArea[1]);
    //comBoxnumNotes.setBounds(rectArea[2]);
    //comBoxPreset.setBounds(rectArea[3]);
	slOutGain.setBounds(rectArea[10]);
    comBoxNote0.setBounds(rectArea[4]);
	togshiftupdown.setBounds(rectArea[11]);
}

void phaseVocoAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    String slName = slider->getName();
    
    if (slName == "# of Notes")
    {
        //processor.numNotes = slider->getValue();
    }
	else if (slName == "Gain")
	{
		processor.curOutGain = slider->getValue();
	}
}

void phaseVocoAudioProcessorEditor::comboBoxChanged(ComboBox* comboBox)
{	
	if (comboBox == &comBoxRoot)
	{
		processor.m_root = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNote0)
	{
		processor.m_note0 = comboBox->getSelectedItemIndex();
	}
}

void phaseVocoAudioProcessorEditor::buttonClicked(Button* button)
{
	if (button == &togshiftupdown)
	{
		processor.m_togState = button->getToggleState();
	}    
}

//void phaseVocoAudioProcessorEditor::timerCallback()
//{
//    slLevel.setValue(processor.curSampleVal);
//}

//initialize a slider with many parameters
void phaseVocoAudioProcessorEditor::initSlider(Slider* slider, Slider::SliderStyle newStyle, juce::String newName,
                Slider::TextEntryBoxPosition newTxtBoxPos, bool txtIsReadOnly,
                juce::String newSuffix, double newMin, double newMax, double newInc, double newValue, bool addListener)
{
    //use the parameters given
    slider->setSliderStyle(newStyle);
    slider->setName(newName);
    slider->setTextBoxStyle(newTxtBoxPos, txtIsReadOnly, slider->getTextBoxWidth(), slider->getTextBoxHeight());
    slider->setTextValueSuffix(newSuffix);
    slider->setRange(newMin, newMax, newInc);
    slider->setValue(newValue);
    
    //set up apperance
    slider->setColour(Slider::thumbColourId, Colours::deepskyblue);
    slider->setColour(Slider::textBoxHighlightColourId, Colours::red);
    
    addAndMakeVisible(slider);

    if (addListener == true)
    {
        slider->addListener(this);
    }    
}

//void phaseVocoAudioProcessorEditor::initButton(Button* btn, String btnName, int buttonGroup)
//{
//    addAndMakeVisible(btn);
//    btn->addListener(this);
//    btn->setButtonText(btnName);
//    btn->setName(btnName);
//    btn->setColour(ToggleButton::textColourId, Colours::white);
//    btn->setColour(ToggleButton::tickColourId, Colours::deepskyblue);
//    btn->setRadioGroupId(buttonGroup);
//}

void phaseVocoAudioProcessorEditor::calcRectAreas()
{
	Rectangle<int> y = getLocalBounds().reduced(10);
	Rectangle<int> x = y;

	rectArea[0] = x;

	//FIRST COLUMN - BPM, SCALE, TAPS, START-Button
	x = y.removeFromTop(y.getHeight() / 2);
	rectArea[1] = x.removeFromLeft(x.getWidth() / 4);
	rectArea[1] = rectArea[1].removeFromBottom(rectArea[1].getHeight() * 8 / 10 );
	rectArea[1] = rectArea[1].removeFromTop(rectArea[1].getHeight() * 6 / 10 );
	rectArea[2] = x.removeFromLeft(x.getWidth() / 3);
	rectArea[2] = rectArea[2].removeFromBottom(rectArea[2].getHeight() * 8 / 10);
	rectArea[2] = rectArea[2].removeFromTop(rectArea[2].getHeight() * 6 / 10);
	rectArea[11] = x.removeFromLeft(x.getWidth() / 2);
	rectArea[11] = rectArea[11].removeFromBottom(rectArea[11].getHeight() * 8 / 10);
	rectArea[3] = rectArea[11].removeFromTop(rectArea[11].getHeight() * 6 / 10);
	rectArea[11] = rectArea[11].removeFromTop(rectArea[11].getHeight() * 5 / 10);
	rectArea[10] = x;
	rectArea[10] = rectArea[10].removeFromBottom(rectArea[10].getHeight() * 8 / 10);
	rectArea[10] = rectArea[10].removeFromTop(rectArea[10].getHeight() * 8 / 10);
	
	x = y.removeFromTop(y.getHeight() / 2);
	rectArea[4] = x.removeFromLeft(x.getWidth() / 3);
	rectArea[4] = rectArea[4].removeFromBottom(rectArea[4].getHeight() * 8 / 10);
	rectArea[4] = rectArea[4].removeFromTop(rectArea[4].getHeight() * 9 / 10);

	
	rectArea[5] = x.removeFromLeft(x.getWidth() / 2);
	rectArea[5] = rectArea[5].removeFromBottom(rectArea[5].getHeight() * 8 / 10);
	rectArea[5] = rectArea[5].removeFromTop(rectArea[5].getHeight() * 9 / 10);

	rectArea[6] = x;
	rectArea[6] = rectArea[6].removeFromBottom(rectArea[6].getHeight() * 8 / 10);
	rectArea[6] = rectArea[6].removeFromTop(rectArea[6].getHeight() * 9 / 10);

	x = y;
	rectArea[7] = x.removeFromLeft(x.getWidth() / 3);
	rectArea[7] = rectArea[7].removeFromBottom(rectArea[7].getHeight() * 8 / 10);
	rectArea[7] = rectArea[7].removeFromTop(rectArea[7].getHeight() *  9 / 10);

	rectArea[8] = x.removeFromLeft(x.getWidth() / 2);
	rectArea[8] = rectArea[8].removeFromBottom(rectArea[8].getHeight() * 8 / 10);
	rectArea[8] = rectArea[8].removeFromTop(rectArea[8].getHeight() * 9 / 10);

	rectArea[9] = x;
	rectArea[9] = rectArea[9].removeFromBottom(rectArea[9].getHeight() * 8 / 10);
	rectArea[9] = rectArea[9].removeFromTop(rectArea[9].getHeight() * 9 / 10);

}