#include "StdAfx.h"
#include "StageGenerator.h"
#include "ThreadGenerator.h"


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
		char* ofile = (char*)(void*)Marshal::StringToHGlobalAnsi(musicFile);
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

bool StageGenerator::DoGenerate( char* musicFile, char* outFile )
{
	SNDFILE *infile, *outfile;

	SF_INFO      sfinfo ;
	int          readcount ;

	static double data [BUFFER_LEN] ;

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(1, gcnew String("Loading input file")));

	if (! (infile = sf_open (musicFile, SFM_READ, &sfinfo)))
	{   /* Open failed so print an error message. */
		throw gcnew Exception("Not able to open input file");
		/* Print the error message fron libsndfile. */
		sf_perror (NULL) ;
		return  false;
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(3, gcnew String("Checking file format")));

	if(sfinfo.channels != 2){
		printf ("Channels other than 2 is not supported");
	}

	cout << "Frame: " << sfinfo.frames << "\n";
	cout << "Channels: " << sfinfo.channels << "\n";
	cout << "Sample Rate:" << sfinfo.samplerate << "\n";



	// 0: For Left Channel
	// 1: For Right Channel

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(5, gcnew String("Preparing data")));

	fftw_complex *in[2];

	in[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);
	in[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);

	int pos[2] = {0,0}; // Keep position of sample in left and right channel

	while ((readcount = sf_read_double (infile, data, BUFFER_LEN)))
	{

		for(int c = 0 ; c < sfinfo.channels ; c++){
			for(int i = c ; i < readcount ; i += sfinfo.channels)
			{
				in[c][pos[c]][0] = data[i];
				in[c][pos[c]][1] = NULL;
				pos[c]++;
			}
		}
	}

	sf_close(infile);

	fftw_complex *sil[2];

	sil[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);
	sil[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * sfinfo.frames);

	memcpy(sil[0], in[0], sizeof(fftw_complex) * sfinfo.frames);
	memcpy(sil[1], in[1], sizeof(fftw_complex) * sfinfo.frames);

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(7, gcnew String("Removing Vocal")));

	// Remove vocal: Assume stereo channels

	cout << "Removing Vocal" << endl;

	for(int i = 0 ; i < sfinfo.frames ; i++){
		double temp = -sil[1][i][0];
		temp += sil[0][i][0];
		sil[0][i][0] = sil[1][i][0] = temp;
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(10, gcnew String("Creating Spectrum")));

	double **voc = BuildSpectrum(sfinfo, in);
	double **novoc = BuildSpectrum(sfinfo, sil);

	for(int c = 0 ; c < sfinfo.channels; c++){
		fftw_free(in[c]); 
		fftw_free(sil[c]);
	}

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(60, gcnew String("Applying smooth filter")));

	voc = ApplyVMedianFilter(sfinfo, voc);
	PaintSpectrum(sfinfo, voc, "spec_voc.bmp");
	novoc = ApplyVMedianFilter(sfinfo, novoc);
	PaintSpectrum(sfinfo, novoc, "spec_novoc.bmp");

	//////////////////////////////////////////////////////////////////////////
	OnProgress(this, gcnew GenerationProgressArgs(40, gcnew String("Substracting spectrum")));

	SubstractSpectrum(sfinfo, voc, novoc);
	// double** res = applyMedianFilter(sfinfo, voc);
	PaintSpectrum(sfinfo, voc, "spec_final.bmp");
	/// xxx

	cout << "Finished!"; 

	Console::Read();
	return 0;
}


//////////////////////////////////////////////////////////////////////////
//// CORE C++ Functions
//////////////////////////////////////////////////////////////////////////

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

int StageGenerator::GetTrackLength(const SF_INFO &sfinfo){
	return (int)(ceil((sfinfo.frames - WINDOW_SIZE)*1.0/HOP_SIZE));
}

