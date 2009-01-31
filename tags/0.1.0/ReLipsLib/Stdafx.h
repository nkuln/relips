// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <math.h>
#include <assert.h>
#include <atlstr.h> 
#include <deque>
#include <algorithm>

#include "sndfile.h"
#include "fftw3.h"
#include "EasyBMP.h"
#include "bass.h"

#define BUFFER_LEN      8192
#define WINDOW_SIZE		2000
#define HOP_SIZE		200

#define MIN_HZ			85
#define MAX_HZ			585

#define SPECTRUM_HEIGHT 500
#define LONGEST_NOTE	0
#define VOLUME_THRESHOLD	16000

#define TO_HZ(x) (x) * 1.0 * sampleRate / WINDOW_SIZE;
#define TO_INDEX(x) (int)((x) * 1.0 * WINDOWSIZE / sampleRate)

using namespace System;
using namespace System::Threading;
using namespace System::Runtime::InteropServices;
using namespace std;



