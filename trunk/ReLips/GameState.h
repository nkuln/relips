#pragma once
#include "GameStateParam.h"
#include "stdafx.h"

using namespace Ogre;

class GameState
{
public:
	GameState(char *name = "");

	virtual ~GameState(void);

	void BringUp();
	void BreakDown();

	virtual bool Update(const FrameEvent& evt);

	virtual bool KeyPressed(const OIS::KeyEvent &arg);

	virtual bool KeyReleased(const OIS::KeyEvent &arg);

	virtual GameState *CreateInstance();


	SceneManager * SceneMgr() const { return m_sceneMgr; }

	char * Name() const { return m_name; }
	bool ReqChangeState() const { return m_reqChangeState; }
	bool ReqExitGame() const { return m_reqExitGame; }
	char * ChangeToStateName() const { return m_changeToStateName; }
	GameStateParam * Param() const { return m_param; }
	void Param(GameStateParam * val) { m_param = val; }
	GameStateParam * ParamToPass() const { return m_paramToPass; }
	
protected:

	virtual void Initialize();

	virtual void CleanUp();

	SceneManager *m_sceneMgr;

	char *m_name;
	
	bool m_reqChangeState;
	
	bool m_reqExitGame;
	
	char *m_changeToStateName;

	GameStateParam *m_param;

	Camera *m_camera;

	GameStateParam *m_paramToPass;
};
