#pragma once
#include "GameState.h"
#include "ObjectTextDisplay.h"
#include "ReLipsUtil.h"
#include "stdafx.h"

using namespace Ogre;
using namespace std;

class MenuGameState :
	public GameState
{
public:
	MenuGameState(char *name);
	virtual ~MenuGameState(void);

	virtual bool Update(const FrameEvent& evt);

	virtual bool KeyPressed(const OIS::KeyEvent &arg);

	virtual bool KeyReleased(const OIS::KeyEvent &arg);

	virtual MenuGameState *CreateInstance();

protected:

	virtual void Initialize();

	void CreateMenuItems();
	virtual void CleanUp();
	void CreateBackgroundKnot();
	void SetDebugText(const char* s);
	void SetDebugText(const char* s, int x);
	bool ListFiles(wstring path, wstring mask, vector<wstring>& files);
	bool isBetweenMove;
	int locationToGo;
	float speed;
	int iconWidth;
	int iconCount;
	int currentIndex;
	int padding;
	vector<SceneNode *> list;
	vector<ObjectTextDisplay *> textList;
};
