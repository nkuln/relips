#include "stdafx.h"
#include "GameState.h"

GameState::GameState(char *name)
	: m_reqChangeState(false), 
	m_changeToStateName(""), 
	m_reqExitGame("false"), 
	m_name(""), 
	m_sceneMgr(NULL),
	m_param(NULL),
	m_paramToPass(NULL),
	m_camera(NULL)
{
	this->m_name = name;
}

GameState::~GameState(void)
{

}

void GameState::BringUp()
{
	m_sceneMgr = Root::getSingleton().getSceneManager("MainSceneMgr");
	m_camera = m_sceneMgr->getCamera("MainCamera");

	m_reqExitGame = false;
	m_reqChangeState = false;
	m_changeToStateName = "";

	Initialize();
}

void GameState::BreakDown()
{
	CleanUp();
	m_sceneMgr->clearScene();
	// m_sceneMgr->getRootSceneNode()->removeAndDestroyAllChildren();
	// m_sceneMgr->destroyAllMovableObjects();
}


void GameState::Initialize()
{

}

bool GameState::Update( const FrameEvent& evt )
{
	return true;
}

void GameState::CleanUp()
{

}

bool GameState::KeyPressed( const OIS::KeyEvent &arg )
{
	return true;
}

bool GameState::KeyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

GameState * GameState::CreateInstance()
{
	return NULL;
}