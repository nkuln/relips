#include "stdafx.h"
#include "StageGenerator.h"
#include "ThreadGenerator.h"
#include "Note.h"
#include "Fragment.h"

StageGenerator::StageGenerator(void)
:m_inOperation(false)
{
}

StageGenerator::~StageGenerator(void)
{
}

bool StageGenerator::CreateStageFromFile( String^ musicFile, String^ outFile )
{
	Monitor::Enter(this);

	try{
		m_inOperation = true;

		//////////////////////////////////////////////////////////////////////////
		char* mfile = (char*)(void*)Marshal::StringToHGlobalAnsi(musicFile);
		char* ofile = (char*)(void*)Marshal::StringToHGlobalAnsi(outFile);
		//////////////////////////////////////////////////////////////////////////

		DoGenerate(mfile,ofile);

		//////////////////////////////////////////////////////////////////////////
		Marshal::FreeHGlobal((System::IntPtr)(void*)mfile);
		Marshal::FreeHGlobal((System::IntPtr)(void*)ofile);
		//////////////////////////////////////////////////////////////////////////

		OnCompleted(this, gcnew GenerationCompletedArgs(false, nullptr));

		m_inOperation = false;
	}
	catch(Exception^ ex)
	{

		OnCompleted(this, gcnew GenerationCompletedArgs(true, ex));

		m_inOperation = false;
		return false;

	}finally{

		Monitor::Exit(this);
	}
	return true;
}

void StageGenerator::CreateStageFromFileAsync(String^ musicFile, String^ outFile){
	m_inOperation = true;
	ThreadGenerator^ tg = gcnew ThreadGenerator(this,musicFile,outFile);
	m_thread = gcnew Thread(gcnew ThreadStart(tg, &ThreadGenerator::ThreadEntryPoint));
	m_thread->Name = "ThreadStageGenerator";
	m_thread->Start();
}

void StageGenerator::AbortOperation(){
	m_thread->Abort();
}

// Voice Removal
void CALLBACK VoiceRemove(HDSP handle, DWORD channel, void *buffer, DWORD length, void *user)
{
	float *d=(float*)buffer;
	DWORD a;

	for (a=0;a<length/4;a+=2) {
		float temp = d[a] - d[a+1]; // left - right
		d[a] = d[a+1] = temp;
	}
}


void StageGenerator::GetFFTAtPosition(HSTREAM &voc_stream, float voc_fft[8192], int pos)
{
	if(!BASS_ChannelSetPosition(voc_stream, pos * 4 , BASS_POS_BYTE)){
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot set channel position. BASS error code:" + error);
	}
	
	DWORD fft_flag = 0;
	if(WINDOW_SIZE == 8192){
		fft_flag = BASS_DATA_FFT8192;
	}else if(WINDOW_SIZE == 4096){
		fft_flag = BASS_DATA_FFT4096;
	}else if(WINDOW_SIZE == 2048){
		fft_flag = BASS_DATA_FFT2048;
	}else if(WINDOW_SIZE == 1024){
		fft_flag = BASS_DATA_FFT1024;
	}else if(WINDOW_SIZE == 512){
		fft_flag = BASS_DATA_FFT512;
	}else if(WINDOW_SIZE == 256){
		fft_flag = BASS_DATA_FFT256;
	}else{
		throw gcnew Exception("Window Size not supported");
	}

	if(-1 == BASS_ChannelGetData(voc_stream, voc_fft, fft_flag | BASS_DATA_FFT_NOWINDOW))
	{
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot get channel data. BASS error code:" + error);
	}
}

void StageGenerator::LoadVoiceAndNoVoiceStream(char *musicFile, HSTREAM &voc_stream, HSTREAM &novoc_stream)
{
	voc_stream = BASS_StreamCreateFile(FALSE, musicFile, 0, 0,
		BASS_STREAM_PRESCAN | BASS_STREAM_DECODE);

	if(!voc_stream)
		throw gcnew Exception("Can't load file " + gcnew String(musicFile));

	novoc_stream = BASS_StreamCreateFile(FALSE, musicFile, 0, 0,
		BASS_STREAM_PRESCAN | BASS_STREAM_DECODE);

	if(!novoc_stream)
		throw gcnew Exception("Can't load file " + gcnew String(musicFile));
}

