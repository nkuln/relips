#pragma once
#include "gamestateparam.h"

class ResultStateParam :
	public GameStateParam
{
public:
	ResultStateParam(void);
	virtual ~ResultStateParam(void);

	int Score() const { return m_score; }
	void Score(int val) { m_score = val; }

protected:
	int m_score;

};
