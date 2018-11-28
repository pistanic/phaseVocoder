/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // _M_PI_

//typedef float fft_complex[2];

#include <vector>
#include "../JuceLibraryCode/JuceHeader.h"

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

enum pitchShiftNote // One Octave with semitone difference
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
	
	int m_root, m_note0;
	bool m_togState;
	   
    float curSampleVal;
   
	private:
		// Private member functions 
		void initFFT(int length);
		void deinitFFT();

		void initWindow(int length, int windowType);
		void deinitWindow();

		void initSynthWindow(int length, int windowType);
		void deinitSynthWindow();

		void updateHopSize();
		void updateScaleFactor();

		void updatePitch();

		double princeArg(double inputPhase);
		
		// Private member variables
		int m_fftSize;
		int m_hopSize, m_hopSelectSize;
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
		int m_windowBufferSize;
		double * m_synthWindowBufferPointer;
		int m_synthWindowBufferSize;

		//Audio Buffer
		AudioSampleBuffer m_inputBuffer, m_outputBuffer;

		int m_inputBufferSize; 
		int m_outputBufferSize;
		int m_inputBufferWritePosition; 
		int m_outputBufferWritePosition;
		int m_outputBufferReadPosition; 

		// fft arrays
		dsp::Complex<float> m_fftTimeDomain[1024]; // fftTransformSize
		dsp::Complex<float> m_fftFrequencyDomain[1024]; // fftTransformSize

		// fft classes
		int const mk_fftOrder = 10;
		dsp::FFT m_forwardFFT;
		dsp::FFT m_reverseFFT;

		// Bool checks
		bool m_fftInit, m_preparedToPlay;

		SpinLock m_fftSpinLock; 

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (phaseVocoAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
