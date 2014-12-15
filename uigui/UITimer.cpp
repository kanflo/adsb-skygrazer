#include <UIGUI.h>
#include <UITimer.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <stdio.h>

using namespace std;

UITimer::UITimer(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, string inCaption, string inFontName, uint32_t inFontSize, SDL_Color &inColor, uint32_t inAdjust, uint32_t inTimeSec, uint32_t inExpireMsg)
 : UICaption(inId, inX, inY, inWidth, inHeight, inCaption, inFontName, inFontSize, inColor, inAdjust)
{
	mLastTimeStamp = time(0);
	mTimeSec = inTimeSec;
	mExpireMsg = inExpireMsg;
	mIsRunning = false;
#if 0
	mTextSurface = NULL;
	mTextShadowSurface = NULL;
#endif
	RenderText();
}

UITimer::~UITimer()
{
#if 0
	if (mTextSurface)
		SDL_FreeSurface(mTextSurface);
	if (mTextShadowSurface)
		SDL_FreeSurface(mTextShadowSurface);
#endif
}


void UITimer::SetTime(uint32_t inTimeSec)
{
	mTimeSec = inTimeSec;
}

uint32_t UITimer::GetTime()
{
	return mTimeSec;
}

void UITimer::Start()
{
	mIsRunning = true;
	mLastTimeStamp = time(0);
	RenderText();
	PassingTime();
}

void UITimer::Stop()
{
	mIsRunning = false;
}

void UITimer::PassingTime()
{
	uint32_t current = time(0);

	if (mIsRunning && current != mLastTimeStamp)
	{
		mTimeSec -= current - mLastTimeStamp;
		if (mTimeSec < 0)
		{
			mTimeSec = 0;
		}
		mLastTimeStamp = current;
		RenderText();
		if (mTimeSec == 0)
		{
			SDL_Event event;
			event.type = SDL_USEREVENT;
			event.user.code = mExpireMsg;
			event.user.data1 = 0;
			event.user.data2 = 0;
			SDL_PushEvent(&event);
		}
	}
}

void UITimer::RenderText()
{
	ostringstream timerString;
	uint32_t min, sec;

	if (mTextSurface)
	{
		SDL_FreeSurface(mTextSurface);
	}
	if (mTextShadowSurface)
	{
		SDL_FreeSurface(mTextShadowSurface);
	}

	min = mTimeSec / 60;
	sec = mTimeSec % 60;

	if (min < 10)
	{
		timerString << "0";
	}
	timerString << min << ":";
	if (sec < 10)
	{
		timerString << "0";
	}
	timerString << sec;

//	cout << "Timer says " << timerString.str() << "\n";

	mTextSurface = UIGUI::RenderText(timerString.str(), mFontPath, mFontSize, mTimeSec <= 30 ? mTimeSec&1 ? mColor : UIGUI::Red : mColor);
	mTextShadowSurface = UIGUI::RenderText(timerString.str(), mFontPath, mFontSize, UIGUI::ShadowColor);
    if (mRect.w < mTextSurface->w)
    {
    	mRect.w = mTextSurface->w;
    }
    if (mRect.h < mTextSurface->h)
    {
    	mRect.h = mTextSurface->h;
    }
}

void UITimer::Render()
{
	SDL_Rect rect = mRect;

	PassingTime();
	switch(mAdjust)
	{
		case leftAdjust:
			break;
		case centerAdjust:
			rect.x += (rect.w - mTextSurface->w)/2;
			break;
		case rightAdjust:
			rect.x += (rect.w - mTextSurface->w);
			break;
	}

	SDL_Rect shadowRect = rect;
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
	SDL_BlitSurface(mTextSurface, NULL, mSurface, &rect);
}
