#pragma once
#include "gamestateparam.h"
#include <iostream>

using namespace std;

class PlayStateParam :
	public GameStateParam
{
public:
	PlayStateParam(void);
	virtual ~PlayStateParam(void);

	string Filename() const { return filename; }
	void Filename(string val) { filename = val; }

	string MusicFilename() const {return musicFilename;}
	void MusicFilename(string val){ musicFilename = val;}



	int Difficulity() const { return difficulity; }
	void Difficulity(int val) { difficulity = val; }

protected:
	string filename;
	string musicFilename;
	int difficulity;

};
