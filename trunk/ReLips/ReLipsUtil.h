#pragma once
#include "OgreFont.h"
#include "OgreFontManager.h"
#include "stdafx.h"


using namespace Ogre;
using namespace std;

class ReLipsUtil
{
public:
	ReLipsUtil(void);
	~ReLipsUtil(void);
	static void writeToTexture(const String &str, TexturePtr destTexture, Image::Box destRectangle, Font* font, const ColourValue &color, char justify,  bool wordwrap);
	static void createMaterialFromString(const String &str, const String &matname);
};
