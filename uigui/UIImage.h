#ifndef __UIIMAGE_H__
#define __UIIMAGE_H__

#include <UIElement.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UIImage : private UIElement
{
	public:
		UIImage(uint32_t inId, uint32_t inX, uint32_t inY, string fileName);
		UIImage(uint32_t inId, uint32_t inX, uint32_t inY, SDL_Texture *inImage);
		~UIImage();
		void loadImage(string fileName);
		void Render();

	protected:
		void scale();

		SDL_Texture *mImageTexture;
};

#endif // __UIIMAGE_H__
