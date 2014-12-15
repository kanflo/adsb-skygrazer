#include <UIGUI.h>
#include <UIScreen.h>
#include <iostream>
#include <assert.h>

//#define FULL_SCREEN

UIGUI *g_gui;


static SDL_Renderer *gRenderer;
static SDL_Point gWindowSize;

// Pre defined standard colors
SDL_Color UIGUI::White =   {0xff, 0xff, 0xff, 0};
SDL_Color UIGUI::Black =   {   0,    0,    0, 0};
SDL_Color UIGUI::Red =     {0xff,    0,    0, 0};
SDL_Color UIGUI::Green =   {   0, 0xff,    0, 0};
SDL_Color UIGUI::Blue =    {   0,    0, 0xff, 0};
SDL_Color UIGUI::Yellow =  {0xff, 0xff,    0, 0};
SDL_Color UIGUI::Purple =  {0xff,    0, 0xff, 0};
SDL_Color UIGUI::Cyan =    {   0, 0xff, 0xff, 0};
SDL_Color UIGUI::Transparent =    {   0, 0, 0, 0};

// Color used when mouse button is down and we are tracking a control
SDL_Color UIGUI::TrackingColor = {0x55, 0xaa,    0, 0};
// Color used when contol received focus
SDL_Color UIGUI::FocusColor    = {0xff, 0xff, 0xff, 0};
// Shadow color, not used when the text has the same color as the shadow color
SDL_Color UIGUI::ShadowColor   = {   0,    0,   0, 0};

// Color used to fille when mouse button is down and we are tracking a control
SDL_Color UIGUI::BackgroundColor = {0, 0, 0, 128};
SDL_Color UIGUI::TrackingBackgroundColor = {0x20, 0x20, 0x20, 128};


