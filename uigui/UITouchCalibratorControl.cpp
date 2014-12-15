#include <UIGUI.h>
#include <UIScreen.h>
#include <UITouchCalibratorControl.h>
#include <iostream>
#include <stdio.h>

using namespace std;

UITouchCalibratorControl::UITouchCalibratorControl(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, SDL_Color &bgColor, uint32_t inMessage)
 : UICanvasControl(inId, inX, inY, inWidth, inHeight, bgColor, inMessage)
{
	mRedMarker.x = mRedMarker.y =
	mGreenMarker.x = mGreenMarker.y = 0;
}

UITouchCalibratorControl::~UITouchCalibratorControl()
{
}

void UITouchCalibratorControl::Render()
{
	UICanvasControl::Render();

	if (mRedMarker.x && mRedMarker.y) {
		SDL_SetRenderDrawColor(UIGUI::GetRenderer(), 0xff, 0, 0, 0xff);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x-15, mRedMarker.y-1, mRedMarker.x+15, mRedMarker.y-1);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x-15, mRedMarker.y, mRedMarker.x+15, mRedMarker.y);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x-15, mRedMarker.y+1, mRedMarker.x+15, mRedMarker.y+1);

		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x-1, mRedMarker.y-15, mRedMarker.x-1, mRedMarker.y+15);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x, mRedMarker.y-15, mRedMarker.x, mRedMarker.y+15);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mRedMarker.x+1, mRedMarker.y-15, mRedMarker.x+1, mRedMarker.y+15);
	}

	if (mGreenMarker.x && mGreenMarker.y) {
		SDL_SetRenderDrawColor(UIGUI::GetRenderer(), 0, 0xff, 0, 0xff);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x-15, mGreenMarker.y-1, mGreenMarker.x+15, mGreenMarker.y-1);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x-15, mGreenMarker.y, mGreenMarker.x+15, mGreenMarker.y);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x-15, mGreenMarker.y+1, mGreenMarker.x+15, mGreenMarker.y+1);

		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x-1, mGreenMarker.y-15, mGreenMarker.x-1, mGreenMarker.y+15);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x, mGreenMarker.y-15, mGreenMarker.x, mGreenMarker.y+15);
		SDL_RenderDrawLine(UIGUI::GetRenderer(), mGreenMarker.x+1, mGreenMarker.y-15, mGreenMarker.x+1, mGreenMarker.y+15);
	}
}

void UITouchCalibratorControl::setRedMarker(SDL_Point &marker)
{
	mRedMarker = marker;
	UIScreen::setNeedRedraw();
}

void UITouchCalibratorControl::setGreenMarker(SDL_Point &marker)
{
	mGreenMarker = marker;
	UIScreen::setNeedRedraw();
}
