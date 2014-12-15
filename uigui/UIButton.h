#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#include <UIElement.h>
#include <UIControl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UIButton : private UIControl
{
	public:
		UIButton(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor);
		void Render();
		~UIButton();
		void SetCaption(string inNewCaption);

		bool MayReceiveFocus();
		void StartedTracking();
		void StoppedTracking();

	protected:
		void RenderText();

		string mString;
		SDL_Texture *mTextTexture;
		SDL_Texture *mTextShadowTexture;
		string mFontPath;
		uint32_t mFontSize;
		SDL_Color mColor;
};

#endif // __UIBUTTON_H__