void StageGenerator::InitializeBASS()
{
	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION)
		throw gcnew Exception("An incorrect version of BASS was loaded");

	// free first
	BASS_Free();

	if (!BASS_Init(-1,SAMPLE_RATE,0,0,NULL))
		throw gcnew Exception("Can't initialize device");
}

void StageGenerator::PerformHPS(float fft[WINDOW_SIZE])
{
	// perform HPS
	float *w1 = static_cast<float *>(malloc(sizeof(float) * WINDOW_SIZE/2));
	float *w2 = static_cast<float *>(malloc(sizeof(float) * WINDOW_SIZE/2));
	float *w3 = static_cast<float *>(malloc(sizeof(float) * WINDOW_SIZE/2));

	int pos = 0;
	for(int i = 1 ; i < WINDOW_SIZE/2 ; i++){
		w1[pos++] = fft[i];
		if(w1[pos - 1] < 0)
		{
			cout << "Failed!";
			exit(-1);
		}
	}

	pos = 0;
	for(int i = 1 ; i < WINDOW_SIZE/2 ; i += 2){
		w2[pos++] = fft[i];
	}

	pos = 0;
	for(int i = 1 ; i < WINDOW_SIZE/2 ; i += 3){
		w3[pos++] = fft[i];
	}

	// save last position
	int lastPos = pos - 1;

	// clean array
	for(int i = 0 ; i < WINDOW_SIZE; i++)
		fft[i] = 0.0f;

	for(int i = 0 ; i <= lastPos ; i++){
		fft[i] = w1[i] * w2[i] * w3[i];
	}

	free(w1); free(w2); free(w3);
}

void StageGenerator::CleanUp(HSTREAM &voc_stream, HSTREAM &novoc_stream)
{
	// free channel
	if(!BASS_StreamFree(voc_stream))
	{
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot free stream. BASS error code:" + error);
	}

	// free channel
	if(!BASS_StreamFree(novoc_stream))
	{
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot free stream. BASS error code:" + error);
	}

	// free BASS
	BASS_Free();
}

void StageGenerator::GetChannelInfo(HSTREAM &voc_stream, BASS_CHANNELINFO &info)
{
	if(!BASS_ChannelGetInfo(voc_stream, &info)){
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot get media file info. BASS error code:" + error);
	}
}
int StageGenerator::MaxIndex(float voc_fft[WINDOW_SIZE])
{
	double hps_max = -1.0;
	int hps_maxi = -1;
	for(int i = 0 ; i < WINDOW_SIZE/3 ; i++){
		if(voc_fft[i] > hps_max){
			hps_max = voc_fft[i];
			hps_maxi = i;
		}
	}
	return hps_maxi;
}

void StageGenerator::Normalize(float voc_fft[WINDOW_SIZE])
{
	// normalize
	int hps_maxi = MaxIndex(voc_fft);
	float hps_max = voc_fft[hps_maxi];

	for(int i = 0 ; i < WINDOW_SIZE/3 ; i++)
		voc_fft[i] /= hps_max;

	for(int i = WINDOW_SIZE/3; i < WINDOW_SIZE ; i++)
		voc_fft[i] = 0.0;
}

