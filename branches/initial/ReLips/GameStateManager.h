#pragma once
#include "GameState.h"
#include "stdafx.h"


using namespace Ogre;
using namespace std;

class GameStateManager
{
public:
	GameStateManager(void);

	~GameStateManager(void);

	void RegisterGameState(GameState *gameState);

	void SetFirstGameStage(GameState *gameState);

	bool FrameStarted(const FrameEvent& evt);

	bool FrameEnded(const FrameEvent& evt);

	void ExitGame();

	void ChangeGameState();

	bool IsNameConflict(const char *name);

	bool InjectKeyPressed(const OIS::KeyEvent &arg){
		return m_currentState->KeyPressed(arg);
	}

	bool InjectKeyReleased(const OIS::KeyEvent &arg){
		return m_currentState->KeyReleased(arg);
	}

private:
	vector<GameState *> m_stateList;

	GameState *m_currentState;
};
