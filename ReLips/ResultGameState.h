#pragma once
#include "gamestate.h"

class ResultGameState :
	public GameState
{
public:
	ResultGameState(char* s);
	virtual ~ResultGameState(void);


	virtual bool Update(const FrameEvent& evt);

	virtual bool KeyPressed(const OIS::KeyEvent &arg);

	virtual bool KeyReleased(const OIS::KeyEvent &arg);

	virtual ResultGameState *CreateInstance();

protected:

	virtual void Initialize();

	virtual void CleanUp();
	void UpdateScore();
	int m_currentScore;
	int m_targetScore;
};
