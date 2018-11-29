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

	labelShiftDir.setText("Shift pitch up : ", dontSendNotification);
	labelShiftDir.attachToComponent(&comBoxShiftDir, true);
	comBoxShiftDir.addItemList({ "DOWN", "UP"}, 1);
	comBoxShiftDir.setSelectedItemIndex(1, false);
	comBoxShiftDir.setJustificationType(Justification::centred);
	comBoxShiftDir.addListener(this);
	addAndMakeVisible(comBoxShiftDir);

	labelNumNotes.setText("Choose # of Notes : ", dontSendNotification);
	labelNumNotes.attachToComponent(&comBoxNumNotes, false);
	comBoxNumNotes.addItemList({ "1", "2", "3", "4", "5", "6"}, 1);
	comBoxNumNotes.setSelectedItemIndex(0, false);
	comBoxNumNotes.setJustificationType(Justification::centred);
	comBoxNumNotes.addListener(this);
	addAndMakeVisible(comBoxNumNotes);

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
	
	labelNote1.setText("Choose Note - 1 : ", dontSendNotification);
	labelNote1.attachToComponent(&comBoxNote1, false);
	comBoxNote1.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote1.setSelectedItemIndex(0, false);
	comBoxNote1.setJustificationType(Justification::centred);
	comBoxNote1.addListener(this);
	addAndMakeVisible(comBoxNote1);
	
	labelNote2.setText("Choose Note - 3 : ", dontSendNotification);
	labelNote2.attachToComponent(&comBoxNote2, false);
	comBoxNote2.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote2.setSelectedItemIndex(0, false);
	comBoxNote2.setJustificationType(Justification::centred);
	comBoxNote2.addListener(this);
	addAndMakeVisible(comBoxNote2);
	
	labelNote3.setText("Choose Note - 4 : ", dontSendNotification);
	labelNote3.attachToComponent(&comBoxNote3, false);
	comBoxNote3.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote3.setSelectedItemIndex(0, false);
	comBoxNote3.setJustificationType(Justification::centred);
	comBoxNote3.addListener(this);
	addAndMakeVisible(comBoxNote3);
	
	labelNote4.setText("Choose Note - 5 : ", dontSendNotification);
	labelNote4.attachToComponent(&comBoxNote4, false);
	comBoxNote4.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote4.setSelectedItemIndex(0, false);
	comBoxNote4.setJustificationType(Justification::centred);
	comBoxNote4.addListener(this);
	addAndMakeVisible(comBoxNote4);
	
	labelNote5.setText("Choose Note - 6 : ", dontSendNotification);
	labelNote5.attachToComponent(&comBoxNote5, false);
	comBoxNote5.addItemList({ "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" }, 1);
	comBoxNote5.setSelectedItemIndex(0, false);
	comBoxNote5.setJustificationType(Justification::centred);
	comBoxNote5.addListener(this);
	addAndMakeVisible(comBoxNote5);

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
    comBoxNumNotes.setBounds(rectArea[2]);
    //comBoxPreset.setBounds(rectArea[3]);
	slOutGain.setBounds(rectArea[10]);
    comBoxNote0.setBounds(rectArea[4]);
    comBoxNote1.setBounds(rectArea[5]);
    comBoxNote2.setBounds(rectArea[6]);
    comBoxNote3.setBounds(rectArea[7]);
    comBoxNote4.setBounds(rectArea[8]);
    comBoxNote5.setBounds(rectArea[9]);
	comBoxShiftDir.setBounds(rectArea[11]);
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
		processor.updatePitch(processor.m_root, 0/*Voice index*/, processor.m_voiceParamsVector);
	}
	else if (comboBox == &comBoxNote0)
	{
		processor.m_note0 = comboBox->getSelectedItemIndex();
		processor.updatePitch(processor.m_note0, 0/*Voice Index*/, processor.m_voiceParamsVector);
	}
<<<<<<< HEAD
	
}

void phaseVocoAudioProcessorEditor::buttonClicked(Button* button)
{
	if (button == &togshiftupdown)
=======
	else if (comboBox == &comBoxNote1)
>>>>>>> editor
	{
		processor.m_note1 = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNote2)
	{
		processor.m_note2 = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNote3)
	{
		processor.m_note3 = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNote4)
	{
		processor.m_note4 = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNote5)
	{
		processor.m_note5 = comboBox->getSelectedItemIndex();
	}
	else if (comboBox == &comBoxNumNotes)
	{
		makeVisibleNotes(comboBox->getSelectedItemIndex());
	}
	else if (comboBox == &comBoxShiftDir)
	{
		if (comboBox->getSelectedItemIndex() == 0)
			processor.m_shiftUP = false;
		else if(comboBox->getSelectedItemIndex() == 1)
			processor.m_shiftUP = true;
	}

	processor.updatePitch();
}

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

void phaseVocoAudioProcessorEditor::makeVisibleNotes(int num)
{
	switch(num)
	{
		case 0:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(false);
			comBoxNote2.setVisible(false);
			comBoxNote3.setVisible(false);
			comBoxNote4.setVisible(false);
			comBoxNote5.setVisible(false);
			break;
		case 1:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(true);
			comBoxNote2.setVisible(false);
			comBoxNote3.setVisible(false);
			comBoxNote4.setVisible(false);
			comBoxNote5.setVisible(false);
			break;
		case 2:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(true);
			comBoxNote2.setVisible(true);
			comBoxNote3.setVisible(false);
			comBoxNote4.setVisible(false);
			comBoxNote5.setVisible(false);
			break;
		case 3:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(true);
			comBoxNote2.setVisible(true);
			comBoxNote3.setVisible(true);
			comBoxNote4.setVisible(false);
			comBoxNote5.setVisible(false);
			break;
		case 4:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(true);
			comBoxNote2.setVisible(true);
			comBoxNote3.setVisible(true);
			comBoxNote4.setVisible(true);
			comBoxNote5.setVisible(false);
			break;
		case 5:
			comBoxNote0.setVisible(true);
			comBoxNote1.setVisible(true);
			comBoxNote2.setVisible(true);
			comBoxNote3.setVisible(true);
			comBoxNote4.setVisible(true);
			comBoxNote5.setVisible(true);
			break;
	}
	comBoxNote0.setSelectedItemIndex(0, false);
	comBoxNote1.setSelectedItemIndex(0, false);
	comBoxNote2.setSelectedItemIndex(0, false);
	comBoxNote3.setSelectedItemIndex(0, false);
	comBoxNote4.setSelectedItemIndex(0, false);
	comBoxNote5.setSelectedItemIndex(0, false);
}