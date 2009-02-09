#include "stdafx.h"

#include "GameStateManager.h"
#include "MenuGameState.h"
#include "PlayState.h"
#include "ResultGameState.h"
using namespace Ogre;


///////////////////////////////////////////////////////

class ReLipsListener : public FrameListener
{
public:
	ReLipsListener(OIS::Keyboard *keyboard, 
		OIS::Mouse *mouse, 
		OIS::JoyStick *joyStick)
		: mKeyboard(keyboard), mMouse(mouse), mJoy(joyStick)
	{
		mContinue = true;
	}

	bool frameStarted(const FrameEvent& evt)
	{
		mKeyboard->capture();
		mMouse->capture();

		return mContinue;
	}

	bool frameEnded(const FrameEvent& evt){
		return true;
	}

private:
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;
	OIS::JoyStick *mJoy;
	bool mContinue;
};

/////////////////////////////////////////////////////////

class BufferedInputHandler : public OIS::KeyListener, public OIS::MouseListener, public OIS::JoyStickListener
{
public:
    BufferedInputHandler(GameStateManager *gameStateMgr, 
		OIS::Keyboard *keyboard = 0, 
		OIS::Mouse *mouse = 0, 
		OIS::JoyStick *joystick = 0)
		: m_gameStateMgr(gameStateMgr)
    {
        if (keyboard)
            keyboard->setEventCallback(this);

        if (mouse)
            mouse->setEventCallback(this);

        if (joystick)
            joystick->setEventCallback(this);

    }

    // KeyListener
    virtual bool keyPressed(const OIS::KeyEvent &arg) { 
		return m_gameStateMgr->InjectKeyPressed(arg);
	}
    virtual bool keyReleased(const OIS::KeyEvent &arg) { 
		return m_gameStateMgr->InjectKeyReleased(arg);
	}

    // MouseListener
    virtual bool mouseMoved(const OIS::MouseEvent &arg) { 
		return true; 
	}
    virtual bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { 
		return true; 
	}
    virtual bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id) { 
		return true; 
	}

    // JoystickListener
    virtual bool buttonPressed(const OIS::JoyStickEvent &arg, int button) { return true; }
    virtual bool buttonReleased(const OIS::JoyStickEvent &arg, int button) { return true; }
    virtual bool axisMoved(const OIS::JoyStickEvent &arg, int axis) { return true; }

private:
	GameStateManager *m_gameStateMgr;
};


/////////////////////////////////////////////////////////

class Application : public FrameListener
{
public:
	void go()
	{
		createRoot();
		defineResources();
		setupRenderSystem();

		createRenderWindow();
		initializeResourceGroups();
		setupScene();
		setupGameStateManager();
		setupInputSystem();

		createFrameListener();
		startRenderLoop();
	}

	~Application()
	{

	   if(mListener)
			delete mListener;

	   if(mRoot)
			delete mRoot;

		mInputManager->destroyInputObject(mKeyboard);
		mInputManager->destroyInputObject(mMouse);
		// mInputManager->destroyInputObject(mJoy);

		OIS::InputManager::destroyInputSystem(mInputManager);

	}

private:
	Root *mRoot;
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;
	OIS::JoyStick *mJoy;
	OIS::InputManager *mInputManager;
	ReLipsListener *mListener;
	BufferedInputHandler *mInputHandler;
	GameStateManager *m_gameStateMgr;

	void createRoot()
	{
		mRoot = new Root();
	}

	void defineResources()
	{
		String secName, typeName, archName;
		ConfigFile cf;
		cf.load("resources.cfg");
		ConfigFile::SectionIterator seci = cf.getSectionIterator();
		while (seci.hasMoreElements())
		{
			secName = seci.peekNextKey();
			ConfigFile::SettingsMultiMap *settings = seci.getNext();
			ConfigFile::SettingsMultiMap::iterator i;

			for (i = settings->begin(); i != settings->end(); ++i)
			{
				typeName = i->first;
				archName = i->second;
				ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
			}
		}

	}

	void setupRenderSystem()
	{
		// Show config dialog or just fix the settings
#if 1
		if (!mRoot->showConfigDialog()) // .. !mRoot->restoreConfig() && .. 
			throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");
#else
		RenderSystem *rs = mRoot->getRenderSystemByName("Direct3D9 Rendering Subsystem");
											 // or use "OpenGL Rendering Subsystem"
		mRoot->setRenderSystem(rs);
		rs->setConfigOption("Full Screen", "No");
		rs->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");
#endif

	}

	void createRenderWindow()
	{
		mRoot->initialise(true, "ReLips");
	}

	void initializeResourceGroups()
	{
		TextureManager::getSingleton().setDefaultNumMipmaps(5);
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	}

	void setupScene()
	{
		SceneManager *mgr = mRoot->createSceneManager(ST_GENERIC, "MainSceneMgr");
		Camera *cam = mgr->createCamera("MainCamera");
		Viewport *vp = mRoot->getAutoCreatedWindow()->addViewport(cam);
		// vp->setBackgroundColour(ColourValue(1.0,1.0,1.0));
	}

	void setupGameStateManager(){
		m_gameStateMgr = new GameStateManager();

		// Register States
		MenuGameState *menuGame = new MenuGameState("MenuGameState");
		m_gameStateMgr->RegisterGameState(menuGame);

		PlayState *play = new PlayState("PlayState");
		m_gameStateMgr->RegisterGameState(play);

		ResultGameState *result = new ResultGameState("ResultGameState");
		m_gameStateMgr->RegisterGameState(result);

		m_gameStateMgr->SetFirstGameStage(menuGame);
	}

	void setupInputSystem()
	{
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		OIS::ParamList pl;
		RenderWindow *win = mRoot->getAutoCreatedWindow();

		win->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		mInputManager = OIS::InputManager::createInputSystem(pl);

		try
		{
		   mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		   mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
		   // mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, true));

		   mInputHandler = new BufferedInputHandler(m_gameStateMgr, mKeyboard, mMouse, NULL);
		   
		   // mKeyboard->setEventCallback(mMenuManager);

		}
		catch (const OIS::Exception &e)
		{
		   throw Exception(42, e.eText, "Application::setupInputSystem");
		}

	}


	void createFrameListener()
	{
		mListener = new ReLipsListener(mKeyboard, mMouse, NULL);

		// Forward Frame Events to current state
		mRoot->addFrameListener(this);

		// Handle ESC keys and keyboard input
		mRoot->addFrameListener(mListener);
	}


	bool frameStarted(const FrameEvent& evt)
	{
		return m_gameStateMgr->FrameStarted(evt);
	}

	bool frameEnded(const FrameEvent& evt){
		return m_gameStateMgr->FrameEnded(evt);
	}


	void startRenderLoop()
	{
		mRoot->startRendering();
	}
};

#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
#else
int main(int argc, char **argv)
#endif
{
	try
	{
		Application app;
		app.go();
	}
	catch(Exception& e)
	{
#if OGRE_PLATFORM == PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		fprintf(stderr, "An exception has occurred: %s\n",
			e.getFullDescription().c_str());
#endif
	}

	return 0;
}
