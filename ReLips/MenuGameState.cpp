#include "stdafx.h"
#include "PlayStateParam.h"
#include "MenuGameState.h"

MenuGameState::MenuGameState( char *name )
	: GameState(name), 
	isBetweenMove(false), 
	locationToGo(0), 
	speed(800.0f), 
	iconWidth(100), 
	padding(50), 
	iconCount(0), 
	currentIndex(0)
{

}

MenuGameState::~MenuGameState(void)
{

}

bool MenuGameState::Update( const FrameEvent& evt )
{
	if(isBetweenMove){
		SceneNode *menuNode = m_sceneMgr->getSceneNode("MenuNode");
		if(menuNode->getPosition().x > locationToGo){
			menuNode->setPosition(menuNode->getPosition() 
				- Vector3(speed * evt.timeSinceLastFrame, 0, 0));

			if(menuNode->getPosition().x <= locationToGo){
				isBetweenMove = false;
			}
		}else if(menuNode->getPosition().x < locationToGo){
			menuNode->setPosition(menuNode->getPosition() 
				+ Vector3(speed * evt.timeSinceLastFrame, 0, 0));

			if(menuNode->getPosition().x >= locationToGo){
				isBetweenMove = false;
			}
		}
	}

	for(int i = 0 ; i < textList.size() ; i++){
		ObjectTextDisplay *d = textList[i];
		Ogre::WindowEventUtilities::messagePump();
		d->update();
	}

	// SetDebugText("current: ", currentIndex);
	
	return true;
}

void MenuGameState::Initialize()
{	
	m_sceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
	m_sceneMgr->setAmbientLight(ColourValue(1.0,1.0,1.0));
	m_sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	m_camera->setPosition(Vector3(0,0,600));
	m_camera->lookAt(Vector3(0,0,0));

	GETOVERLAY(menuOverlay,"MenuOverlay");
	// menuOverlay->show();

	CreateMenuItems();

}

//////////////////////////////////////////////////////////////////////////
// Keyboard Handling
//////////////////////////////////////////////////////////////////////////

bool MenuGameState::KeyPressed( const OIS::KeyEvent &arg )
{
	if(isBetweenMove == false){
		SceneNode *menuNode = m_sceneMgr->getSceneNode("MenuNode");
		switch(arg.key){
			case OIS::KC_ESCAPE:
				{
					m_reqExitGame = true;
					break;
				}

			case OIS::KC_RETURN:
				{
					if(currentIndex == 0){
						// Change to PlayState
						m_reqChangeState = true;
						m_changeToStateName = "PlayState";

						PlayStateParam *p = new PlayStateParam();
						p->Difficulity(1);
						p->Filename("hello-foo-bar.mp3");
						m_paramToPass = p;
					}else if(currentIndex == 1){
						stringstream s;
						vector<wstring> files;

						if (ListFiles(L"C:\\", L"*", files)) {
							for (vector<wstring>::iterator it = files.begin(); 
								it != files.end(); 
								++it) {
									s << it->c_str() << endl;
							}
						}

						SetDebugText(s.str().c_str());
					}else if(currentIndex == 2){
					
					}else if(currentIndex == 3){

					
					}else if(currentIndex == 4){
						m_reqExitGame = true;
					}

					break;
				}

			case OIS::KC_RIGHT:
				if(++currentIndex < iconCount){
					isBetweenMove = true;
					locationToGo = (menuNode->getPosition() - Vector3(iconWidth + padding,0,0)).x;
				}else{
					currentIndex = iconCount - 1;
				}

				break;

			case OIS::KC_LEFT:
				if(--currentIndex >= 0){
					isBetweenMove = true;
					locationToGo = (menuNode->getPosition() + Vector3(iconWidth + padding,0,0)).x;
					break;
				}else{
					currentIndex = 0;
				}

				break;
		}
	}
	return true;
}

