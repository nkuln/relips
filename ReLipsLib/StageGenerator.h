#pragma once

#include "GenerationCompletedArgs.h"
#include "GenerationProgressArgs.h"
#include "Fragment.h"

using namespace System;
using namespace System::Threading;

public delegate void GenerationProgressHandler(Object^ sender, GenerationProgressArgs^ args);
public delegate void GenerationCompletedHandler(Object^ sender, GenerationCompletedArgs^ args);

extern "C"{
	bool compare(const Fragment* f1, const Fragment* f2);
	void CALLBACK VoiceRemove(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user);
};

public ref class StageGenerator
{
public:

	event GenerationProgressHandler^ OnProgress;
	event GenerationCompletedHandler^ OnCompleted;

	StageGenerator(void);
	virtual ~StageGenerator(void);

	bool CreateStageFromFile(String^ musicFile, String^ outFile);
	void CreateStageFromFileAsync(String^ musicFile, String^ outFile);
	bool InOperation(){return m_inOperation;}
	void AbortOperation();

private:
	Thread^ m_thread;
	bool m_inOperation;

	bool DoGenerate(char* musicFile, char* outFile);
	double MedianOfFive(double n1, double n2, double n3, double n4, double n5);
	int GetTrackLength(int numFrames);
	void SubstractSpectrum(int numFrames, double **voc, double **novoc);
	void PaintSpectrum(int numFrames, double **data, const char *outname);
	void PaintSpectrum(int numFrames, int height, double **data, const char *outname);
	double** ApplyVMedianFilter(int numFrames, double **data);
	double** ApplyMedianFilter(int numFrames, double **data);
	double** BuildSpectrum(int numFrames, int sampleRate, int chans, fftw_complex *in[2]);
	double** QuantizeSpectrum(int numFrames, int sampleRate, double** voc);
	void CreateOutputFile(char* outFile, int numFrames, double** qspec);
	bool DoGenerate2(char *musicFile, char* outFile);
	void Normalize(float voc_fft[WINDOW_SIZE]);

	int MaxIndex(float voc_fft[WINDOW_SIZE]);

	void GetChannelInfo(HSTREAM &voc_stream, BASS_CHANNELINFO &info);

	void CleanUp(HSTREAM &voc_stream, HSTREAM &novoc_stream);

	void PerformHPS(float fft[WINDOW_SIZE]);

	void InitializeBASS();

	void LoadVoiceAndNoVoiceStream(char *musicFile, HSTREAM &voc_stream, HSTREAM &novoc_stream);

	void GetFFTAtPosition(HSTREAM &voc_stream, float voc_fft[8192], int pos);

};
