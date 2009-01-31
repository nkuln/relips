#include "stdafx.h"
#include "ReLipsUtil.h"

using namespace Ogre;
using namespace std;


ReLipsUtil::ReLipsUtil(void)
{

}

ReLipsUtil::~ReLipsUtil(void)
{
}


void ReLipsUtil::createMaterialFromString(const String &str, const String &matname){
	stringstream textureName;
	textureName << matname << "Texture";

	TexturePtr texture = TextureManager::getSingleton().createManual(
		textureName.str(), "General",
		TEX_TYPE_2D, 
		512, 50, 
		MIP_UNLIMITED , 
		PF_X8R8G8B8, 
		Ogre::TU_STATIC|Ogre::TU_AUTOMIPMAP
	);

	
	Font* font = (Font*)FontManager::getSingleton().getByName("StarWars").getPointer();

	writeToTexture("hello world",
		texture,
		Image::Box(10,10,500,40),
		font,
		ColourValue(1.0,1.0,1.0,1.0),
		'l',
		true
	);

	// Create a material using the texture
	MaterialPtr material = MaterialManager::getSingleton().create(
		matname, // name
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	material->getTechnique(0)->getPass(0)->createTextureUnitState(textureName.str());
	
}

// Taken from: http://www.ogre3d.org/wiki/index.php/HowTo:_Write_text_on_texture
// Example Usage:
/*

Texture* background = TextureManager::getSingleton().getByName("Background.png").getPointer();

Font* font = FontManager::getSingleton().getByName("BlueHighWay").getPointer();

// Make sure the texture is not WRITE_ONLY, we need to read the buffer to do the blending with the font (get the alpha for example)
Texture* texture = TextureManager::getSingleton().createManual("Write Texture","General",TEX_TYPE_2D, 512, 512, MIP_UNLIMITED , PF_X8R8G8B8, Ogre::TU_STATIC|Ogre::TU_AUTOMIPMAP).getPointer();

//Draw the background to the new texture
texture->getBuffer()->blit(background->getBuffer());

WriteToTexture("hello world",texture,Image::Box(25,275,370,500),font,ColourValue(1.0,1.0,1.0,1.0),'c');


*/
void ReLipsUtil::writeToTexture(const String &str, TexturePtr destTexture, Image::Box destRectangle, Font* font, const ColourValue &color, char justify = 'l',  bool wordwrap = true)
{

	if (destTexture->getHeight() < destRectangle.bottom)
		destRectangle.bottom = destTexture->getHeight();
	if (destTexture->getWidth() < destRectangle.right)
		destRectangle.right = destTexture->getWidth();

	if (!font->isLoaded())
		font->load();

	TexturePtr fontTexture = (TexturePtr) TextureManager::getSingleton().getByName(font->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->getTextureName());
	
	HardwarePixelBufferSharedPtr fontBuffer = fontTexture->getBuffer();
	HardwarePixelBufferSharedPtr destBuffer = destTexture->getBuffer();

	PixelBox destPb = destBuffer->lock(destRectangle,HardwareBuffer::HBL_NORMAL);
	
        // The font texture buffer was created write only...so we cannot read it back :o). One solution is to copy the buffer  instead of locking it. (Maybe there is a way to create a font texture which is not write_only ?)
        
        // create a buffer
        size_t nBuffSize = fontBuffer->getSizeInBytes();
        uint8* buffer = (uint8*)calloc(nBuffSize, sizeof(uint8)); 
        
        // create pixel box using the copy of the buffer
        PixelBox fontPb(fontBuffer->getWidth(), fontBuffer->getHeight(),fontBuffer->getDepth(), fontBuffer->getFormat(), buffer);          
        fontBuffer->blitToMemory(fontPb);

	uint8* fontData = static_cast<uint8*>( fontPb.data );
	uint8* destData = static_cast<uint8*>( destPb.data );

	const size_t fontPixelSize = PixelUtil::getNumElemBytes(fontPb.format);
	const size_t destPixelSize = PixelUtil::getNumElemBytes(destPb.format);

	const size_t fontRowPitchBytes = fontPb.rowPitch * fontPixelSize;
	const size_t destRowPitchBytes = destPb.rowPitch * destPixelSize;

	Box *GlyphTexCoords;
	GlyphTexCoords = new Box[str.size()];

	Font::UVRect glypheTexRect;
	size_t charheight = 0;
	size_t charwidth = 0;

	for(unsigned int i = 0; i < str.size(); i++)
	{
		if ((str[i] != '\t') && (str[i] != '\n') && (str[i] != ' '))
		{
			glypheTexRect = font->getGlyphTexCoords(str[i]);
			GlyphTexCoords[i].left = glypheTexRect.left * fontTexture->getSrcWidth();
			GlyphTexCoords[i].top = glypheTexRect.top * fontTexture->getSrcHeight();
			GlyphTexCoords[i].right = glypheTexRect.right * fontTexture->getSrcWidth();
			GlyphTexCoords[i].bottom = glypheTexRect.bottom * fontTexture->getSrcHeight();

			if (GlyphTexCoords[i].getHeight() > charheight)
				charheight = GlyphTexCoords[i].getHeight();
			if (GlyphTexCoords[i].getWidth() > charwidth)
				charwidth = GlyphTexCoords[i].getWidth();
		}

	}	

	size_t cursorX = 0;
	size_t cursorY = 0;
	size_t lineend = destRectangle.getWidth();
	bool carriagreturn = true;
	for (unsigned int strindex = 0; strindex < str.size(); strindex++)
	{
		switch(str[strindex])
		{
		case ' ': cursorX += charwidth;  break;
		case '\t':cursorX += charwidth * 3; break;
		case '\n':cursorY += charheight; carriagreturn = true; break;
		default:
			{
				//wrapping
				if ((cursorX + GlyphTexCoords[strindex].getWidth()> lineend) && !carriagreturn )
				{
					cursorY += charheight;
					carriagreturn = true;
				}
				
				//justify
				if (carriagreturn)
				{
					size_t l = strindex;
					size_t textwidth = 0;	
					size_t wordwidth = 0;

					while( (l < str.size() ) && (str[l] != '\n)'))
					{		
						wordwidth = 0;

						switch (str[l])
						{
						case ' ': wordwidth = charwidth; ++l; break;
						case '\t': wordwidth = charwidth *3; ++l; break;
						case '\n': l = str.size();
						}
						
						if (wordwrap)
							while((l < str.size()) && (str[l] != ' ') && (str[l] != '\t') && (str[l] != '\n'))
							{
								wordwidth += GlyphTexCoords[l].getWidth();
								++l;
							}
						else
							{
								wordwidth += GlyphTexCoords[l].getWidth();
								l++;
							}
	
						if ((textwidth + wordwidth) <= destRectangle.getWidth())
							textwidth += (wordwidth);
						else
							break;
					}

					if ((textwidth == 0) && (wordwidth > destRectangle.getWidth()))
						textwidth = destRectangle.getWidth();

					switch (justify)
					{
					case 'c':	cursorX = (destRectangle.getWidth() - textwidth)/2;
							lineend = destRectangle.getWidth() - cursorX;
							break;

					case 'r':	cursorX = (destRectangle.getWidth() - textwidth);
							lineend = destRectangle.getWidth();
							break;

					default:	cursorX = 0;
							lineend = textwidth;
							break;
					}

					carriagreturn = false;
				}

				//abort - net enough space to draw
				if ((cursorY + charheight) > destRectangle.getHeight())
					goto stop;

				//draw pixel by pixel
				for (size_t i = 0; i < GlyphTexCoords[strindex].getHeight(); i++ )
					for (size_t j = 0; j < GlyphTexCoords[strindex].getWidth(); j++)
					{
 						float alpha =  color.a * (fontData[(i + GlyphTexCoords[strindex].top) * fontRowPitchBytes + (j + GlyphTexCoords[strindex].left) * fontPixelSize +1 ] / 255.0);
 						float invalpha = 1.0 - alpha;
 						size_t offset = (i + cursorY) * destRowPitchBytes + (j + cursorX) * destPixelSize;
  						ColourValue pix;
 						PixelUtil::unpackColour(&pix,destPb.format,&destData[offset]);
 						pix = (pix * invalpha) + (color * alpha);
 						PixelUtil::packColour(pix,destPb.format,&destData[offset]);
  					}
 
				cursorX += GlyphTexCoords[strindex].getWidth();
			}//default
		}//switch
	}//for

stop:
	delete[] GlyphTexCoords;

	destBuffer->unlock();
	
        // Free the memory allocated for the buffer
        free(buffer); buffer = 0;
}
