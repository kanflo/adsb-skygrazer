#include <UIGUI.h>
#include <UIButton.h>
#include <iostream>
#include <stdio.h>

using namespace std;

UIButton::UIButton(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor)
 : UIControl(inId, inX, inY, inWidth, inHeight, inMessage)
{
	string fontSuffix = ".ttf";
	mString = inCaption;
	mTextTexture = NULL;
	mTextShadowTexture = NULL;
	mFontSize = inFontSize;
	mColor = inColor;
	mFontPath = "c:\\WINDOWS\\Fonts\\";
	mFontPath += inFontName;
	mFontPath += fontSuffix;

	RenderText();
}

UIButton::~UIButton()
{
	SDL_DestroyTexture(mTextTexture);
	SDL_DestroyTexture(mTextShadowTexture);
}

void UIButton::RenderText()
{
	mTextTexture = UIGUI::RenderText(mString, mFontPath, mFontSize, mIsTracking ? UIGUI::TrackingColor : mHasFocus ? UIGUI::FocusColor : mColor);
//	mTextShadowTexture = TTF_RenderText_Blended(font, mString.c_str(), UIGUI::ShadowColor);
}

void UIButton::Render()
{
	SDL_Rect rect = mRect;
	int32_t width, height;
	SDL_QueryTexture(mTextTexture, NULL, NULL, &width, &height);
	SDL_Color *drawColor = (SDL_Color*) &(mIsTracking ? UIGUI::TrackingColor : mHasFocus ? UIGUI::FocusColor : mColor);
	SDL_Color *fillColor = (SDL_Color*) &(mIsTracking ? UIGUI::TrackingBackgroundColor : UIGUI::BackgroundColor);

	SDL_SetRenderDrawColor(mRenderer, fillColor->r, fillColor->g, fillColor->b, fillColor->a);
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(mRenderer, &mRect);

	SDL_SetRenderDrawColor(mRenderer, drawColor->r, drawColor->g, drawColor->b, drawColor->a);
	SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_NONE);
	SDL_RenderDrawRect(mRenderer, &mRect);

	// Center text
	rect.x += (rect.w - width)/2;
	rect.y += (rect.h - height)/2;
	rect.w = width;
	rect.h = height;

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

void UIButton::SetCaption(string inNewCaption)
{
	mString = inNewCaption;
	RenderText();
}

bool UIButton::MayReceiveFocus()
{
	return false;
}

void UIButton::StartedTracking()
{
	RenderText();
}

void UIButton::StoppedTracking()
{
	RenderText();
}
