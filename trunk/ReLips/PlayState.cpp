#include "stdafx.h"
#include "PlayState.h"
#include "Note.h"
#include "ResultGameState.h"
#include "ResultStateParam.h"
#define DEFAULT_RELIPS	"mile.relips" 
#define DEFAULT_PLAY	"mile.mp3"
#define CUSTOM_SHININESS	1
#define CUSTOM_DIFFUSE		2
#define CUSTOM_SPECULAR		3

PlayState::PlayState(char *name)
	: GameState(name),
	m_recChan(NULL),
	m_playChan(NULL),
	m_timer(NULL),
	m_stage(NULL),
	m_score(0),
	m_displayScore(0),
	m_pos(0),
	m_lastNote(-1),
	m_lastPos(0),
	m_lastCents(0),
	m_markerPos(0),
	m_transStart1(0),
	m_transEnd1(0),
	m_transFlag1(false)
{

}

PlayState::~PlayState(void)
{

}

bool PlayState::Update( const FrameEvent& evt )
{
	DWORD currentStat = BASS_ChannelIsActive(m_playChan);
	if(currentStat == BASS_ACTIVE_STOPPED){
		ResultStateParam* param = new ResultStateParam();
		param->Score(m_score);
		m_paramToPass = param;
		m_changeToStateName = "ResultGameState";
		m_reqChangeState = true;
	}else{
	
		// Get current position in song
		long lpos = GetCurrentPosition();

		// Update Fragment note
		UpdateFragment(lpos);

		// Update Score
		SetScore(m_score);

		// Set marker pos
		SetMarkerPosition(m_markerPos);

		// Add note that still not visible
		AddNotes(lpos);

		// Animtations
		Animate(evt);

	}
	return true;
}

