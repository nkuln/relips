#include "StdAfx.h"
#include "Note.h"

using namespace std;
Note::Note(void)
{
}

Note::~Note(void)
{
}

double Note::lognote( double freq )
{
	double oct = (log(freq) - log(440.0))/log(2.0) + 4.0;
	return oct;
}

int Note::ToSymbol(double freq, double &cents)
{
	double lnote = lognote( freq );
	int oct = floor( lnote );
	cents = 1200.0 * ( lnote - oct );

	// var note_table = "A A#B C C#D D#E F F#G G#";
	int notes[] = {NT_A, NT_ASHARP, 
		NT_B,
		NT_C, NT_CSHARP, 
		NT_D, NT_DSHARP,
		NT_E,
		NT_F, NT_FSHARP,
		NT_G, NT_GSHARP};

	double offset = 50.0;
	int x = 1;
	int result = -1;

	if ( cents < 50.0 )
	{
		result = NT_A;
	}
	else if ( cents >= 1150.0 )
	{
		result = NT_A;
		cents -= 1200.0;
		oct++;
	}
	else
	{	
		for (int j = 1 ; j <= 11 ; j++ )
		{
			if ( cents >= offset && cents < (offset + 100.0 ) )
			{
				// note = note_table.charAt( x ) + note_table.charAt( x + 1 );
				result = notes[x];
				cents -= ( j * 100 );
				break;
			}
			offset += 100;
			x += 1;
		}
	}

	return result;
}

int Note::ToSymbol(double freq){
	double cents = 0;
	return ToSymbol(freq,cents);
}

void Note::ToSymbolString( double freq, char *str )
{
	int note = ToSymbol(freq);
	return ToSymbolString(note, str);
}

void Note::ToSymbolString( int note, char *str )
{
	switch(note){
		case NT_A:
			strcpy(str, "A "); break;
		case NT_ASHARP:
			strcpy(str, "AA"); break;
		case NT_B:
			strcpy(str, "B "); break;
		case NT_BSHARP:
			strcpy(str, "BB"); break;
		case NT_C:
			strcpy(str, "C "); break;
		case NT_CSHARP:
			strcpy(str, "CC"); break;
		case NT_D:
			strcpy(str, "D "); break;
		case NT_DSHARP:
			strcpy(str, "DD"); break;
		case NT_E:
			strcpy(str, "E "); break;
		case NT_ESHARP:
			strcpy(str, "EE"); break;
		case NT_F:
			strcpy(str, "F "); break;
		case NT_FSHARP:
			strcpy(str, "FF"); break;
		case NT_G:
			strcpy(str, "G "); break;
		case NT_GSHARP:
			strcpy(str, "GG"); break;
	}
}
double Note::GetPositionOnScale( int note, double length )
{
	double cents = 0;
	return GetPositionOnScale(note, 0, length);
}

double Note::GetPositionOnScale( int note, double cents, double length )
{
	return (note - 1)/13.0 * length + (length/13.0 * cents/50.0);
}

double Note::DetectPitch(float fft[], int sampleRate, int windowSize){
	// Perform HPS
	float *w1 = static_cast<float *>(malloc(sizeof(float) * windowSize/2));
	float *w2 = static_cast<float *>(malloc(sizeof(float) * windowSize/2));
	float *w3 = static_cast<float *>(malloc(sizeof(float) * windowSize/2));

	int pos = 0;
	for(int i = 1 ; i < windowSize/2 ; i++){
		w1[pos++] = fft[i];
	}

	pos = 0;
	for(int i = 1 ; i < windowSize/2 ; i += 2){
		w2[pos++] = fft[i];
	}

	pos = 0;
	for(int i = 1 ; i < windowSize/2 ; i += 3){
		w3[pos++] = fft[i];
	}
	// Save last position
	int lastPos = pos - 1;

	int max = 0;
	for(int i = 0 ; i <= lastPos ; i++){
		w1[i] *= w2[i] * w3[i];
		if(w1[i] > w1[max])
			max = i;
	}

	free(w1); free(w2); free(w3);

	// Calculate frequency
	return 1.0 * sampleRate * max / windowSize;

}