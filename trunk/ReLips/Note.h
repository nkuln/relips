#pragma once
#include "stdafx.h"

using namespace std;

class Note
{
public:
	Note(void);
	virtual ~Note(void);

	// Start at C as Do in piano
	
	static const int NT_A = 11;
	static const int NT_ASHARP = 12;
	static const int NT_B = 13;
	static const int NT_BSHARP = 14;
	static const int NT_C = 1;
	static const int NT_CSHARP = 2;
	static const int NT_D = 3;
	static const int NT_DSHARP = 4;
	static const int NT_E = 5;
	static const int NT_ESHARP = 6;
	static const int NT_F = 7;
	static const int NT_FSHARP = 8;
	static const int NT_G = 9;
	static const int NT_GSHARP = 10;

	static int ToSymbol(double freq, double &cents);
	static int ToSymbol(double freq);

	static void ToSymbolString(double freq, char *str);

	static double GetPositionOnScale(int note, double length);
	static double GetPositionOnScale( int note, double cents, double length );
	static void ToSymbolString(int note, char *str);
	static double DetectPitch(float fft[], int sampleRate, int windowSize);
	static double lognote(double freq);
};