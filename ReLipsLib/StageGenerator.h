#pragma once

#include "GenerationCompletedArgs.h"
#include "GenerationProgressArgs.h"
#include "Fragment.h"

#define WINDOW_SIZE 8192
#define SAMPLE_RATE 44100
#define HOP_SIZE 200

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

	double MedianOfFive(double n1, double n2, double n3, double n4, double n5);
	
	bool DoGenerate2(char *musicFile, char* outFile);

	void WriteOutputFile(char *outFile, bool *pitches, int pitches_len);

	void FilterOutShortNotes(bool *pitches, int pitches_len, double min_sec);

	void PaintPitchesToFile(QWORD &len, bool *pitches);

	void Normalize(float voc_fft[WINDOW_SIZE]);

	int MaxIndex(float voc_fft[WINDOW_SIZE]);

	void GetChannelInfo(HSTREAM &voc_stream, BASS_CHANNELINFO &info);

	void CleanUp(HSTREAM &voc_stream, HSTREAM &novoc_stream);

	void PerformHPS(float fft[WINDOW_SIZE]);

	void InitializeBASS();

	void LoadVoiceAndNoVoiceStream(char *musicFile, HSTREAM &voc_stream, HSTREAM &novoc_stream);

	void GetFFTAtPosition(HSTREAM &voc_stream, float voc_fft[8192], int pos);

};
