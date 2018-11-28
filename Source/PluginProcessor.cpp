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
	, m_forwardFFT(mk_fftOrder)
	, m_reverseFFT(mk_fftOrder)
{
	
	m_fftSize = 1024; 
	m_hopSelectSize = eighthWindow;
	m_windowType = hann;

	m_pitchShift = c; 
	m_note0 = c;
	m_root = c; 
	m_pitchShiftValue = 1.0;
	m_oneOverPitchShift = 1.0;
	m_ratio = 1.0;

	m_synthWindowSize = 1024;
	for (int i = 0; i < 2 * m_fftSize; ++i)
	{
		m_omega.push_back(0.25 * M_PI*i);
	}

	m_fftInit = false;
	m_fftTransformSize = 0;
	m_samplesSinceFFT = 0; 
	m_fftScaleFactor = 0; 

	m_inputBufferSize = 1;
	m_outputBufferSize = 1;
	m_inputBufferWritePosition = 0;
	m_outputBufferWritePosition = 0;
	m_outputBufferReadPosition = 0;

	m_hopSize = 128; // m_fftsize/8
	m_windowBufferPointer = 0; 
	m_synthWindowBufferPointer = 0; 


	m_preparedToPlay = false;
	
	
}

phaseVocoAudioProcessor::~phaseVocoAudioProcessor()
{
	deinitFFT();
	deinitWindow();
	deinitSynthWindow();
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

	//set reset phase information
	
	for (int i = 0; i < 2048; ++i)
	{
		m_omega[i] = 2 * M_PI*i* m_hopSize / m_fftTransformSize;
		for (int j = 0; j < 2; ++j)
		{
			m_phi0[i][j] = 0; 
			m_dphi[i][j] = 0; 
			m_psi[i][j] = 0; 
		}
	}
	
	initFFT(m_fftSize);
	initWindow(m_fftSize, m_windowType);
	initSynthWindow(floor(m_fftSize*m_oneOverPitchShift), m_windowType);

	m_preparedToPlay = true; 
}

void phaseVocoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	deinitFFT();
	deinitWindow();
	deinitSynthWindow();

	m_preparedToPlay = false; 
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

	//Caluclate semitone shift
	m_pitchShift = m_note0 - m_root;
	updatePitch();

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    
	int const numSamples = buffer.getNumSamples();

	int channel, inputWritePosition, samplesSinceFFT;
	int outputWritePosition, outputReadPosition;

	//float linOutGain = powf(10, curOutGain / 20.0);
    
	m_fftSpinLock.enter();

	if (!m_fftInit)
	{
		for (channel = 0; channel < totalNumOutputChannels; ++channel)
		{
			buffer.clear(channel, 0, buffer.getNumSamples());
		}
		m_fftSpinLock.exit();
		return;
	}

    //go through each channel for the current frame
    for (channel = 0; channel < totalNumInputChannels; ++channel)
    {
		//int const k_fftTransformSize = 1024; // m_fftTransformSize;
		//double const k_oneOverPitchShift = m_oneOverPitchShift;
		//Prep resample variables

		double* grain2 = new double[m_fftTransformSize + 1];
		double* grain3 = new double[(int)floor(m_oneOverPitchShift*m_fftTransformSize)];
		double lx;
		double x;
		double dx;
		int ix;
		
		// Single channel input data
		float* channelData = buffer.getWritePointer(channel);
		// Circular Buffers 
		float* inputBufferData = m_inputBuffer.getWritePointer(jmin (channel, (m_inputBuffer.getNumChannels() - 1)));
		float* outputBufferData = m_outputBuffer.getWritePointer(jmin (channel, (m_inputBuffer.getNumChannels() - 1)));
		
		inputWritePosition = m_inputBufferWritePosition;
		outputWritePosition = m_outputBufferWritePosition;
		outputReadPosition = m_outputBufferReadPosition;
		samplesSinceFFT = m_samplesSinceFFT;

		for (int i = 0; i < numSamples; ++i)
		{
			float const in = channelData[i];

			channelData[i] = outputBufferData[outputReadPosition];
			outputBufferData[outputReadPosition] = 0.0;
			if (++outputReadPosition >= m_outputBufferSize)
				outputReadPosition = 0;
			
			inputBufferData[inputWritePosition] = in;
			if (++inputWritePosition >= m_inputBufferSize)
				inputWritePosition = 0;
			if (++samplesSinceFFT >= m_hopSize)
			{
				samplesSinceFFT = 0; 

				int inputBufferStartPosition = (inputWritePosition + m_inputBufferSize - m_fftTransformSize)
													% m_inputBufferSize;

				int inputBufferIndex = inputBufferStartPosition;
				
				// Set img to 0 real to window buffer
				for (int fftBufferIndex = 0; fftBufferIndex < m_fftTransformSize; ++fftBufferIndex)
				{
					m_fftTimeDomain[fftBufferIndex].imag(0.0);
					
					if (fftBufferIndex >= m_windowBufferSize) // saftey check
						m_fftTimeDomain[fftBufferIndex].real(0.0);
					else
						m_fftTimeDomain[fftBufferIndex].real(m_windowBufferPointer[fftBufferIndex]
																*inputBufferData[inputBufferIndex]);
					++inputBufferIndex;
					if (inputBufferIndex >= m_inputBufferSize)
						inputBufferIndex = 0; 
				}
				// take fft
				m_forwardFFT.perform(m_fftTimeDomain, m_fftFrequencyDomain, false);

				// ~`* P H A S E V O C O C O *`~
				for (int i = 0; i < m_fftTransformSize; ++i)
				{
					double amp = sqrt((m_fftFrequencyDomain[i].real() * m_fftFrequencyDomain[i].real()) + (m_fftFrequencyDomain[i].imag()
						* m_fftFrequencyDomain[i].imag()) );
					double phase = atan2(m_fftFrequencyDomain[i].imag(), m_fftFrequencyDomain[i].real());

					m_dphi[i][channel] = m_omega[i] + princeArg(phase - m_phi0[i][channel] - m_omega[i]);
					
					m_phi0[i][channel] = phase; 
					m_psi[i][channel] = princeArg(m_psi[i][channel] + m_dphi[i][channel] * m_ratio);

					m_fftFrequencyDomain[i].real(amp*cos(m_psi[i][channel]));
					m_fftFrequencyDomain[i].imag(amp*sin(m_psi[i][channel]));

				}
				m_reverseFFT.perform(m_fftFrequencyDomain, m_fftTimeDomain, true);

				// Interpolate
				for (int i = 0; i < m_fftTransformSize; ++i)
					grain2[i] = m_fftTimeDomain[i].real();
				for (int i = 0; i < floor(m_oneOverPitchShift*m_fftTransformSize); ++i)
				{
					lx = floor(m_oneOverPitchShift*m_fftTransformSize);
					x = i * m_fftTransformSize / lx;
					ix = floor(x);
					dx = x - (double)ix;
					grain3[i] = grain2[ix] * (1.0 - dx) + grain2[ix + 1] * dx;
				}

				int outputBufferIndex = outputWritePosition;
				
				// Synthesize
				for (int fftBufferIndex = 0; fftBufferIndex < floor(m_oneOverPitchShift*m_fftTransformSize); ++fftBufferIndex)
				{
					if (fftBufferIndex > m_synthWindowBufferSize)
						outputBufferData[outputBufferIndex] += 0;
					else
						outputBufferData[outputBufferIndex] += grain3[fftBufferIndex] * m_fftScaleFactor * m_synthWindowBufferPointer[fftBufferIndex];
					if (++outputBufferIndex >= m_outputBufferSize)
						outputBufferIndex = 0;
				}
				outputWritePosition = (outputWritePosition + m_hopSize) % m_outputBufferSize;
						
			}

		}

		/*
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
		*/
		delete[] grain2;
		delete[] grain3;
    }
	// update state variabeles
	m_inputBufferWritePosition = inputWritePosition;
	m_outputBufferWritePosition = outputWritePosition;
	m_outputBufferReadPosition = outputReadPosition;

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	{
		buffer.clear(i, 0, buffer.getNumSamples());
	}

	m_fftSpinLock.exit();
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