void StageGenerator::PaintPitchesToFile(QWORD &len, bool *pitches)
{
	BMP spec;
	spec.SetBitDepth(8);
	spec.SetSize(len/HOP_SIZE + 1, 15);

	for(int pos = 0 ; pos < len ; pos += HOP_SIZE){
		for(int i = 0 ; i < 15 ; i++){
			int row = pos/HOP_SIZE;
			if(pitches[row*15 + i] == true){
				spec(row, i)->Alpha = 0xFF;
				spec(row, i)->Red = 0x00;
				spec(row, i)->Blue = 0x00;
				spec(row, i)->Green = 0x00;
			}else{
				spec(row, i)->Alpha = 0xFF;
				spec(row, i)->Red = 0xFF;
				spec(row, i)->Blue = 0xFF;
				spec(row, i)->Green = 0xFF;
			}
		}
	}

	spec.SetBitDepth(24);
	spec.WriteToFile("spec.bmp");
}
void StageGenerator::FilterOutShortNotes(bool *pitches, int pitches_len, double min_sec)
{
	int** temp;
	temp = new int*[pitches_len];

	for(int i = 0 ; i < pitches_len ; i++)
		temp[i] = new int[15];

	for(int i = 0 ; i < pitches_len ; i++){

		// set zero

		for(int j = 1 ; j <= 14 ; j++){
			temp[i][j] = 0;
		}

		// count continuity

		for(int j = 1 ; j <= 14; j++){
			if(pitches[i*15 + j] == true){
				if(i > 0) temp[i][j] = temp[i-1][j] + 1;
				else temp[i][j] = 1;
			}
		}
	}

	// minimum period for a note
	int min_len = (int)(min_sec * SAMPLE_RATE / HOP_SIZE);

	for(int i = pitches_len - 1 ; i >= 0 ; i--){
		for(int j = 1 ; j <= 14; j++){
			if(temp[i][j] != 0){
				if(temp[i][j] >= min_len){
					temp[i][j] = min_len;
					pitches[i*15 + j] = true;
				}else if(temp[i + 1][j] >= min_len){
					temp[i][j] = min_len;
					pitches[i*15 + j] = true;
				}else{
					temp[i][j] = 0;
					pitches[i*15 + j] = false;
				}
			}
		}
	}
}
void StageGenerator::WriteOutputFile(char *outFile, bool *pitches, int pitches_len)
{
	deque<Fragment *> q;

	for(int j = 1 ; j < 15; j++){
		int start = -1, end = -1;
		bool flag = false;
		for(int i = 1 ; i < pitches_len ; i++){
			if(flag == false && pitches[i*15 + j] == true){

				// first black

				start = i; flag = true;

			}else if(flag == true && pitches[i*15 + j] == false){

				// first white

				end = i-1;

				if(start > end){
					cout << "Failed!";
					exit(-1);
				}

				Fragment *f = new Fragment(start*HOP_SIZE, end*HOP_SIZE, j);
				q.push_back(f);
				flag = false;
			}
		}
	}

	sort(q.begin(), q.end() , compare);

	for(deque<Fragment *>::iterator i = q.begin() ; i != q.end() ; i++){
		Fragment *f = *i;
		if((i+1) != q.end()){
			Fragment *g = i[1];

			// Check collision
			if(f->End() >= g->Start()){
				if(f->End() >= g->End())
					q.erase(i+1);
				else
					g->Start(f->End());
			}
		}
		// cout << f->Start() << " : " << f->End() << endl;
	}

	FILE *fo = fopen(outFile, "w");

	for(deque<Fragment *>::iterator i = q.begin() ; i != q.end() ; i++){
		Fragment *f = *i;
		fprintf(fo,"%d\t%d\t%d\r\n", f->Start(), f->End(), f->Note());
	}

	fclose(fo);


	for(deque<Fragment *>::iterator i = q.begin() ; i != q.end() ; i++)
		delete *i;
}
void StageGenerator::FilterOutUnwantedFreqs(int i_start, int i_end, float voc_fft[WINDOW_SIZE])
{
	// filter out unwanted freqs
	for(int i = 0 ; i < WINDOW_SIZE/2 ; i++){
		if(i < i_start || i > i_end)
			voc_fft[i] = 0.0;
	}
}

