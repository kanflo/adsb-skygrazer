#ifndef __UICAPTION_H__
#define __UICAPTION_H__

#include <UIElement.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UICaption : protected UIElement
{
	public:
		UICaption(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor, uiguiTextAdjust inAdjust);
		void Render();
		~UICaption();
		void setCaption(string inNewCaption);

		bool MayReceiveFocus();
		void StartedTracking();
		void StoppedTracking();
		void StartedFocus();
		void StoppedFocus();

	protected:
		void RenderText();

		string mString;
		SDL_Texture *mTextTexture;
		SDL_Texture *mTextShadowTexture;
		string mFontPath;
		uint32_t mFontSize;
		uiguiTextAdjust mAdjust;
		SDL_Color mColor;
};

#endif // __UICAPTION_H__
