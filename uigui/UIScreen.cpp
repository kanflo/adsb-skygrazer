#include <UIScreen.h>
#include <SDL.h>
#include <iostream>
#include <assert.h>


//#define UIGUI_VERBOSE

static bool gNeedRedraw = false;
SDL_DisplayMode gScreenMode;
bool gHasCheckedScreenSize = false;

UIScreen::UIScreen(UIGUI *inUIGUI)
{
	assert(inUIGUI);
    mRenderer = inUIGUI->GetRenderer();
	mTracking = NULL;
	mFocus= NULL;
	mLastClick.x = mLastClick.y = 0;
	mBgColor.r = 
	mBgColor.g = 
	mBgColor.b = 0;
	mBgColor.a = 255;

	mEventType = SDL_RegisterEvents(1);
	if (mEventType == ((Uint32)-1)) {
		printf("Error: UIScreen failed to register event type\n");
		exit(0);
	}
}

UIScreen::~UIScreen()
{
	list<UIElement*>::iterator i;

	for(i = mElements.begin(); i != mElements.end(); ++i)
	{
		delete (*i);
	}

	mElements.clear();
}

void UIScreen::setNeedRedraw(bool redraw)
{
	if (redraw) {
		SDL_Event event;
		event.type = SDL_USEREVENT;
		event.user.code = event_Redraw;
		event.user.data1 = 0;
		event.user.data2 = 0;
		SDL_PushEvent(&event);
	}
	gNeedRedraw = redraw;
}

bool UIScreen::needsRedraw()
{
	return gNeedRedraw;
}

/*
SDL_Renderer* UIScreen::GetRenderer()
{
	return inUIGUI->GetRenderer();
}
*/
void UIScreen::AddElement(UIElement *inNewElement)
{
	assert(inNewElement);
	mElements.push_front(inNewElement);
//	inNewElement->mSurface = mSurface;
	inNewElement->mRenderer = mRenderer;
}

void UIScreen::Open()
{
}

void UIScreen::Render()
{
	if (UIScreen::needsRedraw()) {
		SDL_SetRenderDrawColor(mRenderer, mBgColor.r, mBgColor.g, mBgColor.b, mBgColor.a);
		SDL_RenderClear(mRenderer);

		list<UIElement*>::reverse_iterator i;
		for(i = mElements.rbegin(); i != mElements.rend(); ++i) {
			(*i)->Render();
		}
#if 0
		if (mMousePointer) {
			// draw the mouse pointer
			SDL_BlitSurface(mMousePointer, NULL, mSurface, &mMousePosition);
		}
#endif
		// update the screen
		SDL_RenderPresent(mRenderer);

		UIScreen::setNeedRedraw(false);
	}
}

void UIScreen::Run()
{
}

void UIScreen::Close()
{
}

// Return first element at given coordinate or NULL if none found.
// If elements overlap, the most recent added element will be returned.
UIElement* UIScreen::FindElementAt(int inX, int inY)
{
	list<UIElement*>::iterator i;

	for(i = mElements.begin(); i != mElements.end(); ++i) {
		if ((*i)->WithinBounds(inX, inY)) {
			return (*i);
		}
	}
	return NULL;
}

UIElement* UIScreen::FindElementId(int inId)
{
	list<UIElement*>::iterator i;

	for(i = mElements.begin(); i != mElements.end(); ++i) {
		if ((*i)->GetId() == inId) {
			return (*i);
		}
	}
	return NULL;
}

