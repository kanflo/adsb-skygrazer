#include <UIGUI.h>
#include <UITextField.h>
#include <iostream>
#include <stdio.h>

using namespace std;

UITextField::UITextField(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor)
 : UIControl(inId, inX, inY, inWidth, inHeight, inMessage)
{
	string fontSuffix = ".ttf";
	mString = inCaption;
	mTextSurface = NULL;
	mTextShadowSurface = NULL;
	mFontSize = inFontSize;
	mColor = inColor;
	mFontPath = "c:\\WINDOWS\\Fonts\\";
	mFontPath += inFontName;
	mFontPath += fontSuffix;
	RenderText();
	mLastTicks = SDL_GetTicks();
	mCaretToggle = 0;
}

UITextField::~UITextField()
{
	SDL_FreeSurface(mTextSurface);
	SDL_FreeSurface(mTextShadowSurface);
}

void UITextField::RenderText()
{
	TTF_Font *font;

	if (mTextSurface)
	{
		SDL_FreeSurface(mTextSurface);
		SDL_FreeSurface(mTextShadowSurface);
		mTextSurface = NULL;
		mTextShadowSurface = NULL;
	}

	if (mString.length() == 0)
	{
		return;
	}

	font = TTF_OpenFont(mFontPath.c_str(), mFontSize);
	if (!font)
	{
		cerr << "Count not load font " << mFontPath.c_str() << " at size " << mFontSize;
		exit(2);
	}

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	mTextSurface = TTF_RenderText_Blended(font, mString.c_str(), mIsTracking ? UIGUI::TrackingColor : mHasFocus ? UIGUI::FocusColor : mColor);
	mTextShadowSurface = TTF_RenderText_Blended(font, mString.c_str(), UIGUI::ShadowColor);

    TTF_CloseFont(font);
#if 0
    if (mRect.w < mTextSurface->w)
    {
    	mRect.w = mTextSurface->w;
    }
    if (mRect.h < mTextSurface->h)
    {
    	mRect.h = mTextSurface->h;
    }
#endif
}

void UITextField::Render()
{
	SDL_Rect clipRect = mRect;
	uint32_t x, y, h, w;

	clipRect.x += 4;
	clipRect.w -= 8;

	SDL_Color *lineColor = &(mIsTracking ? UIGUI::TrackingColor : mHasFocus ? UIGUI::FocusColor : mColor);

	if (SDL_GetTicks() - mLastTicks > 500)
	{
		mCaretToggle = mCaretToggle ? 0 : 1;
		mLastTicks = SDL_GetTicks();
	}

	if (mHasFocus) // Draw the caret
	{
		if (mTextSurface)
		{
			h = mTextSurface->h;
			w = mTextSurface->w;
		}
		else
		{
			h = mRect.h;
			w = 2;
		}

		if (mCaretToggle && mHasFocus)
		{
			for(uint32_t y=4; y<h-3; y++)
			{
				if (mRect.x+w + 4 < clipRect.x + clipRect.w)
				{
					DrawPixel(mRect.x+w + 5,  mRect.y+y, *lineColor);
					DrawPixel(mRect.x+w + 6, mRect.y+y, *lineColor);
				}
			}
		}
	}

	// Draw the border
	for(x=0; x<mRect.w; x++)
	{
		DrawPixel(mRect.x+x, mRect.y, *lineColor);
		DrawPixel(mRect.x+x, mRect.y+1, *lineColor);
		DrawPixel(mRect.x+x, mRect.y+mRect.h-1, *lineColor);
		DrawPixel(mRect.x+x, mRect.y+mRect.h, *lineColor);
	}

	for(y=0; y<mRect.h; y++)
	{
		DrawPixel(mRect.x, mRect.y+y, *lineColor);
		DrawPixel(mRect.x+1, mRect.y+y, *lineColor);
		DrawPixel(mRect.x+mRect.w-1, mRect.y+y, *lineColor);
		DrawPixel(mRect.x+mRect.w, mRect.y+y, *lineColor);
	}

	if (mTextSurface != NULL)
	{
		SDL_SetClipRect(mSurface, &clipRect);


		SDL_Rect shadowRect = clipRect;
		if (mColor.r != UIGUI::ShadowColor.r || mColor.g != UIGUI::ShadowColor.g || mColor.b != UIGUI::ShadowColor.b)
		{
			if (mFontSize > 25)
			{
				shadowRect.x += 2;
				shadowRect.y += 2;
			}
			else
			{
				shadowRect.x++;
				shadowRect.y++;
			}
			SDL_BlitSurface(mTextShadowSurface, NULL, mSurface, &shadowRect);
		}

		SDL_BlitSurface(mTextSurface, NULL, mSurface, &clipRect);


		SDL_SetClipRect(mSurface, NULL);
	}
}

void UITextField::SetCaption(string inNewCaption)
{
	mString = inNewCaption;
	RenderText();
}

string UITextField::GetCaption()
{
	return mString;
}

bool UITextField::MayReceiveFocus()
{
	return true;
}

void UITextField::StartedTracking()
{
	RenderText();
}
void UITextField::StoppedTracking()
{
	RenderText();
}

void UITextField::StartedFocus()
{
	RenderText();
}
void UITextField::StoppedFocus()
{
	RenderText();
}

uint32_t UITextField::HandleKeyDownEvent(SDL_Event *inEvent)
{
	if (inEvent->type == SDL_KEYDOWN)
	{
		switch (inEvent->key.keysym.sym) {
			case SDLK_RETURN:
				return mMessage;
				break;
			case SDLK_BACKSPACE:
			case SDLK_DELETE:
				if (mString.length() > 1)
				{
					mString = mString.substr(0, mString.length()-1);
				}
				else
				{
					mString.assign("");
				}
				break;
			// Keys we do not want the text field to listen to
			case SDLK_ESCAPE:
				return msg_None;
				break;

			default:
				break;
				// Accept input

		}
#if 0
		if (inEvent->key.keysym.unicode != SDLK_BACKSPACE && inEvent->key.keysym.unicode != 0)
		{
			char temp[2] = { (char) inEvent->key.keysym.unicode & 0xff, 0};
			mString.append(temp);
		}
#endif
	}

	RenderText();
	return msg_None;
}

// We do not want the text field to send its messege simply by getting clicked-
uint32_t UITextField::HandleMouseUpEvent(SDL_Event *inEvent)
{
	return msg_None;
}
