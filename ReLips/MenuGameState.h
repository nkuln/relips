#pragma once
#include "GameState.h"
#include "ObjectTextDisplay.h"
#include "ReLipsUtil.h"
#include "stdafx.h"
#include "boost/filesystem.hpp"
#include <iostream>

using namespace Ogre;
using namespace std;
namespace fs = boost::filesystem;

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
	void ReadReLipsFile();

	void ShowAdditionalMenu();

	virtual void CleanUp();
	void CreateBackgroundKnot();
	void SetDebugText(const char* s);
	void SetDebugText(const char* s, int x);
	void UpdateCurrentFileText();
	bool isBetweenMove;
	int locationToGo;
	float speed;
	int iconWidth;
	int iconCount;
	int currentIndex;
	int padding;

	int currentFile;

	vector<SceneNode *> list;
	vector<ObjectTextDisplay *> textList;
	vector<fs::path> filelist;
};