void StageGenerator::SubstractSpectrum(SF_INFO &sfinfo, double **voc, double **novoc){
	const int len = GetTrackLength(sfinfo);
	for(int i = 1 ; i < len-1 ; i++){
		for(int j = 1 ; j < SPECTRUM_HEIGHT -1; j++){
			assert(0 <= voc[i][j] && voc[i][j] <= 1.0);
			assert(0 <= novoc[i][j] && novoc[i][j] <= 1.0);

			voc[i][j] -= novoc[i][j] + novoc[i-1][j] + novoc[i+1][j]
			+novoc[i][j-1] + novoc[i][j+1];
			if(voc[i][j] < 0) voc[i][j] = 0;
		}
	}
}

void StageGenerator::PaintSpectrum(SF_INFO &sfinfo, double **data, const char *outname){
	const int len = GetTrackLength(sfinfo);

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

double** StageGenerator::ApplyVMedianFilter(SF_INFO sfinfo, double **data){
	const int len = GetTrackLength(sfinfo);

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

double** StageGenerator::ApplyMedianFilter(SF_INFO sfinfo, double **data){
	const int len = GetTrackLength(sfinfo);

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


double** StageGenerator::BuildSpectrum(SF_INFO &sfinfo, fftw_complex *in[2]){

	int windowSize;

	fftw_complex *out[2];
	fftw_plan p[2];

	out[0] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);
	out[1] = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * WINDOW_SIZE);

	double** res;
	const int len = GetTrackLength(sfinfo);
	res = new double*[len];
	for(int i = 0 ; i < len ; i++){
		res[i] = new double[SPECTRUM_HEIGHT];
	}

	int specPos = 0; // position in spectrum

	FILE *fo = fopen("fourier.txt", "w");

	double max_so_far = -1;

	for(int wpos = 0; wpos < sfinfo.frames - WINDOW_SIZE ; wpos += HOP_SIZE){

		windowSize = WINDOW_SIZE;

		if(wpos + WINDOW_SIZE > sfinfo.frames){

			// Support for variable windowSize
			// Not used

			assert(false);
			windowSize = sfinfo.frames - wpos;
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

		for(int c = 0 ; c < sfinfo.channels ; c++){

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

		const int lbound = (int)(MIN_HZ * windowSize * 1.0 / sfinfo.samplerate);
		const int ubound = (int)(MAX_HZ * windowSize * 1.0 / sfinfo.samplerate);

		assert(lbound >= 0);
		assert(ubound < windowSize);

		double max = w1[0][lbound];
		int maxPos = lbound;

		for(int i = lbound ; i <= ubound && i < windowSize; i++){

			if(w1[0][i] > max){
				max = w1[0][i];
				maxPos = i;
			}
		}

		// Set zero
		for(int t = 0 ; t < SPECTRUM_HEIGHT ; t++){
			res[specPos][t] = 0.0;
		}

		// Set global max
		if(max > max_so_far) max_so_far = max;

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

		//// Apply median filter

		//for(int i = lbound + 2; i < (ubound - 2) && i < (windowSize - 2) ; i++){
		//	const int t = i - lbound;

		//	res[specPos][t] = medianOfFive(buff[t-2],
		//		buff[t-1],buff[t],buff[t+1],buff[t+2]);

		//}

		specPos++;

		if(wpos % 10000 == 0){
			
			//////////////////////////////////////////////////////////////////////////
			// Start at 10, End at 60
			int percent = 10 + (int)(50.0 * wpos/(sfinfo.frames - WINDOW_SIZE));
			OnProgress(this, gcnew GenerationProgressArgs(percent, gcnew String("Creating Spectrum")));
			
			// printf("Finished %d\n", wpos);
		}

		fprintf(fo,"%lf %lf\n", 
			wpos * 1.0 / sfinfo.samplerate, 
			maxPos * 1.0 * sfinfo.samplerate / windowSize); // in Hz

		for(int c = 0 ; c++ < sfinfo.channels; c++){
			fftw_destroy_plan(p[c]);
		}
	}

	fclose(fo);

	for(int c = 0 ; c < sfinfo.channels; c++){
		fftw_free(out[c]);
	}

	return res;
}