void PlayState::Initialize()
{
	// Init Animations
	InitAnimations();

	// Show Overlay

	GETOVERLAY(scoreOverlay,"ScoreOverlay");
	scoreOverlay->show();

	m_stage = new Stage(DEFAULT_RELIPS);
	m_queue = new deque<Fragment *>();

	// Setup Scene
	m_sceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	m_sceneMgr->setAmbientLight(ColourValue(0.8,0.8,0.8));
	m_sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	// Setup Camera
	//m_camera->setPosition(0,150,1400);
	//m_camera->lookAt(0,0,-1000);

	// Setup Lights
	Light *light = m_sceneMgr->createLight("OverHeadLight");
	light->setPosition(0.0,40,800);
	light->setType(Light::LT_POINT);
	light->setDiffuseColour(ColourValue::White);
	light->setSpecularColour(ColourValue::White);
	light->setAttenuation(7000, 0.0, 0.0007, 0.000002);

	Light *light2 = m_sceneMgr->createLight("OverHeadLight2");
	light2->setPosition(0.0,100,1400);
	light2->setType(Light::LT_POINT);
	light2->setDiffuseColour(ColourValue::White);
	light2->setSpecularColour(ColourValue::White);
	light2->setAttenuation(7000, 0.0, 0.0007, 0.000002);




	// Setup Plane

	Plane ground(Vector3::UNIT_Y, 0);

	MeshManager::getSingleton().createPlane("GroundPlane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, ground,
		5000,2600,20,20,true,1,50,26,-Vector3::UNIT_Z);

	Entity *ent = m_sceneMgr->createEntity("GroundPlaneEnt","GroundPlane");
	ent->setMaterialName("Examples/TextureEffect4");
	ent->setCastShadows(false);

	CREATEROOTCHILD(groundNode,"GroundSceneNode");
	groundNode->attachObject(ent);
	groundNode->setPosition(0,-1,0);

	Plane plane(Vector3::UNIT_Y, 0);

	MeshManager::getSingleton().createPlane("NotePlane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		500,2600,20,20,true,1,1,1,-Vector3::UNIT_Z);

	ent = m_sceneMgr->createEntity("NotePlaneEnt","NotePlane");
	ent->setMaterialName("ReLips/NotePlane01");
	ent->setCastShadows(false);

	CREATEROOTCHILD(planeNode,"PlaneSceneNode");
	planeNode->attachObject(ent);

	// Setup Fragments Node
	CREATEROOTCHILD(fragParentNode,"FragmentParentNode");
	fragParentNode->setPosition(0,0,1000);
	SceneNode *fragNode = fragParentNode->createChildSceneNode("FragmentNode");

	// Setup Limiting plane

	Plane limit(Vector3::UNIT_Y, 0);

	MeshManager::getSingleton().createPlane("LimitPlane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, limit,
		500,800,20,20,true,1,5,8,-Vector3::UNIT_Z);

	Entity *limitEnt = m_sceneMgr->createEntity("LimitEnt","LimitPlane");
	limitEnt->setMaterialName("Examples/Rockwall");
	limitEnt->setCastShadows(false);

	CREATEROOTCHILD(limitNode,"LimitNode");
	limitNode->attachObject(limitEnt);
	limitNode->setPosition(0,1.0,1400);

	//Entity *limitEnt = m_sceneMgr->createEntity("LimitEnt",SceneManager::PrefabType::PT_PLANE);
	//limitEnt->setCastShadows(false);
	//CREATEROOTCHILD(limitNode,"LimitNode");
	//limitEnt->setMaterialName("Examples/Rockwall");
	//limitNode->setPosition(0.0,1.0,1400);
	//limitNode->attachObject(limitEnt);
	//limitNode->setScale(2.5,4,4);
	//limitNode->rotate(Quaternion(Degree(-90),Vector3::UNIT_X),Node::TransformSpace::TS_WORLD);
	
	// Setup Singing Pitch Marker
	Entity *marker = m_sceneMgr->createEntity("MarkerEnt",SceneManager::PrefabType::PT_PLANE);
	marker->setCastShadows(false);
	CREATEROOTCHILD(markerNode,"MarkerNode");
	marker->setMaterialName("ReLips/Star");
	markerNode->setPosition(0.0,2.0,1000);
	markerNode->attachObject(marker);
	markerNode->setScale(0.5,0.5,0.5);
	markerNode->rotate(Quaternion(Degree(-90),Vector3::UNIT_X),Node::TransformSpace::TS_WORLD);

	// Create shared node for 2 fountains
	SceneNode *foundtainNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode();

	// fountain 1
	m_particleSystem = m_sceneMgr->createParticleSystem("fountain1",
		"Examples/PurpleFountain");
	// Point the fountain at an angle
	m_particleNode = markerNode->createChildSceneNode();

	// fNode->translate(200,-100,0);
	// fNode->rotate(Vector3::UNIT_X, Degree(20));
	m_particleNode->setOrientation(Quaternion(Degree(-30),Vector3::UNIT_X));
	m_particleNode->attachObject(m_particleSystem);

	// Start BASS
	BassInitAndPlay();
}


void PlayState::TimerTick(){

	DWORD level = BASS_ChannelGetLevel(m_recChan);
	int leftLevel = LOWORD(level);
	int rightLevel = HIWORD(right);

	long lpos = GetCurrentPosition();

	//OverlayElement *elem = OverlayManager::getSingleton().getOverlayElement("TextAreaOverlay");
	//if(leftLevel > 10000){
	//	char buff[100];
	//	Note::ToSymbolString(note, buff);
	//	stringstream s;
	//	s << "Note: " << buff << " " << cents << " " << lpos;
	//	 elem->setCaption(s.str());
	//	m_markerPos = Note::GetPositionOnScale(note, cents, 400) - 200;
	//}else{
	//	elem->setCaption("-- ");
	//}

	//OverlayElement *elem = OverlayManager::getSingleton().getOverlayElement("TextAreaOverlay");
	//char buff[100];
	//Note::ToSymbolString(note, buff);
	//stringstream s;
	//s << "Size : " << m_queue->size();
	//elem->setCaption(s.str());

	if(m_queue->size() != 0){

		Fragment *front = m_queue->front();

		if(lpos > front->End()){
			m_queue->pop_front();
			delete front;
		}else{
			if(m_queue->size() != 0){
				front = m_queue->front();

				float fft[4096];
				BASS_ChannelGetData(m_recChan, fft, BASS_DATA_FFT8192); // get the FFT data

				double freq = Note::DetectPitch(fft,44100,8192);
				double cents = 0;
				int note = Note::ToSymbol(freq, cents);

				//note = Note::NT_D;
				//leftLevel = 5000;
				
				
				if(leftLevel >= 1500){
					if(lpos >= front->Start() && (m_lastNote == note) && (note == front->Note())){
						if(m_lastPos >= front->Start()){
							m_score += (int)((lpos - m_lastPos)*1.0/FRAGMENT_SCALING);

						}
					}
					m_particleNode->setOrientation(Quaternion(Degree(70),Vector3::UNIT_X));
					m_markerPos = Note::GetPositionOnScale(note, cents, 400) - 200;
				}else{
					m_particleNode->setOrientation(Quaternion(Degree(-30),Vector3::UNIT_X));
				}

				m_lastNote = note;
				m_lastCents = cents;
				m_lastPos = lpos;

			}
		}

	}
}

//////////////////////////////////////////////////////////////////////////
// Keyboard Handling
//////////////////////////////////////////////////////////////////////////

bool PlayState::KeyPressed( const OIS::KeyEvent &arg )
{
	switch(arg.key){
		case OIS::KC_R:
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

bool PlayState::KeyReleased( const OIS::KeyEvent &arg )
{
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Utility Functions
//////////////////////////////////////////////////////////////////////////

void PlayState::AddNotes( long frameLength )
{
	while(m_stage && m_stage->Frags()->size() != 0){
		Fragment *front = m_stage->Frags()->front();
		if(frameLength + 100000> front->Start()){
			stringstream uname;
			uname << "Frag" << front->Start();

			GETNODE(fragNode,"FragmentNode");
			SceneNode *t = fragNode->createChildSceneNode();


			Entity *e = m_sceneMgr->createEntity(uname.str() + "Ent",SceneManager::PrefabType::PT_CUBE);
			e->setMaterialName("Examples/TransparentTest");
			e->setCastShadows(true);

	/*		SubEntity* sub = e->getSubEntity(0);
			sub->setMaterialName("Examples/CelShading");
			sub->setCustomParameter(CUSTOM_SHININESS, Vector4(100.0f, 0.0f, 0.0f, 0.0f));
			sub->setCustomParameter(CUSTOM_DIFFUSE, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			sub->setCustomParameter(CUSTOM_SPECULAR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));*/

			t->attachObject(e);
			double length = 1.0 * front->Length()/FRAGMENT_SCALING;
			t->setScale(0.2, 0.02, length/100);
			double xpos = Note::GetPositionOnScale(front->Note(), 400) - 200;
			t->setPosition(xpos,
				10, 
				-1.0 * (front->Start() / FRAGMENT_SCALING + length/2) );


			m_stage->Frags()->pop_front();
			m_queue->push_back(front);
		}else{
			break;
		} // end lpos
	}
}


void PlayState::SetScore(int score){
	stringstream s;
	s << "score " << (score);
	GETOVERLAYELEM(scoreText,"ScoreText1");
	scoreText->setCaption(s.str());
}

void PlayState::SetMarkerPosition(int pos)
{
	GETNODE(marker,"MarkerNode");
	Vector3 mpos = marker->getPosition();
	mpos.x = pos;
	marker->setPosition(mpos);
}

void CALLBACK SpectrumReceived( UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	PlayState *p = (PlayState *)dwUser;
	p->TimerTick();
}

BOOL CALLBACK DuffRecording( HRECORD handle, const void *buffer, DWORD length, void *user )
{
	return TRUE; // continue recording
}

PlayState *PlayState::CreateInstance()
{
	return new PlayState("PlayState");
}

long PlayState::GetCurrentPosition()
{
	QWORD pos = BASS_ChannelGetPosition(m_playChan,BASS_POS_BYTE);
	long lpos = (long)pos/4;
	return lpos;
}

void PlayState::CleanUp()
{
	if (m_timer)
		timeKillEvent(m_timer);

	delete m_stage;
	m_stage = NULL;

	BASS_RecordFree();
	BASS_Free();

	GETOVERLAY(scoreOverlay,"ScoreOverlay");
	scoreOverlay->hide();

}


void PlayState::BassInitAndPlay()
{
	// check the correct BASS was loaded
	if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
		LOG("An incorrect version of BASS.DLL was loaded")
			exit(-1);
	}

	// initialize BASS recording (default device)
	if (!BASS_RecordInit(-1)) {
		LOG("Can't initialize device")
			exit(-1);
	}

	// start recording (44100hz mono 16-bit)
	if (!(m_recChan = BASS_RecordStart(44100, 1, 0, &DuffRecording, 0))) {
		LOG("Can't start recording")
			exit(-1);
	}

	// setup update timer (40hz)
	m_timer = timeSetEvent(25, 25, (LPTIMECALLBACK)&SpectrumReceived, (DWORD)this, TIME_PERIODIC);

	// Play sound
	if(!BASS_Init(-1, 44100, 0, 0, NULL)){
		LOG("Can't initialize device")
			exit(-1);
	}

	DWORD act,time,level;
	BOOL ismod;
	QWORD pos;

	if(m_playChan = BASS_StreamCreateFile(FALSE, DEFAULT_PLAY, 0, 0, BASS_STREAM_PRESCAN | BASS_STREAM_AUTOFREE)){
		pos = BASS_ChannelGetLength(m_playChan, BASS_POS_BYTE);
		// printf("streaming file [%I64u bytes]",pos);
	}else{
		LOG("Cannot play file")
			exit(-1);
	}

	BASS_ChannelPlay(m_playChan, FALSE);
}

//////////////////////////////////////////////////////////////////////////
// Animation
//////////////////////////////////////////////////////////////////////////

void PlayState::InitAnimations(){
	// Camera
	m_transFlag1 = true;
	m_transStart1 = 500;
	m_transEnd1 = 150;
	m_camera->setPosition(0, m_transStart1,1600);
	m_camera->lookAt(0,0,-1000);		

}

void PlayState::Animate(const FrameEvent& evt){
	if(m_transFlag1){ // Camera
		if(m_camera->getPosition().y > 150){
			Vector3 v = m_camera->getPosition();
			v.y -= evt.timeSinceLastFrame * 200;
			m_camera->setPosition(v);
		}else{
			Vector3 v = m_camera->getPosition();
			v.y = 150;
			m_camera->setPosition(v);
			m_transFlag1 = false;
		}
		m_camera->lookAt(0,0,-1000);
	}

	GETNODE(m, "MarkerNode");
	m->rotate(Quaternion(Degree(100*evt.timeSinceLastFrame),Vector3::UNIT_Y),Node::TransformSpace::TS_PARENT);
}

void PlayState::UpdateFragment( long lpos )
{
	GETNODE(nn,"FragmentNode");
	if(nn)
		nn->setPosition(Vector3(0,0, 1.0 * lpos / FRAGMENT_SCALING));
	else
		exit(-1);
}