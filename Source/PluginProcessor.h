/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"


#ifndef M_PI
	#define M_PI 3.14159265358979323846
#endif // _M_PI_

enum windowType
{
	hann = 0,
	hamm,
	bart
};

enum hopSize
{
	window = 0,
	halfWindow,
	quarterWindow,
	eighthWindow
};

enum pitchShiftVal // One Octave with semitone difference
{
	c = 0, 
	cs,
	d,
	ds,
	e,
	f,
	fs,
	g,
	gs,
	a,
	as,
	b
};

//==============================================================================
/**
 */
class phaseVocoAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    phaseVocoAudioProcessor();
    ~phaseVocoAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    float curOutGain;
    float curSampleVal;
   
	private:
		int m_fftSize, m_hopSize;
		int m_fftTransformSize; 
		int m_samplesSinceFFT;

		double m_fftScaleFactor; 

		int m_windowType, m_synthWindowSize;
		
		int m_pitchShift;
		double m_pitchShiftValue, m_oneOverPitchShift; // compute 1/pitchShift for efficency.
		double m_ratio;
		
		// phase vectrors length 2*fftSize
		std::vector<double> m_omega;
		// phase 2d arrays 
		double m_phi[2048][2];
		double m_phi0[2048][2];
		double m_dphi[2048][2];
		double m_psi[2048][2];

		// window buffer pointers
		double * m_windowBufferPointer;
		double * m_synthWindowBufferPointer;

		//Audio Buffer
		AudioSampleBuffer m_inputBuffer, m_outputBuffer;

		int m_inputBufferSize; 
		int m_outputBufferSize;
		int m_inputBufferWritePosition; 
		int m_outputBufferWritePosition;
		int m_outputBufferReadPosition; 

		// Bool checks
		bool m_fftInit, m_preparedToPlay;

		SpinLock m_fftSpinLock; 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (phaseVocoAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
