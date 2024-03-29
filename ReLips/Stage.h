#pragma once
#include "stdafx.h"
#include "Fragment.h"
#include "Note.h"

using namespace std;

class Stage
{
public:
	Stage(const char *filename);
	~Stage(void);

	void MockStage();

	deque<Fragment *> *Frags() const { return m_frags; }
	void Frags(deque<Fragment *> *val) { m_frags = val; }


private:
	deque<Fragment *> *m_frags;

	void LoadFromFile(const char* filename);

	void ClearFragments();
};
