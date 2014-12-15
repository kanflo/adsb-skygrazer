#ifndef __UIIMAGEBUTTON_H__
#define __UIIMAGEBUTTON_H__

#include <UIControl.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UIImageButton : private UIControl
{
	public:
		UIImageButton(uint32_t inId, uint32_t inX, uint32_t inY, string fileName, uint32_t inMessage);
		~UIImageButton();
		void loadImage(string fileName);
		void Render();

		bool MayReceiveFocus();

	protected:
		void scale();

		SDL_Texture *mImageTexture;
};

#endif // __UIIMAGEBUTTON_H__
