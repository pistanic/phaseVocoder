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
	m_note0 = m_note1 = m_note2 = m_note3 = m_note4 = m_note5 = c;
	//m_numVoicesScaleFactor = 0;
	m_root = c; 

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

	//m_voiceParamsVector.reserve(6);
	voiceParams emptyVoiceParams;

	for (int i = 0; i < 6/*Max Number of Voices*/; ++i)
	{
		m_voiceParamsVector.push_back(emptyVoiceParams);
	}

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
	initSynthWindow(floor(m_fftSize*m_voiceParamsVector.at(0).oneOverPitchShift), m_windowType);

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
		double** grain2 = new double*[6];
		for (int i = 0; i < 6; ++i)
			grain2[i] = new double[m_fftTransformSize + 1];

		double** grain3 = new double*[6];
		for (int i = 0; i < 6; ++i)
			grain3[i] = new double[(int)floor(m_voiceParamsVector.at(i).oneOverPitchShift*m_fftTransformSize)];
		
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
					m_fftTimeDomainPre[fftBufferIndex].imag(0.0);

					if (fftBufferIndex >= m_windowBufferSize) // saftey check
						m_fftTimeDomainPre[fftBufferIndex].real(0.0);
					else
						m_fftTimeDomainPre[fftBufferIndex].real(m_windowBufferPointer[fftBufferIndex]
							* inputBufferData[inputBufferIndex]);
					++inputBufferIndex;
					if (inputBufferIndex >= m_inputBufferSize)
						inputBufferIndex = 0;
				}
				// take fft

				m_forwardFFT.perform(m_fftTimeDomainPre, m_fftFrequencyDomainPre, false);

				// ~`* P H A S E V O C O C O *`~

				for (int i = 0; i < m_fftTransformSize; ++i)
				{
					double amp = sqrt((m_fftFrequencyDomainPre[i].real() * m_fftFrequencyDomainPre[i].real()) + (m_fftFrequencyDomainPre[i].imag()
						* m_fftFrequencyDomainPre[i].imag()));
					double phase = atan2(m_fftFrequencyDomainPre[i].imag(), m_fftFrequencyDomainPre[i].real());

					m_dphi[i][channel] = m_omega[i] + princeArg(phase - m_phi0[i][channel] - m_omega[i]);
					m_phi0[i][channel] = phase;

					for (int voice = 0; voice <= m_numberOfVoices; ++voice)
						timeManipulation(i/*sampleIdx*/, channel, m_fftFrequencyDomain[voice],voice, amp);
				}
				for (int voice = 0; voice < m_numberOfVoices; ++voice)
					m_reverseFFT.perform(m_fftFrequencyDomain[voice], m_fftTimeDomain[voice], true);

				for (int voice = 0; voice < m_numberOfVoices; ++voice)
					interpolate(grain2[voice], grain3[voice], i, voice, m_fftTransformSize);
		
				int outputBufferIndex = outputWritePosition;
				
				double * masterGrain = new double [m_fftTransformSize];

				for (int voice = 0; voice < m_numberOfVoices; ++voice)
				{
					for (int samples = 0; samples < m_fftTransformSize; ++samples)
						masterGrain[samples] += grain3[voice][samples];
				}

				//for (int voice = 0; voice < m_numberOfVoices; ++voice)
				//	synthesize(voice, grain3[voice], outputBufferIndex, channel);
				/*
				for (int fftBufferIndex = 0; fftBufferIndex < floor(m_voiceParamsVector.at(0).oneOverPitchShift*m_fftTransformSize); ++fftBufferIndex)
					{
						if (fftBufferIndex > m_synthWindowBufferSize)
							outputBufferData[outputBufferIndex] += 0;
						else
						{
							for (auto& voiceBufferIter : m_voiceBuffers)
							{
								float* audioBufferPointer = voiceBufferIter.getWritePointer(jmin(channel, (m_inputBuffer.getNumChannels() - 1)));
								outputBufferData[outputBufferIndex] += (1.0 / (double)m_numberOfVoices)*audioBufferPointer[outputBufferIndex];
							}
						}
						if (++outputBufferIndex >= m_outputBufferSize)
							outputBufferIndex = 0;
					}
				*/

				for (int fftBufferIdx = 0; fftBufferIdx < m_fftTransformSize; ++fftBufferIdx)
				{
					if (fftBufferIdx > m_synthWindowBufferSize)
						outputBufferData[outputBufferIndex] += 0;
					else
						outputBufferData[outputBufferIndex] += masterGrain[fftBufferIdx] *10/*(1.0 / (double)m_numberOfVoices)*(m_fftScaleFactor)*/*m_synthWindowBufferPointer[fftBufferIdx];
					if (++outputBufferIndex >= m_outputBufferSize)
						outputBufferIndex = 0;
				}

				outputWritePosition = (outputWritePosition + m_hopSize) % m_outputBufferSize;						
			}

		}

		for (int i = 0; i < 6; ++i) {
			delete[] grain2[i];
			delete[] grain3[i];
		}

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
	updatePitch(m_note0, 0/*Voice Index*/);

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
				m_fftScaleFactor = 0.5;
				break;
			case quarterWindow :
				m_fftScaleFactor = 0.25;
				break;
			case eighthWindow :
				m_fftScaleFactor = 0.25;
				break;
		}
	}

}

