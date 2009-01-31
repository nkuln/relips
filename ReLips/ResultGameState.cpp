#include "StdAfx.h"
#include "ResultGameState.h"
#include "ResultStateParam.h"

ResultGameState::ResultGameState( char* s )
:GameState(s),
m_currentScore(0),
m_targetScore(0)
{

}
ResultGameState::~ResultGameState(void)
{
}

bool ResultGameState::Update( const FrameEvent& evt )
{
	if(m_currentScore < m_targetScore){
		int offset = (int)(evt.timeSinceLastFrame * 100.0);
		if(m_currentScore + offset < m_targetScore)
			m_currentScore += offset;
		else
			m_currentScore = m_targetScore;
	}
	UpdateScore();
	return true;
}

void ResultGameState::UpdateScore(){
	GETOVERLAYELEM(elem, "ResultText1");
	stringstream s;
	s << m_currentScore;
	elem->setCaption(s.str());
}

bool ResultGameState::KeyPressed( const OIS::KeyEvent &arg )
{
	switch(arg.key){
		case OIS::KC_RETURN:
			{
				m_reqChangeState = true;
				m_changeToStateName = "MenuGameState";
				m_paramToPass = NULL;
				break;
			}
		case OIS::KC_ESCAPE:
			{
				m_reqExitGame = true;
				break;
			}
	}
	return true;
}

bool ResultGameState::KeyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

ResultGameState * ResultGameState::CreateInstance()
{
	return new ResultGameState("ResultGameState");
}

void ResultGameState::Initialize()
{
	m_targetScore = static_cast<ResultStateParam *>(m_param)->Score();

	m_camera->setPosition(Vector3(0,0,600));
	m_camera->lookAt(Vector3(0,0,0));

	// Create some nice fireworks
	m_sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(
		m_sceneMgr->createParticleSystem("Fireworks", "Examples/Fireworks"));


	GETOVERLAY(ov, "ResultOverlay");
	ov->show();
}

void ResultGameState::CleanUp()
{
	GETOVERLAY(ov, "ResultOverlay");
	ov->hide();
}