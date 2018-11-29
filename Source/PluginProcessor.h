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
    
	struct voiceParams
	{
		double ratio = 1.0;
		double pitchShiftValue = 1.0;
		double oneOverPitchShift = 1.0;
		double lx = 0.0;
		double x = 0.0;
		double dx = 0.0;
		int ix = 0.0;
	};

	std::vector<voiceParams> m_voiceParamsVector;

	void updatePitch(int const note, int const voice);

    float curOutGain;
	
	int m_root, m_note0, m_note1, m_note2, m_note3, m_note4, m_note5;
	bool m_shiftUP;			//true to shift pitch up
	int m_numberOfVoices;
	   
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

		void interpolate(double* grain2, double* grain3, int i/*sample idx*/, int j/*voice idx*/, int m_fftTransformSize);
		void synthesize(int voice, double* grain3, int outputBufferIndex, int channel);
		void timeManipulation(int i, int channel, dsp::Complex<float>* freqDomainArray,int voice, double amp);

		double princeArg(double inputPhase);
		
		// Private member variables
		int m_fftSize;
		int m_hopSize, m_hopSelectSize;
		int m_fftTransformSize; 
		int m_samplesSinceFFT;

		double m_fftScaleFactor;
		int m_numVoicesScaleFactor;

		int m_windowType, m_synthWindowSize;
		
		int m_pitchShift;
		//double m_pitchShiftValue, m_oneOverPitchShift; // compute 1/pitchShift for efficency.
		//double m_ratio;
		
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
		std::vector<AudioSampleBuffer> m_voiceBuffers; 

		int m_inputBufferSize; 
		int m_outputBufferSize;
		int m_inputBufferWritePosition; 
		int m_outputBufferWritePosition;
		int m_outputBufferReadPosition; 

		// fft arrays

		dsp::Complex<float> m_fftTimeDomainPre[1024];
		dsp::Complex<float> m_fftTimeDomain[6][1024]; // fftTransformSize

		dsp::Complex<float> m_fftFrequencyDomainPre[1024]; // fftTransformSize
		dsp::Complex<float> m_fftFrequencyDomain[6][1024]; // fftTransformSize



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
