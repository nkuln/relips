#pragma once
#include "stdafx.h"

class ObjectTextDisplay {

public:
	ObjectTextDisplay(const Ogre::MovableObject* p, const Ogre::Camera* c) {
		// generate unique name
		shapeName << p->getName() << "TextDisplay";
		containerName << p->getName() << "Container";
		textName << p->getName() << "Text";

		m_p = p;
		m_c = c;
		m_enabled = false;
		m_text = "";

		// create an overlay that we can use for later
		m_pOverlay = Ogre::OverlayManager::getSingleton().create(shapeName.str());
		m_pContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement(
			"Panel", containerName.str()));

		m_pOverlay->add2D(m_pContainer);

		m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", textName.str());
		m_pText->setDimensions(1.0, 1.0);
		m_pText->setMetricsMode(Ogre::GMM_RELATIVE);
		m_pText->setPosition(0, 0);

		m_pText->setParameter("font_name", "BerlinSans32");
		m_pText->setParameter("char_height", "0.05");
		m_pText->setParameter("horz_align", "center");

		m_pContainer->addChild(m_pText);
		m_pOverlay->show();
	}

	virtual ~ObjectTextDisplay() {

		// overlay cleanup -- Ogre would clean this up at app exit but if your app 
		// tends to create and delete these objects often it's a good idea to do it here.

		m_pOverlay->hide();
		Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
		m_pContainer->removeChild(textName.str());
		m_pOverlay->remove2D(m_pContainer);
		overlayManager->destroyOverlayElement(m_pText);
		overlayManager->destroyOverlayElement(m_pContainer);
		overlayManager->destroy(m_pOverlay);
	}

	void enable(bool enable) {
		m_enabled = enable;
		if (enable)
			m_pOverlay->show();
		else
			m_pOverlay->hide();
	}

	void setText(const Ogre::String& text) {
		m_text = text;
		m_pText->setCaption(m_text);
	}

	void update();

protected:
	const Ogre::MovableObject* m_p;
	const Ogre::Camera* m_c;
	bool m_enabled;
	Ogre::Overlay* m_pOverlay;
	Ogre::OverlayElement* m_pText;
	Ogre::OverlayContainer* m_pContainer;
	Ogre::String m_text;
	stringstream shapeName, containerName, textName;
};
