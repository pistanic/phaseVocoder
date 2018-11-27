/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"



//==============================================================================
phaseVocoAudioProcessor::phaseVocoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", AudioChannelSet::stereo(), true)
#endif
                  )
#endif
{
	
	m_fftSize = 1024; 
	m_fftTransformSize = 0;
	m_samplesSinceFFT = 0; 
	m_fftScaleFactor = 0; 

	m_hopSize = eighthWindow;
	m_windowType = hann;
	m_windowBufferPointer = 0; 

	m_synthWindowSize = 1024;
	m_synthWindowBufferPointer = 0; 

	m_pitchShift = c; // No shift
	m_pitchShiftValue = 1.0;
	m_oneOverPitchShift = 1.0; 

	m_ratio = 1.0; 

	int omegaFactor = 0; 

	switch (m_hopSize)
	{
		case window:
		{
			omegaFactor = 2;
			break;
		}
		case halfWindow:
		{
			omegaFactor = 1;
			break;
		}
		case quarterWindow:
		{
			omegaFactor = 0.5;
			break;
		}
		case eighthWindow:
			omegaFactor = 0.25;
			break;
		default:
		{
			omegaFactor = 0;
			break;
		}
	}

	for (int i = 0; i < 2*m_fftSize; ++i)
	{
		m_omega.push_back(omegaFactor * M_PI*i); 
	}
	
	m_inputBufferSize = 1;
	m_outputBufferSize = 1;

	m_inputBufferWritePosition = 0;
	m_outputBufferWritePosition = 0; 
	m_outputBufferReadPosition = 0; 

	m_fftInit = m_preparedToPlay = true;
	
	
}

phaseVocoAudioProcessor::~phaseVocoAudioProcessor()
{
}

//==============================================================================
const String phaseVocoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool phaseVocoAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool phaseVocoAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

double phaseVocoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int phaseVocoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int phaseVocoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void phaseVocoAudioProcessor::setCurrentProgram (int index)
{
}

const String phaseVocoAudioProcessor::getProgramName (int index)
{
    return String();
}

void phaseVocoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void phaseVocoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void phaseVocoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool phaseVocoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}
#endif

void phaseVocoAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    static int counter = 0; //to track number of samples overtime
    
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

	float linOutGain = powf(10, curOutGain / 20.0);
    
    //go through each channel for the current frame
    for (int ch = 0; ch < totalNumInputChannels; ++ch)
    {
        //go through each frame for this callback
        for (int n = 0; n < buffer.getNumSamples(); ++n)
        {
            //get the current sample's sign and absolute value
            float curSample = buffer.getSample(ch, n);
            buffer.setSample(ch, n, curSample * linOutGain);
        }
        
        //calculate RMS after the fact and display it
        float curMag = buffer.getMagnitude(ch, 0, buffer.getNumSamples());
        curSampleVal = curMag; //the displayed power value on the GUI
    }
}


//==============================================================================
bool phaseVocoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* phaseVocoAudioProcessor::createEditor()
{
    return new phaseVocoAudioProcessorEditor (*this);
}

//==============================================================================
void phaseVocoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void phaseVocoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void phaseVocoAudioProcessor::initWindow(int length, int windowType)
{
	if (m_windowBufferPointer != 0)
		deinitWindow();
	if (length == 0)
		return; 
	m_windowBufferPointer = (double *)malloc(length * sizeof(double));

	double windowLength = length; 

	for (int i = 0; i < length; ++i)
	{
		switch (windowType)
		{
			case bart:
				m_windowBufferPointer[i] = (2.0 / (windowLength + 2.0)) * (0.5*(windowLength + 2.0) - abs((double)i - 0.5*windowLength));
				break;
			case hann:
				m_windowBufferPointer[i] = 0.5*(1.0 - cos(2.0*M_PI*(double)i / windowLength));
				break;
			case hamm:
			default:
				m_windowBufferPointer[i] = 0.54 - 0.46*cos(2.0*M_PI*(double)i / windowLength);
				break;
		}
	}
	m_windowBufferSize = length; 
	updateScaleFactor();
}

void phaseVocoAudioProcessor::deinitWindow()
{
	if (m_windowBufferPointer == 0)
		return;
	m_fftSpinLock.enter();
	m_windowBufferSize = 0;
	m_fftSpinLock.exit();
	free(m_windowBufferPointer);
	m_windowBufferPointer = 0; 
}

void phaseVocoAudioProcessor::initSynthWindow(int length, int windowType)
{
	if (m_synthWindowBufferPointer != 0)
		deinitWindow();
	if (length == 0)
		return;
	m_synthWindowBufferPointer = (double *)malloc(length * sizeof(double));

	double windowLength = length;

	for (int i = 0; i < length; ++i)
	{
		switch (windowType)
		{
		case bart:
			m_synthWindowBufferPointer[i] = (2.0 / (windowLength + 2.0)) * (0.5*(windowLength + 2.0) - abs((double)i - 0.5*windowLength));
			break;
		case hann:
			m_synthWindowBufferPointer[i] = 0.5*(1.0 - cos(2.0*M_PI*(double)i / windowLength));
			break;
		case hamm:
		default:
			m_synthWindowBufferPointer[i] = 0.54 - 0.46*cos(2.0*M_PI*(double)i / windowLength);
			break;
		}
	}
	m_synthWindowBufferSize = length;
	updateScaleFactor();

}

void phaseVocoAudioProcessor::deinitSynthWindow()
{
	if (m_synthWindowBufferPointer == 0)
		return;
	m_fftSpinLock.enter();
	m_synthWindowBufferSize = 0;
	m_fftSpinLock.exit();
	free(m_synthWindowBufferPointer);
	m_synthWindowBufferPointer = 0;
}

void phaseVocoAudioProcessor::updateHopSize()
{

}

void phaseVocoAudioProcessor::updateScaleFactor()
{

}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new phaseVocoAudioProcessor();
}



