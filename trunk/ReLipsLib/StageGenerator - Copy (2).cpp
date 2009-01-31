#include "StdAfx.h"
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


bool StageGenerator::DoGenerate2(char *musicFile, char* outFile){
	return false;
}

bool StageGenerator::DoGenerate( char* musicFile, char* outFile )
{
	printf("BASS WAV writer example : MOD/MPx/OGG -> BASS.WAV\n"
			"-------------------------------------------------\n");

	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		throw gcnew Exception("An incorrect version of BASS was loaded");
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(1, gcnew String("Loading input file")));

	// Free first
	BASS_Free();

	if (!BASS_Init(-1,44100,0,0,NULL))
		throw gcnew Exception("Can't initialize device");

	HSAMPLE sample = BASS_SampleLoad(false, musicFile, 0, 0, 1, 0);
	if(!sample){
		// Cannot load file
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot load media file. BASS error code:" + error);
	}

	BASS_SAMPLE sampleinfo;
	
	if(!BASS_SampleGetInfo(sample, &sampleinfo)){
		// Cannot get info
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot get media file info. BASS error code:" + error);
	}

	//DWORD numChannel = BASS_SampleGetChannels(sample, sampleinfo.ma)

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(3, gcnew String("Checking file format")));


	if(sampleinfo.chans != 2){
		throw gcnew Exception("Channels other than 2 is not supported");
	}

	if((sampleinfo.flags & BASS_SAMPLE_8BITS) || (sampleinfo.flags & BASS_SAMPLE_FLOAT)){
		throw gcnew Exception("Sampling resolution not supported");
	}

	cout << "Length(byte): " << sampleinfo.length << "\n";
	cout << "Channels: " << sampleinfo.chans << "\n";
	cout << "Sample Rate:" << sampleinfo.freq << "\n";

	// 0: For Left Channel
	// 1: For Right Channel



	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(5, gcnew String("Preparing data")));

	// Load sample data with BASS
	
	short *buf = new short[sampleinfo.length/2];
	if(!BASS_SampleGetData(sample, buf)){
		// Cannot get sample data
		int error = BASS_ErrorGetCode();
		throw gcnew Exception("Cannot get sample data. BASS error code:" + error);
	}

	// Get number of frames. 2 bytes for 16-bit sample.
	int numFrames = sampleinfo.length/(2 * sampleinfo.chans);

	fftw_complex *in[2];

	in[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numFrames);
	in[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numFrames);

	int pos[2] = {0,0}; // Keep position of sample in left and right channel

	for(int c = 0 ; c < sampleinfo.chans ; c++){
		for(int i = c ; i < numFrames*2 ; i += sampleinfo.chans)
		{
			in[c][pos[c]][0] = buf[i];
			in[c][pos[c]][1] = NULL;
			pos[c]++;
		}
	}

	BASS_SampleFree(sample);
	delete buf;

	BASS_Free();

	fftw_complex *sil[2];

	sil[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numFrames);
	sil[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * numFrames);

	memcpy(sil[0], in[0], sizeof(fftw_complex) * numFrames);
	memcpy(sil[1], in[1], sizeof(fftw_complex) * numFrames);

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(7, gcnew String("Removing Vocal")));

	// Remove vocal: Assume stereo channels

	cout << "Removing Vocal" << endl;

	for(int i = 0 ; i < numFrames ; i++){
		double temp = -sil[1][i][0];
		temp += sil[0][i][0];
		sil[0][i][0] = sil[1][i][0] = temp;
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(10, gcnew String("Creating Spectrum")));

	const int len = GetTrackLength(numFrames);

	double **voc = BuildSpectrum(numFrames, sampleinfo.freq, sampleinfo.chans, in);

	for(int c = 0 ; c < sampleinfo.chans; c++){
		fftw_free(in[c]); 
	}

	double **novoc = BuildSpectrum(numFrames, sampleinfo.freq, sampleinfo.chans, sil);
	for(int c = 0 ; c < sampleinfo.chans; c++){
		fftw_free(sil[c]);
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(60, gcnew String("Applying smooth filter")));

	double **oldvoc = voc, **oldnovoc = novoc;

	voc = ApplyVMedianFilter(numFrames, voc);
	PaintSpectrum(numFrames, voc, "spec_voc.bmp");

	// Free oldvoc
	for(int i = 0 ; i < len ; i++){
		delete [] oldvoc[i];
	}
	delete [] oldvoc;

	novoc = ApplyVMedianFilter(numFrames, novoc);
	PaintSpectrum(numFrames, novoc, "spec_novoc.bmp");

	// Free oldnovoc
	for(int i = 0 ; i < len ; i++){
		delete [] oldnovoc[i];
	}
	delete [] oldnovoc;

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(80, gcnew String("Substracting spectrum")));

	SubstractSpectrum(numFrames, voc, novoc);

	PaintSpectrum(numFrames, voc, "spec_final.bmp");
	/// xxx

	// Free novoc
	for(int i = 0 ; i < len ; i++){
		delete [] novoc[i];
	}
	delete [] novoc;

	//////////////////////////////////////////////////////////////////////////

	double** qspec = QuantizeSpectrum(numFrames, sampleinfo.freq, voc);

	// Free voc
	for(int i = 0 ; i < len ; i++){
		delete [] voc[i];
	}
	delete [] voc;

	CreateOutputFile(outFile, numFrames, qspec);

	PaintSpectrum(numFrames, Note::NT_NUMNOTE + 1, qspec, "quantized_voc.bmp");
	// double** res = applyMedianFilter(numFrames, voc);

	for(int i = 0 ; i < len ; i++)
		delete [] qspec[i];
	delete [] qspec;

	return 0;
}