bool MenuGameState::KeyReleased( const OIS::KeyEvent &arg )
{
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Utility
//////////////////////////////////////////////////////////////////////////

void MenuGameState::SetDebugText(const char* s, int x){
	stringstream ss;
	ss << s << x;
	SetDebugText(ss.str().c_str());
}

void MenuGameState::SetDebugText(const char* s){
	GETOVERLAYELEM(debugText,"DebugText1");
	debugText->setCaption(s);
}
void MenuGameState::CreateBackgroundKnot(){

}
void MenuGameState::CleanUp()
{
	GETOVERLAY(menuOverlay,"MenuOverlay");
	menuOverlay->hide();

	for(int i = 0 ; i < textList.size() ; i++){
		ObjectTextDisplay *d  = textList[i];
		delete d;
	}

	textList.clear();
	list.clear();
}

MenuGameState *MenuGameState::CreateInstance()
{
	return new MenuGameState("MenuGameState");
}

void MenuGameState::CreateMenuItems()
{
	SceneNode *menuNode = m_sceneMgr->getRootSceneNode()->createChildSceneNode("MenuNode");
	menuNode->setPosition(Vector3::ZERO);
	Entity *ent;
	BillboardSet *bbset;
	Billboard *bb;
	ObjectTextDisplay* text;

	FloatRect rect(0.0, 0.0, 1.0, 1.0);

	// Quick Play Icon
	SceneNode *quickPlayNode = menuNode->createChildSceneNode("QuickPlayNode");

	bbset = m_sceneMgr->createBillboardSet("QuickPlayBBSet", 4);
	ReLipsUtil::createMaterialFromString("Hello Gant", "QuickPlayMat");
	bbset->setMaterialName("ReLips/Next");

	bb = bbset->createBillboard(Vector3::ZERO);
	quickPlayNode->attachObject(bbset);

	text = new ObjectTextDisplay(bbset,m_camera);
	text->enable(true);
	text->setText("Select Song");

	textList.push_back(text);

	list.push_back(quickPlayNode);

	// Voice Training Icon

	SceneNode *voiceTrainingNode = menuNode->createChildSceneNode("VoiceTrainingNode");
	bbset = m_sceneMgr->createBillboardSet("VoiceTrainingBBSet", 4);
	bbset->setMaterialName("ReLips/Setting");
	bb = bbset->createBillboard(Vector3::ZERO);
	voiceTrainingNode->attachObject(bbset);

	text = new ObjectTextDisplay(bbset,m_camera);
	text->enable(true);
	text->setText("Settings");

	textList.push_back(text);

	list.push_back(voiceTrainingNode);

	//// Settings Icon
	SceneNode *settingsNode = menuNode->createChildSceneNode("SettingsNode");
	bbset = m_sceneMgr->createBillboardSet("SettingsBBSet", 4);
	bbset->setMaterialName("ReLips/Stat");
	bb = bbset->createBillboard(Vector3::ZERO);
	settingsNode->attachObject(bbset);

	text = new ObjectTextDisplay(bbset,m_camera);
	text->enable(true);
	text->setText("Statistics");

	textList.push_back(text);

	list.push_back(settingsNode);

	//// Info Icon
	SceneNode *infoNode = menuNode->createChildSceneNode("InfoNode");
	bbset = m_sceneMgr->createBillboardSet("InfoBBSet", 4);
	bbset->setMaterialName("ReLips/Info");
	bb = bbset->createBillboard(Vector3::ZERO);
	infoNode->attachObject(bbset);

	text = new ObjectTextDisplay(bbset,m_camera);
	text->enable(true);
	text->setText("About");

	textList.push_back(text);

	list.push_back(infoNode);

	//// Exit Icon
	SceneNode *exitNode = menuNode->createChildSceneNode("ExitNode");
	bbset = m_sceneMgr->createBillboardSet("ExitBBSet", 4);
	bbset->setMaterialName("ReLips/Exit");
	bb = bbset->createBillboard(Vector3::ZERO);
	exitNode->attachObject(bbset);

	text = new ObjectTextDisplay(bbset,m_camera);
	text->enable(true);
	text->setText("Exit Game");

	textList.push_back(text);

	list.push_back(exitNode);

	iconCount = list.size();

	for(int i = 0 ; i < list.size() ; i++){
		SceneNode *n = list[i];
		n->setPosition(Vector3(i * (iconWidth + padding),0,0));
	}

	//SceneNode *root = m_sceneMgr->getRootSceneNode();
	//root->addChild(menuNode);
}

bool MenuGameState::ListFiles(wstring path, wstring mask, vector<wstring>& files) {
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA ffd;
	wstring spec;
	stack<wstring> directories;

	directories.push(path);
	files.clear();

	while (!directories.empty()) {
		path = directories.top();
		spec = path + L"\\" + mask;
		directories.pop();

		hFind = FindFirstFile(spec.c_str(), &ffd);
		if (hFind == INVALID_HANDLE_VALUE)  {
			return false;
		} 

		do {
			if (wcscmp(ffd.cFileName, L".") != 0 && 
				wcscmp(ffd.cFileName, L"..") != 0) {
					if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						directories.push(path + L"\\" + ffd.cFileName);
					}
					else {
						files.push_back(path + L"\\" + ffd.cFileName);
					}
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		if (GetLastError() != ERROR_NO_MORE_FILES) {
			FindClose(hFind);
			return false;
		}

		FindClose(hFind);
		hFind = INVALID_HANDLE_VALUE;
	}

	return true;
}
