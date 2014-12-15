#ifndef __UIGUI_H__
#define __UIGUI_H__

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <string>
#include <assert.h>

using std::string;

class UIScreen;

enum uiguiTextAdjust
{
	leftAdjust = 0,
	centerAdjust,
	rightAdjust
};

#define msg_No   (-3)
#define msg_Yes  (-2)
#define msg_None  (0)
#define msg_MQTT  (0x100)

#define event_Redraw (0x10000000)
#define event_Quit   (0x10000001)

//#define UIGUI_VERBOSE

// Used as id when creating ui elements and you don't care about the id.
#define id_Any    (0)

class UIGUI
{
	public:
		UIGUI(uint32_t inWidth, uint32_t inHeight, uint32_t inCustomMousePointer = true);
		virtual ~UIGUI();
		void run(UIScreen *inScreen, SDL_Event *event);
		static SDL_Renderer* GetRenderer();
//		SDL_Surface* GetSurface();

		static void DisplayMessage(string message);
		static uint32_t DisplayYesNoMessage(string message);
		static SDL_Texture* RenderText(string inString, string inFontPath, uint32_t inFontSize, SDL_Color &inColor);
		static SDL_Point* getWindowSize();

	protected:
		virtual void Render();

		uint32_t mEventType;
		uint32_t mWidth;
		uint32_t mHeight;
		SDL_Window *mWindow;
		SDL_Renderer *mRenderer;
		SDL_Surface *mSurface;
		SDL_Surface *mMousePointer;
		SDL_Surface *mBackgroundTile;
		SDL_Rect mMousePosition;

		bool mLastMouseButtonPressed;

	public:
		static SDL_Color White;
		static SDL_Color Black;
		static SDL_Color Red;
		static SDL_Color Green;
		static SDL_Color Blue;
		static SDL_Color Yellow;
		static SDL_Color Purple;
		static SDL_Color Cyan;
		static SDL_Color Transparent;

		static SDL_Color TrackingColor;
		static SDL_Color FocusColor;
		static SDL_Color ShadowColor;
		static SDL_Color BackgroundColor;
		static SDL_Color TrackingBackgroundColor;
};


#endif // __UIGUI_H__

