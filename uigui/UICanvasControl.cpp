#include <UIGUI.h>
#include <UICanvasControl.h>
#include <iostream>
#include <stdio.h>

using namespace std;

UICanvasControl::UICanvasControl(int inId, int inX, int inY, int inWidth, int inHeight, SDL_Color &bgColor, int inMessage)
 : UIControl(inId, inX, inY, inWidth, inHeight, inMessage)
{
	mBgColor = bgColor;
}

UICanvasControl::~UICanvasControl()
{
}

void UICanvasControl::Render()
{
	SDL_SetRenderDrawColor(mRenderer, mBgColor.r, mBgColor.g, mBgColor.b, mBgColor.a);
	SDL_RenderFillRect(mRenderer, &mRect);
}

bool UICanvasControl::MayReceiveFocus()
{
	return false;
}
