#ifndef __UICANVASCONTROL_H__
#define __UICANVASCONTROL_H__

#include <UIControl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UICanvasControl : public UIControl
{
	public:
		UICanvasControl(int inId, int inX, int inY, int inWidth, int inHeight, SDL_Color &bgColor, int inMessage);
		~UICanvasControl();
		void Render();

		bool MayReceiveFocus();

	protected:
		SDL_Color mBgColor;
};

#endif // __UICANVASCONTROL_H__