UIGUI::UIGUI(uint32_t inWidth, uint32_t inHeight, uint32_t inCustomMousePointer)
{
	g_gui = this;

//	SDL_Surface *temp;

	mWidth = inWidth;
	mHeight = inHeight;
	mMousePointer = NULL;
	mBackgroundTile = NULL;

	// initialize video system
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	// set the title bar
//	SDL_WM_SetCaption("UIGUI", "UIGUI");

//    (void) SDL_EnableUNICODE(1);
//    (void) SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	// Set the video mode, go fullscreen
	mWindow = SDL_CreateWindow("UIGUI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, inWidth, inHeight, 0);
	gWindowSize.x = inWidth;
	gWindowSize.y = inHeight;
	printf("Error: %s\n", SDL_GetError());
	assert(mWindow);
	mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
	assert(mRenderer);
	gRenderer = mRenderer;

	// Get the current video hardware information
///	const SDL_VideoInfo* myPointer = SDL_GetVideoInfo();

	// Print out some information
	// WARNING: myPointer is not NULL here
//	cout << "Current video resolution is " << myPointer->current_w << "x" << myPointer->current_h << " pixels" << endl;
    int32_t i;
    SDL_DisplayMode current;

    // Get current display mode of all displays.
    for(i = 0; i < SDL_GetNumVideoDisplays(); ++i) {
        int err = SDL_GetCurrentDisplayMode(i, &current);
        if(err != 0) {
            // In case of error...
            SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
//			assert(0);
        } else {
            // On success, print the current display mode.
            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. \n", i, current.w, current.h, current.refresh_rate);
            mWidth = current.w;
            mHeight = current.h;
        }
    }


#if 0
	if (inCustomMousePointer)
	{
		// Hide the mouse pointer
		(void) SDL_ShowCursor(SDL_DISABLE);

		// load mouse pointer
		temp = IMG_Load("images\\xmastree-64-shadow.png");
		if (!temp)
		{
			printf("Failed to load mouse pointer\n");
			assert(0);
		}
		this->mMousePointer = SDL_DisplayFormatAlpha(temp);
		SDL_FreeSurface(temp);
	}
	// load background tile
	temp  = SDL_LoadBMP("images\\grass.bmp");
	assert(temp);
	mBackgroundTile = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
#endif

	if (TTF_Init() < 0) {
		fprintf(stderr, "Couldn't initialize TTF: %s\n",SDL_GetError());
		SDL_Quit();
		exit(2);
	}

	memset(&mMousePosition, 0, sizeof(mMousePosition));
	mEventType = SDL_RegisterEvents(1);
}

UIGUI::~UIGUI()
{
	SDL_FreeSurface(mSurface);

	if (mMousePointer)
	{
		(void) SDL_ShowCursor(SDL_ENABLE);
		SDL_FreeSurface(mMousePointer);
	}

	TTF_Quit();
	SDL_Quit();
}

SDL_Renderer* UIGUI::GetRenderer()
{
	return gRenderer;
}

/*
SDL_Surface* UIGUI::GetSurface()
{
	return mSurface;
}
*/

void UIGUI::Render()
{
	// Do nothing here
}

void UIGUI::run(UIScreen *inScreen, SDL_Event *event)
{
    SDL_zero(*event);
	int command = msg_None;

	assert(inScreen);

	mLastMouseButtonPressed = false;
	/* message pump */
	while (command == msg_None)	{
		bool eventHandled = false;
#if 1
		if (SDL_WaitEvent(event)) {
#else
		/* look for an event */
		if (SDL_PollEvent(event)) {
#endif
			if (event->type == mEventType) {
				if (event->user.code == event_Redraw) {
					eventHandled = true;
					// Redraw
				} else {
					return;
				}
				
			} else switch (event->type) {
//				case SDL_USEREVENT:
//					break;

				/* close button clicked */
				case SDL_QUIT:
					event->type = mEventType;
					event->user.code = event_Quit;
					return;
					break;

				/* handle the keyboard */
				case SDL_KEYDOWN:
					switch (event->key.keysym.sym) {
						case SDLK_ESCAPE:
							event->type = mEventType;
							event->user.code = event_Quit;
							return;
							break;
						default:
							break;
							// nada
					}
					break;

#if 0
                case SDL_MOUSEMOTION:
					eventHandled = true;
                    mMousePosition.x = event->motion.x;
                    mMousePosition.y = event->motion.y;

					mMousePosition.x -= 6;
					mMousePosition.y -= 6;

					/* collide with edges of screen */
					if ( mMousePosition.x < 0 ) {
						mMousePosition.x = 0;
					} else if ( mMousePosition.x > mWidth) {
						mMousePosition.x = mWidth;
					}
					if ( mMousePosition.y < 0 ) {
						mMousePosition.y = 0;
					} else if ( mMousePosition.y > mHeight) {
						mMousePosition.y = mHeight;
					}
					if ( mMousePosition.y > 300) {
						SDL_Event event;

						event->type = SDL_USEREVENT;
						event->user.code = 0x12345678;
						event->user.data1 = 0;
						event->user.data2 = 0;
						SDL_PushEvent(&event);
					}
                    break;
#endif
//            OnMouseMove(Event->motion.x,Event->motion.y,Event->motion.xrel,Event->motion.yrel,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_RIGHT))!=0,(Event->motion.state&SDL_BUTTON(SDL_BUTTON_MIDDLE))!=0);
				default:
					break;
					// nada
			}

			if ((event->motion.state & SDL_BUTTON(SDL_BUTTON_LEFT)) != mLastMouseButtonPressed) {
				mLastMouseButtonPressed = event->motion.state&SDL_BUTTON(SDL_BUTTON_LEFT);
			}

			// Let the screen handle events
			if (inScreen) {
				eventHandled |= inScreen->handleEvent(event);
			}

			// Draw the screen-on-display
			inScreen->Render();

			if (!eventHandled) {
//				printf("Event not handled, returning\n");
				return;
			}
		}
	}
}

void UIGUI::DisplayMessage(string message)
{
#if 0
	UIScreen *s = new UIScreen(g_gui);
	int cmd_Ok = 1000;

	s->AddElement((UIElement *) new UICaption (id_Any,  20,  g_gui->mHeight/3, g_gui->mWidth-40, 60, message, "impact", 48, UIGUI::White, centerAdjust));
	s->AddElement((UIElement *) new UIButton      (id_Any, (g_gui->mWidth-300)/2, g_gui->mHeight/3+150, 300, 60, cmd_Ok, "Ok", "impact", 48, UIGUI::White));

	int command;
	while(1) {
		command = g_gui->Run(s);
		if (command == cmd_Ok) {
			break;
		}
	}
	delete s;
#endif
}

uint32_t UIGUI::DisplayYesNoMessage(string message)
{
#if 0
	UIScreen *s = new UIScreen(g_gui);

	s->AddElement((UIElement *) new UICaption (id_Any,  20,  g_gui->mHeight/3, g_gui->mWidth-40, 60, message, "impact", 48, UIGUI::White, centerAdjust));
	s->AddElement((UIElement *) new UIButton      (id_Any, 200, g_gui->mHeight/3+150, 300, 60, msg_Yes, "Ja", "impact", 48, UIGUI::White));
	s->AddElement((UIElement *) new UIButton      (id_Any, g_gui->mWidth-500, g_gui->mHeight/3+150, 300, 60, msg_No, "Nej", "impact", 48, UIGUI::White));

	int command;
	while(1) {
		command = g_gui->Run(s);
		if (command == msg_Yes || command == msg_No) {
			cout << "User selected command " << command << endl;
			delete s;
			return command;
		}
	}
	// Never reached
	delete s;
#endif
	return msg_Yes;
}

SDL_Texture* UIGUI::RenderText(string inString, string inFontPath, uint32_t inFontSize, SDL_Color &inColor)
{
	TTF_Font *font;
	SDL_Surface *surface;
	
	if (inString.length() == 0) {
		return NULL;
	}
	
#if 1
	font = TTF_OpenFont("/usr/share/fonts/truetype/droid/DroidSans.ttf"/*inFontPath.c_str()*/, inFontSize);
#else
	font = TTF_OpenFont("/opt/vc/src/hello_pi/hello_font/Vera.ttf"/*inFontPath.c_str()*/, inFontSize);
#endif

	assert(font);

//	cout << "Rendering '" << inString.c_str() << "'\n";

//	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
#if 1
	surface = TTF_RenderText_Blended(font, inString.c_str(), inColor);
#else
	surface = TTF_RenderText_Solid(font, inString.c_str(), inColor);
#endif
	assert(surface);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
	assert(gRenderer);
	assert(texture);
	SDL_FreeSurface(surface);

	TTF_CloseFont(font);
	return texture;
}

SDL_Point* UIGUI::getWindowSize()
{
	return &gWindowSize;
}
