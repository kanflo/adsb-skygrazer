#ifndef __UITEXTFIELD_H__
#define __UITEXTFIELD_H__

#include <UITextField.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UITextField : private UIControl
{
	public:
		UITextField(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor);
		void Render();
		~UITextField();
		void SetCaption(string inNewCaption);
		string GetCaption();

		bool MayReceiveFocus();
		void StartedTracking();
		void StoppedTracking();
		void StartedFocus();
		void StoppedFocus();

		uint32_t HandleKeyDownEvent(SDL_Event *inEvent);
		uint32_t HandleMouseUpEvent(SDL_Event *inEvent);

	protected:
		void RenderText();

		string mString;
		SDL_Surface *mTextSurface;
		SDL_Surface *mTextShadowSurface;
		string mFontPath;
		uint32_t mFontSize;
		SDL_Color mColor;
		uint32_t mLastTicks;
		uint32_t mCaretToggle;
};
#endif // __UITEXTFIELD_H__
