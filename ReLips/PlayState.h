#pragma once
#include "gamestate.h"
#include "Stage.h"
#include "stdafx.h"

using namespace std;

extern "C"{
	void CALLBACK SpectrumReceived( UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );
	BOOL CALLBACK DuffRecording( HRECORD handle, const void *buffer, DWORD length, void *user );
};

// pseudo-this bundle structure
typedef struct tagthisInstance
{
	DWORD   pseudoThis;
	DWORD   dwInstance;
} thisInstance,*LPTHISINSTANCE;

class PlayState :
	public GameState
{
public:
	PlayState(char *name);
	~PlayState(void);

	virtual bool Update(const FrameEvent& evt);

	void UpdateFragment( long lpos );
	void AddNotes( long lpos );

	void SetMarkerPosition(int pos);

	long GetCurrentPosition();

	virtual bool KeyPressed(const OIS::KeyEvent &arg);

	virtual bool KeyReleased(const OIS::KeyEvent &arg);

	virtual PlayState *CreateInstance();

	virtual void TimerTick();

protected:

	static const int FRAGMENT_SCALING = 60;

	virtual void Initialize();

	void BassInitAndPlay();
	virtual void CleanUp();
	void SetScore(int score);
	void InitAnimations();
	void Animate(const FrameEvent& evt);
	HRECORD m_recChan;	// recording channel
	HSTREAM m_playChan; // playing channel
	DWORD m_timer; // update timer
	volatile int m_markerPos; 
	volatile int m_score; // current score
	volatile int m_transStart1;
	volatile bool m_transFlag1;
	volatile int m_transEnd1;
	int m_displayScore; // displaying score
	int m_pos; // current position
	ParticleSystem* m_particleSystem;
	SceneNode* m_particleNode;
	
	int m_lastNote;
	double m_lastCents;
	int m_lastPos;

	Stage *m_stage; // current stage
	string m_audio;
	thisInstance m_instance;
	deque<Fragment *> *m_queue; // temporarily store fragments
};
