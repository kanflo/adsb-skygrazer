#include <UIGUI.h>
#include <UIControl.h>
#include <UIElement.h>
#include <iostream>

#include <stdio.h>

using namespace std;

UIControl::UIControl(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage)
 : UIElement(inId, inX, inY, inWidth, inHeight)
{
	mMessage = inMessage;
}

UIControl::~UIControl()
{
}

void UIControl::Render()
{
}

uint32_t UIControl::HandleMouseUpEvent(SDL_Event *inEvent)
{
	return mMessage;
}
