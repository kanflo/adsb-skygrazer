#include <UIGUI.h>
#include <UIScreen.h>

#include <UICaption.h>
#include <iostream>
//#include <stdtypes.h>

#include <stdio.h>

using namespace std;

UICaption::UICaption(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor, uiguiTextAdjust inAdjust)
 : UIElement(inId, inX, inY, inWidth, inHeight)
{
	string fontSuffix = ".ttf";
	mString = inCaption;
	mTextTexture = NULL;
	mFontSize = inFontSize;
	mColor = inColor;
	mFontPath = "c:\\WINDOWS\\Fonts\\";
	mFontPath += inFontName;
	mFontPath += fontSuffix;
	mAdjust = inAdjust;
	mTextTexture = NULL;
	mTextShadowTexture = NULL;
	RenderText();
}

UICaption::~UICaption()
{
	if (mTextTexture) SDL_DestroyTexture(mTextTexture);
	if (mTextShadowTexture) SDL_DestroyTexture(mTextShadowTexture);
}

void UICaption::RenderText()
{
	int32_t width, height;
	if (mTextTexture) {
		SDL_DestroyTexture(mTextTexture);
	}
	if (mTextShadowTexture) {
		SDL_DestroyTexture(mTextShadowTexture);
	}

	mTextTexture = UIGUI::RenderText(mString, mFontPath, mFontSize, mIsTracking ? UIGUI::TrackingColor : mHasFocus ? UIGUI::FocusColor : mColor);
	mTextShadowTexture = UIGUI::RenderText(mString, mFontPath, mFontSize, UIGUI::ShadowColor);
	if (mTextTexture) {
		SDL_QueryTexture(mTextTexture, NULL, NULL, &width, &height);
	   	mRect.w = width;
	   	mRect.h = height;
	}
	UIScreen::setNeedRedraw();
}

void UICaption::Render()
{
	SDL_Rect rect = mRect;

	if (!mTextTexture) {
		return;
	}

	switch(mAdjust) {
		case leftAdjust:
			break;
		case centerAdjust:
			rect.x -= (rect.w)/2;
			break;
		case rightAdjust:
			rect.x -= rect.w;
			break;
	}

	SDL_Rect shadowRect = rect;
	if (mColor.r != UIGUI::ShadowColor.r || mColor.g != UIGUI::ShadowColor.g || mColor.b != UIGUI::ShadowColor.b) {
		if (mFontSize > 25) {
			shadowRect.x += 2;
			shadowRect.y += 2;
		} else {
			shadowRect.x++;
			shadowRect.y++;
		}
		SDL_RenderCopy(mRenderer, mTextShadowTexture, NULL, &shadowRect);
	}
	SDL_RenderCopy(mRenderer, mTextTexture, NULL, &rect);
}

void UICaption::setCaption(string inNewCaption)
{
	mString = inNewCaption;
	RenderText();
}

bool UICaption::MayReceiveFocus()
{
	return false;
}

void UICaption::StartedTracking() {}
void UICaption::StoppedTracking() {}
void UICaption::StartedFocus() {}
void UICaption::StoppedFocus() {}
