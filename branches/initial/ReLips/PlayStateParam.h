#pragma once
#include "gamestateparam.h"

class PlayStateParam :
	public GameStateParam
{
public:
	PlayStateParam(void);
	virtual ~PlayStateParam(void);

	char *Filename() const { return filename; }
	void Filename(char * val) { filename = val; }

	int Difficulity() const { return difficulity; }
	void Difficulity(int val) { difficulity = val; }

protected:
	char *filename;
	int difficulity;

};
