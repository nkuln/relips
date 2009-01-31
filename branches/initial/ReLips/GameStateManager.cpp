#include "stdafx.h"
#include "GameStateManager.h"

GameStateManager::GameStateManager(void)
{
}

GameStateManager::~GameStateManager(void)
{
}

void GameStateManager::RegisterGameState( GameState *gameState )
{
	if(IsNameConflict(gameState->Name())){
		cout << "Error! Name Conflict!";
		exit(-1);
	}

	m_stateList.push_back(gameState);
}

void GameStateManager::SetFirstGameStage( GameState *gameState )
{
	m_currentState = gameState;
	m_currentState->BringUp();
}

bool GameStateManager::FrameStarted(const FrameEvent& evt )
{
	return m_currentState->Update(evt);
}

bool GameStateManager::FrameEnded(const FrameEvent& evt )
{
	if(m_currentState->ReqChangeState() == true){
		ChangeGameState();
	}else if(m_currentState->ReqExitGame() == true){
		ExitGame();
		return false;
	}
	return true;
}

void GameStateManager::ExitGame()
{
	m_currentState->BreakDown();
	m_stateList.clear();
}

void GameStateManager::ChangeGameState()
{
	char *changeTo = m_currentState->ChangeToStateName();
	m_currentState->BreakDown();

	for(int i = 0 ; i < m_stateList.size() ; i++){
		if(strcmp(changeTo, m_stateList[i]->Name()) == 0){
			GameStateParam *param = m_currentState->ParamToPass();
			m_currentState = m_stateList[i]->CreateInstance();
			m_currentState->Param(param);
			m_currentState->BringUp();
			return;
		}
	}

	cout << "Error! " << changeTo << " not found!";
	return;
}

bool GameStateManager::IsNameConflict( const char *name )
{
	for(int i = 0 ; i < m_stateList.size() ; i++){
		if(strcmp(name, m_stateList[i]->Name()) == 0){
			return true;
		}
	}
	return false;
}