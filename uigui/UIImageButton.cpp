#include <UIGUI.h>
#include <UIImageButton.h>
#include <UIScreen.h>
#include <iostream>
#include <stdio.h>

using namespace std;

UIImageButton::UIImageButton(uint32_t inId, uint32_t inX, uint32_t inY, string fileName, uint32_t inMessage)
 : UIControl(inId, inX, inY, 0, 0, inMessage)
{
	this->loadImage(fileName);
}

UIImageButton::~UIImageButton()
{
	if (mImageTexture) {
		SDL_DestroyTexture(mImageTexture);
	}
}

void UIImageButton::Render()
{
	SDL_RenderCopy(mRenderer, mImageTexture, NULL, &mRect);
}

bool UIImageButton::MayReceiveFocus()
{
	return false;
}

void UIImageButton::loadImage(string fileName)
{
	if (mImageTexture) {
		SDL_DestroyTexture(mImageTexture);
	}
	SDL_Surface *temp;
	temp = IMG_Load(fileName.c_str());
	if (!temp) {
		printf("UIImageButton: Failed to load %s\n", fileName.c_str());
		printf("IMG Error: %s / %s\n", IMG_GetError(), SDL_GetError());
		assert(0);
	}
	mImageTexture = SDL_CreateTextureFromSurface(mRenderer, temp);
	SDL_FreeSurface(temp);
	this->scale();
	UIScreen::setNeedRedraw();
}

// Scale image according to display, if needed
void UIImageButton::scale()
{
	int32_t width, height;
	SDL_Point *winSize = UIGUI::getWindowSize();
	SDL_QueryTexture(mImageTexture, NULL, NULL, &width, &height);
	if (width > winSize->x) {
		mRect.w = winSize->x;
		mRect.h = winSize->x * ((float) height / (float) width);
	} else {
		mRect.w = width;
		mRect.h = height;
	}
}