void phaseVocoAudioProcessor::initFFT(int length)
{
	if (m_fftInit)
		deinitFFT();

	m_fftTransformSize = length;
	 
	m_inputBufferSize = m_fftTransformSize;
	m_inputBuffer.setSize(2, m_inputBufferSize);
	m_inputBuffer.clear();
	m_inputBufferWritePosition = 0; 
	m_samplesSinceFFT = 0; 

	m_outputBufferSize = 2 * m_fftTransformSize;
	m_outputBuffer.setSize(2, m_outputBufferSize);
	m_outputBuffer.clear();
	m_outputBufferReadPosition = 0; 
	
	updateHopSize();
	updatePitch();

	m_fftInit = true; 
}

void phaseVocoAudioProcessor::deinitFFT()
{
	if (!m_fftInit)
		return;

	m_fftSpinLock.enter();
	m_fftInit = false; 
	m_fftSpinLock.exit(); 

	//free memory 

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
		deinitSynthWindow();
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
	switch (m_hopSelectSize)
	{
		case window :
			m_hopSize = m_fftTransformSize;
			break;
		case halfWindow :
			m_hopSize = m_fftTransformSize / 2;
			break;
		case quarterWindow :
			m_hopSize = m_fftTransformSize / 4;
			break;
		case eighthWindow :
			m_hopSize = m_fftTransformSize / 8;
			break;
	}

	updateScaleFactor();
	m_outputBufferWritePosition = m_hopSize + m_fftTransformSize;
}

void phaseVocoAudioProcessor::updateScaleFactor()
{
	double windowSum = 0.0;

	for (int i = 0; i < m_windowBufferSize; ++i)
		windowSum += m_windowBufferPointer[i];

	if (windowSum == 0.0)
		m_fftScaleFactor = 0.0;  //mute output

	else
	{
		switch (m_hopSelectSize)
		{
			case window : 
				m_fftScaleFactor = 1;
				break;
			case halfWindow :
				m_fftScaleFactor = 2;
				break;
			case quarterWindow :
				m_fftScaleFactor = 4;
				break;
			case eighthWindow :
				m_fftScaleFactor = 8; //what the fuck?
				break;
		}
	}

}

void phaseVocoAudioProcessor::updatePitch()
{
	switch (m_pitchShift)
	{
	case c :
		m_pitchShiftValue = 1.0;
		break;
	case cs :
		m_pitchShiftValue = pow(2.0, (1 / 12));
		break;
	case d :
		m_pitchShiftValue = pow(2.0, 2 * (1 / 12));
		break;
	case ds:
		m_pitchShiftValue = pow(2.0, 3 * (1 / 12));
		break;
	case e : 
		m_pitchShiftValue = pow(2.0, 4 * (1 / 12));
		break;
	case f:
		m_pitchShiftValue = pow(2.0, 5 * (1 / 12));
		break;
	case fs:
		m_pitchShiftValue = pow(2.0, 6 * (1 / 12));
		break;
	case g :
		m_pitchShiftValue = pow(2.0, 7 * (1 / 12));
		break;
	case gs :
		m_pitchShiftValue = pow(2.0, 8 * (1 / 12));
		break;
	case a:
		m_pitchShiftValue = pow(2.0, 9 * (1 / 12));
		break;
	case as:
		m_pitchShiftValue = pow(2.0, 10 * (1 / 12));
		break;
	case b :
		m_pitchShiftValue = pow(2.0, 11 * (1 / 12));
		break; 
	}

	m_ratio = round(m_pitchShiftValue*m_hopSize) / m_hopSize;
	m_oneOverPitchShift = 1 / m_pitchShiftValue;

}

double phaseVocoAudioProcessor::princeArg(double inputPhase)
{
	if (inputPhase >= 0)
		return fmod(inputPhase + M_PI, 2 * M_PI) - M_PI;
	else
		return fmod(inputPhase + M_PI, -2 * M_PI) + M_PI;
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new phaseVocoAudioProcessor();
}