void phaseVocoAudioProcessor::updatePitch(int const note, int const voice)
{
	double pitchShiftValueTemp;
	double shiftDirection = 1.0;
	m_pitchShift = note - m_root; // determine semi-tone shift

	if (!m_shiftUP)
		shiftDirection = -1.0;

	switch (m_pitchShift)
	{
	case c :
		pitchShiftValueTemp = 1.0;
		break;
	case cs :
		pitchShiftValueTemp = pow(2.0, (1.0 / 12.0)*shiftDirection);
		break;
	case d :
		pitchShiftValueTemp = pow(2.0, 2.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case ds:
		pitchShiftValueTemp = pow(2.0, 3.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case e : 
		pitchShiftValueTemp = pow(2.0, 4.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case f:
		pitchShiftValueTemp = pow(2.0, 5.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case fs:
		pitchShiftValueTemp = pow(2.0, 6.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case g :
		pitchShiftValueTemp = pow(2.0, 7.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case gs :
		pitchShiftValueTemp = pow(2.0, 8.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case a:
		pitchShiftValueTemp = pow(2.0, 9.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case as:
		pitchShiftValueTemp = pow(2.0, 10.0 * (1.0 / 12.0)*shiftDirection);
		break;
	case b :
		pitchShiftValueTemp = pow(2.0, 11.0 * (1.0 / 12.0)*shiftDirection);
		break; 
	}

	m_voiceParamsVector.at(voice).pitchShiftValue = pitchShiftValueTemp;
	m_voiceParamsVector.at(voice).ratio = round(pitchShiftValueTemp*m_hopSize) / m_hopSize;
	m_voiceParamsVector.at(voice).oneOverPitchShift = 1.0 / pitchShiftValueTemp;
}

double phaseVocoAudioProcessor::princeArg(double inputPhase)
{
	if (inputPhase >= 0)
		return fmod(inputPhase + M_PI, 2 * M_PI) - M_PI;
	else
		return fmod(inputPhase + M_PI, -2 * M_PI) + M_PI;
}

void phaseVocoAudioProcessor::interpolate(double* grain2, double* grain3, int sample, int voice, int m_fftTransformSize)
{
	for (int i = 0; i < m_fftTransformSize; ++i)
		grain2[i] = m_fftTimeDomain[voice][i].real();
	for (int i = 0; i < floor(m_voiceParamsVector.at(voice).oneOverPitchShift*m_fftTransformSize); ++i)
	{
		m_voiceParamsVector.at(voice).lx = floor(m_voiceParamsVector.at(voice).oneOverPitchShift*m_fftTransformSize);
		m_voiceParamsVector.at(voice).x = i * m_fftTransformSize / m_voiceParamsVector.at(voice).lx;
		m_voiceParamsVector.at(voice).ix = floor(m_voiceParamsVector.at(voice).x);
		m_voiceParamsVector.at(voice).dx = m_voiceParamsVector.at(voice).x - (double)m_voiceParamsVector.at(voice).ix;
		grain3[i] = grain2[m_voiceParamsVector.at(voice).ix] * (1.0 - m_voiceParamsVector.at(voice).dx) + grain2[m_voiceParamsVector.at(voice).ix + 1] * m_voiceParamsVector.at(voice).dx;
	}
	/*// Interpolate

for (int i = 0; i < m_fftTransformSize; ++i)
	grain2[i] = m_fftTimeDomain[i].real();
for (int i = 0; i < floor(m_voiceParamsVector.at(0).oneOverPitchShift*m_fftTransformSize); ++i)
{
	lx = floor(m_voiceParamsVector.at(0).oneOverPitchShift*m_fftTransformSize);
	x = i * m_fftTransformSize / lx;
	ix = floor(x);
	dx = x - (double)ix;
	grain3[i] = grain2[ix] * (1.0 - dx) + grain2[ix + 1] * dx;
}
*/
}

/*
void phaseVocoAudioProcessor::synthesize(int voice, double* grain3, int outputBufferIndex, int channel)
{
	float* audioBufferPointer = m_voiceBuffers.at(voice).getWritePointer(jmin(channel, (m_inputBuffer.getNumChannels() - 1)));
	for (int fftBufferIndex = 0; fftBufferIndex < floor(m_voiceParamsVector.at(voice).oneOverPitchShift*m_fftTransformSize); ++fftBufferIndex)
	{
		if (fftBufferIndex > m_synthWindowBufferSize)
			audioBufferPointer[fftBufferIndex] += 0;
		else
			audioBufferPointer[fftBufferIndex] += grain3[fftBufferIndex] * m_fftScaleFactor * m_synthWindowBufferPointer[fftBufferIndex];
	}
}
*/
void phaseVocoAudioProcessor::timeManipulation(int i, int channel, dsp::Complex<float>* freqDomainArray, int voice, double amp)
{
	m_psi[i][channel] = princeArg(m_psi[i][channel] + m_dphi[i][channel] * m_voiceParamsVector.at(voice).ratio);

	freqDomainArray[i].real(amp*cos(m_psi[i][channel]));
	freqDomainArray[i].imag(amp*sin(m_psi[i][channel]));
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new phaseVocoAudioProcessor();
}