bool StageGenerator::DoGenerate(char *musicFile, char* outFile){

#define GEN_BITMAP 1 // To generate bitmap of spectrum or not

	InitializeBASS();

	// ### PROGRESS ###
	OnProgress(this, gcnew GenerationProgressArgs(1, gcnew String("Loading input file")));

	// channel to load from file
	HSTREAM voc_stream;
	HSTREAM novoc_stream;
	LoadVoiceAndNoVoiceStream(musicFile, voc_stream, novoc_stream);

	// set DSP function
	BASS_ChannelSetDSP(novoc_stream,&VoiceRemove,0,2);


	// get info
	BASS_CHANNELINFO voc_info, novoc_info;
	GetChannelInfo(voc_stream, voc_info);
	GetChannelInfo(novoc_stream, novoc_info);

	// get length
	QWORD len = BASS_ChannelGetLength(voc_stream, BASS_POS_BYTE) / 4;

	// output media info
	cout << "Number of channels: " << voc_info.chans << endl;
	cout << "Filename: " << voc_info.filename << endl;
	cout << "Frequency: " << voc_info.freq << endl;
	cout << "Length (samples): " << len << endl;

	// ### PROGRESS ###
	OnProgress(this, gcnew GenerationProgressArgs(2, gcnew String("Loading completed")));

	// allocate pitches 2d array
	bool* pitches;
	int pitches_len = len/HOP_SIZE + 1;
	int pitches_size = pitches_len * 15;
	pitches = new bool[pitches_size];
	for(int i = 0 ; i < pitches_size ; i++){
		pitches[i] = false;
	}
	
	// filter only singing freqs
	int i_start = floor(80.0 * WINDOW_SIZE / SAMPLE_RATE);
	int i_end = ceil(1000.0 * WINDOW_SIZE / SAMPLE_RATE);

#if GEN_BITMAP
	BMP spec_voc, spec_novoc;
	spec_voc.SetBitDepth(8);
	spec_voc.SetSize(len/HOP_SIZE + 1, WINDOW_SIZE/3);
	spec_novoc.SetBitDepth(8);
	spec_novoc.SetSize(len/HOP_SIZE + 1, WINDOW_SIZE/3);
#endif

	for(int pos = 0; pos < len ; pos += HOP_SIZE){

		if((pos/HOP_SIZE) % 500 == 0){
			// ### PROGRESS ###
			int percent = (int)(98.0 * pos / len) + 2;
			OnProgress(this, gcnew GenerationProgressArgs(percent, gcnew String("Loading completed")));
		}

		// get level
		DWORD level, left, right, avglevel;
		level=BASS_ChannelGetLevel(voc_stream);
		left=LOWORD(level); // the left level
		right=HIWORD(level); // the right level 
		
		if(level != -1)
			avglevel = (left + right)/2;
		else
			avglevel = 0;

		// sample data FFT
		float voc_fft[WINDOW_SIZE], novoc_fft[WINDOW_SIZE];

		// get FFT data at sample position i
		GetFFTAtPosition(voc_stream, voc_fft, pos);
		GetFFTAtPosition(novoc_stream, novoc_fft, pos);

		// filter out unwanted freqs
		FilterOutUnwantedFreqs(i_start, i_end, voc_fft);
		FilterOutUnwantedFreqs(i_start, i_end, novoc_fft);

		// HPS the FFTs
		PerformHPS(voc_fft); PerformHPS(novoc_fft);
		Normalize(voc_fft); Normalize(novoc_fft);

#if GEN_BITMAP
		// Paint Voc
		for(int i = i_start ; i < i_end ; i++){
			int col = (int)(255.0 * voc_fft[i]);
			spec_voc(pos/HOP_SIZE, i)->Alpha = 0xFF;
			spec_voc(pos/HOP_SIZE, i)->Red = 0xFF - (col & 0xFF);
			spec_voc(pos/HOP_SIZE, i)->Blue = 0xFF - (col & 0xFF);
			spec_voc(pos/HOP_SIZE, i)->Green = 0xFF - (col & 0xFF);
		}

		// Paint Novoc
		for(int i = i_start ; i < i_end ; i++){
			int col = (int)(255.0 * novoc_fft[i]);
			spec_novoc(pos/HOP_SIZE, i)->Alpha = 0xFF;
			spec_novoc(pos/HOP_SIZE, i)->Red = 0xFF - (col & 0xFF);
			spec_novoc(pos/HOP_SIZE, i)->Blue = 0xFF - (col & 0xFF);
			spec_novoc(pos/HOP_SIZE, i)->Green = 0xFF - (col & 0xFF);
		}
#endif

#if 1
		// subtract HPS spectrum
		for(int i = 0 ; i < WINDOW_SIZE/3 ; i++){
			voc_fft[i] -= novoc_fft[i];
			if(voc_fft[i] < 0) voc_fft[i] = 0.0f;
		}
#endif
		// median smoothing

		// TODO:
		
		// pitch tracking

		double pitch_spec[15] = {0.0};
		for(int i = i_start ; i < i_end ; i++){
			double hz = 1.0 * i * SAMPLE_RATE / WINDOW_SIZE;
			int note = Note::ToSymbol(hz);
			pitch_spec[note] += voc_fft[i];
		}
		
		// get 3 peaks and put it in 
		double old_peak = -1.0;

		for(int count = 1 ; count <= 1 && avglevel > 10000 ; count++){

			double max = -1.0;
			int maxi = -1;
			for(int i = 1 ; i <= 14 ; i++){
				if(pitch_spec[i] > max){
					max = pitch_spec[i];
					maxi = i;
				}
			}

			if(max > 0.0){
				if(old_peak >= 0.0){
					if(max/old_peak >= 0.80){
						pitches[(pos/HOP_SIZE)*15 + maxi] = true;
						pitch_spec[maxi] = -1.0;
						old_peak = max;
					}
				}else if(count == 1){ // first time
					pitches[(pos/HOP_SIZE)*15 + maxi] = true;
					pitch_spec[maxi] = -1.0;
					old_peak = max;
				}
			}

		}

		cout << "Processed " << pos << endl;
	}

#if GEN_BITMAP
	spec_voc.SetBitDepth(24);
	spec_voc.WriteToFile("spec_voc_.bmp");

	spec_novoc.SetBitDepth(24);
	spec_novoc.WriteToFile("spec_novoc_.bmp");
#endif

	// paint the pitches to BMP file

	PaintPitchesToFile(len, pitches);

	// clean up allocated BASS stream

	CleanUp(voc_stream, novoc_stream);

	// filter out garbage
	// ### PROGRESS ###
	OnProgress(this, gcnew GenerationProgressArgs(100, gcnew String("Post processing")));

	double min_sec = 0.05;
	FilterOutShortNotes(pitches, pitches_len, min_sec);
	
	// write output
	// ### PROGRESS ###
	OnProgress(this, gcnew GenerationProgressArgs(100, gcnew String("Creating output file")));

	WriteOutputFile(outFile, pitches, pitches_len);

	// free allocated 2d pitches array
	delete [] pitches;

	// ### PROGRESS ###
	OnProgress(this, gcnew GenerationProgressArgs(100, gcnew String("Completed!")));

	return true;
}

bool compare( const Fragment* f1, const Fragment* f2 )
{
	return f1->Start() < f2->Start();
}

double StageGenerator::MedianOfFive(double n1, double n2, double n3, double n4, double n5){
	double *a = &n1, *b = &n2, *c = &n3, *d = &n4, *e = &n5;
	double *tmp;

	// makes a < b and b < d
	if(*b < *a){
		tmp = a; a = b; b = tmp;
	}

	if(*d < *c){
		tmp = c; c = d; d = tmp;
	}

	// eleminate the lowest
	if(*c < *a){
		tmp = b; b = d; d = tmp; 
		c = a;
	}

	// gets e in
	a = e;

	// makes a < b and b < d
	if(*b < *a){
		tmp = a; a = b; b = tmp;
	}

	// eliminate another lowest
	// remaing: a,b,d
	if(*a < *c){
		tmp = b; b = d; d = tmp; 
		a = c;
	}

	if(*d < *a)
		return *d;
	else
		return *a;

}