//////////////////////////////////////////////////////////////////////////
//// CORE C++ Functions
//////////////////////////////////////////////////////////////////////////


bool compare( const Fragment* f1, const Fragment* f2 )
{
	return f1->Start() < f2->Start();
}

void StageGenerator::CreateOutputFile(char* outFile, int numFrames, double** qspec){

	const int len = GetTrackLength(numFrames);

	int** temp;
	temp = new int*[len];

	for(int i = 0 ; i < len ; i++)
		temp[i] = new int[Note::NT_NUMNOTE + 1];


	for(int i = 1 ; i < len-1 ; i++){
		for(int j = 0 ; j < Note::NT_NUMNOTE + 1 ; j++){
			temp[i][j] = 0;
		}

		for(int j = 0 ; j < Note::NT_NUMNOTE + 1; j++){
			if(qspec[i][j] >= 1.0){
				if(i > 1) temp[i][j] = temp[i-1][j] + 1;
				else temp[i][j] = 1;
			}
		}
	}

	for(int i = len - 3 ; i >= 1 ; i--){
		for(int j = 0 ; j < Note::NT_NUMNOTE + 1; j++){
			if(temp[i][j] != 0){
				if(temp[i][j] >= 10){ temp[i][j] = 10; qspec[i][j] = 1.0;}
				else if(temp[i + 1][j] >= 10){ temp[i][j] = 10; qspec[i][j] = 1.0;}
				else{ temp[i][j] = 0; qspec[i][j] = 0.0;}
			}
		}
	}

	deque<Fragment *> q;

	for(int j = 1 ; j < Note::NT_NUMNOTE + 1; j++){
		int start = -1, end = -1;
		bool flag = false;
		for(int i = 1 ; i < len-1 ; i++){
			if(flag == false && temp[i][j] == 10){
				// First black
				start = i;
				flag = true;
			}else if(flag == true && temp[i][j] == 0){
				// First white
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


	// Free
	for(int i = 0 ; i < len ; i++)
		delete [] temp[i];

	delete [] temp;

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

double** StageGenerator::QuantizeSpectrum(int numFrames, int sampleRate, double** voc)
{
	const int len = GetTrackLength(numFrames);

	// position for lower and upper bound in array

	const int lbound = (int)(MIN_HZ * WINDOW_SIZE * 1.0 / sampleRate);
	const int ubound = (int)(MAX_HZ * WINDOW_SIZE * 1.0 / sampleRate);

	double** res;
	res = new double*[len];

	for(int i = 0 ; i < len ; i++)
		res[i] = new double[Note::NT_NUMNOTE + 1];

	double hz = 0.0;

	for(int i = 1 ; i < len-1 ; i++){
		
		// Set Zero
		for(int j = 0 ; j < Note::NT_NUMNOTE + 1 ; j++){
			res[i][j] = 0;
		}

		for(int j = 0 ; j < SPECTRUM_HEIGHT -1; j++){
			assert(0 <= voc[i][j] && voc[i][j] <= 1.0);

			hz = TO_HZ(j + lbound);

			int note = Note::ToSymbol(hz);

			res[i][note] += voc[i][j];

			if(res[i][note] > 1.0) res[i][note] = 1.0;
		}
	}

	// Clean up
	for(int i = 1 ; i < len-1 ; i++){
		for(int j = 0 ; j < Note::NT_NUMNOTE + 1 ; j++){
			res[i][j] = res[i][j] > 0.5 ? 1 : 0;
		}
	}

	return res;
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

int StageGenerator::GetTrackLength(int numFrames){
	return (int)(ceil((numFrames - WINDOW_SIZE)*1.0/HOP_SIZE));
}

void StageGenerator::SubstractSpectrum(int numFrames, double **voc, double **novoc){
	const int len = GetTrackLength(numFrames);
	for(int i = 1 ; i < len-1 ; i++){
		for(int j = 1 ; j < SPECTRUM_HEIGHT -1; j++){
			assert(0 <= voc[i][j] && voc[i][j] <= 1.0);
			assert(0 <= novoc[i][j] && novoc[i][j] <= 1.0);

			//voc[i][j] -= novoc[i][j] + novoc[i-1][j] + novoc[i+1][j]
			//+novoc[i][j-1] + novoc[i][j+1];

			voc[i][j] -= novoc[i][j];

			if(voc[i][j] < 0) voc[i][j] = 0;
		}
	}
}

void StageGenerator::PaintSpectrum(int numFrames, int height, double **data, const char *outname){
	const int len = GetTrackLength(numFrames);

	// Create Empty Bitmap
	BMP spec;
	spec.SetBitDepth(8);
	spec.SetSize(len, height);

	for(int i = 0 ; i < len ; i++){
		for(int j = 0 ; j < height ; j++){
			int col = (int)(255.0 * data[i][j]);
			spec(i, j)->Alpha = 0xFF;
			spec(i, j)->Red = 0xFF - col & 0xFF;
			spec(i, j)->Blue = 0xFF - col & 0xFF;
			spec(i, j)->Green = 0xFF - col & 0xFF;
		}
	}

	spec.SetBitDepth(24);
	spec.WriteToFile(outname);

}

void StageGenerator::PaintSpectrum(int numFrames, double **data, const char *outname){
	const int len = GetTrackLength(numFrames);

	// Create Empty Bitmap
	BMP spec;
	spec.SetBitDepth(8);
	spec.SetSize(len, SPECTRUM_HEIGHT);

	for(int i = 0 ; i < len ; i++){
		for(int j = 0 ; j < SPECTRUM_HEIGHT ; j++){
			int col = (int)(255.0 * data[i][j]);
			spec(i, j)->Alpha = 0xFF;
			spec(i, j)->Red = 0xFF - col & 0xFF;
			spec(i, j)->Blue = 0xFF - col & 0xFF;
			spec(i, j)->Green = 0xFF - col & 0xFF;
		}
	}

	spec.SetBitDepth(24);
	spec.WriteToFile(outname);

}

double** StageGenerator::ApplyVMedianFilter(int numFrames, double **data){
	const int len = GetTrackLength(numFrames);

	double** res;
	res = new double*[len];
	for(int i = 0 ; i < len ; i++)
		res[i] = new double[SPECTRUM_HEIGHT];

	for(int i = 0 ; i <= 1; i++){
		for(int j = 0 ; j < SPECTRUM_HEIGHT ; j++){
			res[i][j] = res[len-i-1][j] = 0.0;
		}
	}

	for(int i = 2 ; i < len - 2 ; i++){
		for(int j = 0 ; j < SPECTRUM_HEIGHT ; j++){
			res[i][j] = MedianOfFive(
				data[i-2][j],data[i-1][j],
				data[i+2][j],data[i+1][j],
				data[i][j]);
			assert(res[i][j] >= 0);
		}
	}
	return res;
}

double** StageGenerator::ApplyMedianFilter(int numFrames, double **data){
	const int len = GetTrackLength(numFrames);

	double** res;
	res = new double*[len];
	for(int i = 0 ; i < len ; i++)
		res[i] = new double[SPECTRUM_HEIGHT];

	for(int i = 1 ; i < len - 1 ; i++){
		for(int j = 1 ; j < SPECTRUM_HEIGHT - 1; j++){
			res[i][j] = MedianOfFive(
				data[i-1][j],data[i+1][j],
				data[i][j-1],data[i][j+1],
				data[i][j]);
		}
	}
	return res;
}


double** StageGenerator::BuildSpectrum(int numFrames, int sampleRate, int chans, fftw_complex *in[2]){

	int windowSize;

	fftw_complex *out[2];
	fftw_plan p[2];

	out[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);
	out[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);

	double** res;
	const int len = GetTrackLength(numFrames);
	res = new double*[len];
	for(int i = 0 ; i < len ; i++){
		res[i] = new double[SPECTRUM_HEIGHT];
	}

	int specPos = 0; // position in spectrum

	double max_so_far = -1;

	for(int wpos = 0; wpos < numFrames - WINDOW_SIZE ; wpos += HOP_SIZE){

		
		windowSize = WINDOW_SIZE;

		if(wpos + WINDOW_SIZE > numFrames){

			// Support for variable windowSize
			// Not used

			assert(false);
			windowSize = numFrames - wpos;
			p[0] = fftw_plan_dft_1d(windowSize, &in[0][wpos], out[0], FFTW_FORWARD, FFTW_ESTIMATE);
			p[1] = fftw_plan_dft_1d(windowSize, &in[1][wpos], out[1], FFTW_FORWARD, FFTW_ESTIMATE);
		}else{
			p[0] = fftw_plan_dft_1d(WINDOW_SIZE, &in[0][wpos], out[0], FFTW_FORWARD, FFTW_ESTIMATE);
			p[1] = fftw_plan_dft_1d(WINDOW_SIZE, &in[1][wpos], out[1], FFTW_FORWARD, FFTW_ESTIMATE);
		}

		fftw_execute(p[0]); 
		fftw_execute(p[1]);

		double w1[2][WINDOW_SIZE/2] = {0.0}, 
			w2[2][WINDOW_SIZE/2] = {0.0}, 
			w3[2][WINDOW_SIZE/2] = {0.0};


		////////// Harmonic Product Spectrum /////////////////

		for(int c = 0 ; c < chans ; c++){

			for(int i = 0 ; i < WINDOW_SIZE ; i++){
				out[c][i][0] = abs(out[c][i][0]);
			}

			int pos = 0;
			for(int i = 0 ; i < windowSize/2 ; i++){
				w1[c][pos++] = out[c][i][0];
			}

			pos = 0;
			for(int i = 0; i < windowSize/2 ; i+=2){
				w2[c][pos++] = out[c][i][0];
			}


			pos = 0;
			for(int i = 0 ; i < windowSize/2 ; i+=3){
				w3[c][pos++] = out[c][i][0];
			}

			// Last pos
			pos--;

			for(int i = 0 ; i <= pos ; i++){
				assert(w1[c][i] >= 0);
				assert(w2[c][i] >= 0);
				assert(w3[c][i] >= 0);
				w1[c][i] = w1[c][i] * w2[c][i] * w3[c][i];
				assert(w1[c][i] >= 0);

			}

		}

		for(int i = 0 ; i < windowSize/2 ; i++){

			// Choose Min from 2 channel (most likely to be a voice)
			w1[0][i] = w1[0][i] < w1[1][i] ? w1[0][i] : w1[1][i];

		}

		// position for lower and upper bound in array

		const int lbound = (int)(MIN_HZ * windowSize * 1.0 / sampleRate);
		const int ubound = (int)(MAX_HZ * windowSize * 1.0 / sampleRate);

		assert(lbound >= 0);
		assert(ubound < windowSize);

		double max = w1[0][lbound];
		int maxPos = lbound;

		//////////////////////////////////////////////////////////////////////////
		//////////// BUGGY!!! ////////////
		//////////////////////////////////////////////////////////////////////////
		for(int i = lbound ; i <= ubound && i < windowSize; i++){

			if(w1[0][i] > max){
				max = w1[0][i];
				maxPos = i;
			}
		}

		////////////////////////////////////////////////////////////////////////////
		//char buf[100];
		//double freq = 1.0 * maxPos * sampleRate / windowSize;
		//Note::ToSymbolString(freq,buf);
		//cout << buf << " : " << freq << endl;

		////////////////////////////////////////////////////////////////////////////

		// Set zero
		for(int t = 0 ; t < SPECTRUM_HEIGHT ; t++){
			res[specPos][t] = 0.0;
		}

		// Set global max
		if(max > max_so_far) 
			max_so_far = max;

		double buff[SPECTRUM_HEIGHT];

		for(int i = lbound ; i < ubound && i < windowSize  ; i++){

			const int t = i - lbound;

			assert(0 <= t && t < SPECTRUM_HEIGHT);
			assert(0 <= specPos && specPos < len);

			if(max == 0)
				buff[t] = 0;
			else
				buff[t] = w1[0][i]/max;

			res[specPos][t] = buff[t];

			assert(res[specPos][t] >= 0);
		}


		specPos++;

		if(wpos % 10000 == 0){
			
			//////////////////////////////////////////////////////////////////////////
			// Start at 10, End at 60
			int percent = 10 + (int)(50.0 * wpos/(numFrames - WINDOW_SIZE));
			OnProgress(this, gcnew GenerationProgressArgs(percent, gcnew String("Creating Spectrum")));

		}

		//fprintf(fo,"%lf %lf\n", 
		//	wpos * 1.0 / sampleRate, 
		//	maxPos * 1.0 * sampleRate / windowSize); // in Hz

		for(int c = 0 ; c++ < chans; c++){
			fftw_destroy_plan(p[c]);
		}
	}


	for(int c = 0 ; c < chans; c++){
		fftw_free(out[c]);
	}



	return res;
}
