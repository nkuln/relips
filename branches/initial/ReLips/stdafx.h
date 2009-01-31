// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

// TODO: reference additional headers your program requires here

#include <windows.h>
#include <Ogre.h>
#include <OIS/OIS.h>
#include <OgreFont.h>
#include <OgreFontManager.h>
#include <OIS/OIS.h>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <stack>
#include <conio.h>
#include <deque>
#include "bass.h"

// User Defined Ogre Macros
#define GETNODE(newnode,nodename) SceneNode *newnode = m_sceneMgr->getSceneNode(nodename);
#define GETOVERLAY(newov,ovname) Overlay *newov = OverlayManager::getSingleton().getByName(ovname);
#define GETOVERLAYELEM(newelem,elemname) OverlayElement *newelem = OverlayManager::getSingleton().getOverlayElement(elemname);
#define CREATEROOTCHILD(newchild,childname) SceneNode *newchild = m_sceneMgr->getRootSceneNode()->createChildSceneNode(childname);
#define LOG(x) LogManager::getSingleton().logMessage(x);

using namespace std;