bool UIScreen::handleEvent(SDL_Event *inEvent)
{
	uint32_t command;
	bool handledEvent = false;
	switch(inEvent->type) {
		case SDL_KEYDOWN:
			handledEvent = true;
#ifdef UIGUI_VERBOSE
			fprintf(stderr, "Key down\n");
#endif // UIGUI_VERBOSE
			if (mFocus) {
				command = mFocus->HandleKeyDownEvent(inEvent);
				if (command != msg_None) {
					inEvent->type = getEventType();
					inEvent->user.code = command;
					handledEvent = false;
				}
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			handledEvent = true;
			if (!mTracking) {
#ifdef UIGUI_VERBOSE
				fprintf(stderr, "Mouse button down\n");
#endif // UIGUI_VERBOSE
				mTracking = FindElementAt(inEvent->motion.x, inEvent->motion.y);
				printf("Tracking 0x%x\n", mTracking);
				if (mTracking) {
#ifdef UIGUI_VERBOSE
					fprintf(stderr, "Start tracking\n");
#endif // UIGUI_VERBOSE
					if (mTracking) {
						mTracking->ReceivedTracking();
					}
				}
			}
//			mFocus = NULL;
			break;
		case SDL_MOUSEBUTTONUP:
			handledEvent = true;
			if (mTracking) {
				if (mFocus != mTracking) {
#ifdef UIGUI_VERBOSE
					fprintf(stderr, "Mouse button up\n");
#endif // UIGUI_VERBOSE
					if (mTracking) {
						mTracking->LostTracking();
					}
					if (mFocus) {
						mFocus->LostFocus();
						mFocus = NULL;
					}
					if (mTracking == FindElementAt(inEvent->motion.x, inEvent->motion.y)) {
#ifdef UIGUI_VERBOSE
						fprintf(stderr, ">>>> Tracking element hit %d\n", mTracking->MayReceiveFocus());
#endif // UIGUI_VERBOSE
						if (mTracking->MayReceiveFocus()) {
#ifdef UIGUI_VERBOSE
							fprintf(stderr, ">>>> May receive focus\n");
#endif // UIGUI_VERBOSE
							if (mFocus) {
								mFocus->LostFocus();
							}
							mFocus = mTracking;
							mFocus->ReceivedFocus();
						}
#ifdef UIGUI_VERBOSE
						fprintf(stderr, "Clicked\n");
#endif // UIGUI_VERBOSE
						if (mTracking) {
							command = mTracking->HandleMouseUpEvent(inEvent);
							if (command != msg_None) {
								mTracking->LostTracking();
								mTracking = NULL;
								mLastClick.x = inEvent->motion.x;
								mLastClick.y = inEvent->motion.y;
								printf("Got command %d (0x%x) for evt type %d\n", command, command, getEventType());
								inEvent->type = getEventType();
								inEvent->user.code = command;
								handledEvent = false;
							}
						} else {
							printf("Not tracking\n");
						}
					}
				}
				if (mTracking) {
					mTracking->LostTracking();
				}
				mTracking = NULL;
			}
			break;
		case SDL_MOUSEMOTION:
			handledEvent = true;
			if (mTracking) {
#ifdef UIGUI_VERBOSE
				fprintf(stderr, "Tracking\n");
#endif // UIGUI_VERBOSE
//				mTracking->HandleMouseMotionEvent(inEvent);
			}
		default:
			break;
	}

	// TODO. This isn't nice, but if not, we will get flooded
	// with key down events for some weird reason...
//	inEvent->type = SDL_NOEVENT;
	return handledEvent;
}

void UIScreen::SetFocus(UIElement *inNewFocus)
{
	assert(inNewFocus);
	mFocus = inNewFocus;
	mFocus->ReceivedFocus();
}

SDL_Point* UIScreen::getLastTap()
{
	return &mLastClick;
}

SDL_DisplayMode* UIScreen::getDisplayMode()
{
	if (!gHasCheckedScreenSize) {
	    int i;
	    // Get current display mode of all displays.
	    for(i = 0; i < SDL_GetNumVideoDisplays(); ++i) {
	        int err = SDL_GetCurrentDisplayMode(i, &gScreenMode);
	        if(err != 0) {
	            SDL_Log("Could not get display mode for video display #%d: %s", i, SDL_GetError());
			} else {
	            // On success, print the current display mode.
	            SDL_Log("Display #%d: current display mode is %dx%dpx @ %dhz. \n", i, gScreenMode.w, gScreenMode.h, gScreenMode.refresh_rate);
	        }
		}
		gHasCheckedScreenSize = true;
	}
	return &gScreenMode;
}

void UIScreen::setBackgroundColor(SDL_Color &color)
{
	mBgColor = color;
}
