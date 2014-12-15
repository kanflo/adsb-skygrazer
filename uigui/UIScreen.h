#ifndef __UISCREEN_H__
#define __UISCREEN_H__

#include <UIGUI.h>
#include <UIElement.h>
#include <list>
#include <SDL.h>

using namespace std;

class UIScreen
{
	public:
		UIScreen(UIGUI *inUIGUI);
		~UIScreen();
		SDL_Texture *GetTexture();
		SDL_Renderer* GetRenderer();
		void AddElement(UIElement *inNewElement);
		void Open();
		void Render();
		void Run();
		void Close();
		virtual bool handleEvent(SDL_Event *inEvent);
		UIElement* FindElementAt(int inX, int inY);
		UIElement* FindElementId(int inId);
		void SetFocus(UIElement *inNewFocus);
		SDL_Point* getLastTap();
		void setBackgroundColor(SDL_Color &color);

		static SDL_DisplayMode* getDisplayMode();
		static bool needsRedraw();
		static void setNeedRedraw(bool redraw = true);

		uint32_t getEventType() { return mEventType; };

	private:
		uint32_t mEventType;
		SDL_Renderer *mRenderer;
		SDL_Texture *mTexture;
		UIElement   *mTracking;
		UIElement   *mFocus;
		list<UIElement*> mElements;
		SDL_Point mLastClick;
		SDL_Color mBgColor;
};


#endif // __UISCREEN_H__
