#include <UIGUI.h>
#include <UIElement.h>
#include <UIScreen.h>

UIElement::UIElement(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight)
{
	mId = inId;
	mRect.x = inX;
	mRect.y = inY;
	mRect.h = inHeight;
	mRect.w = inWidth;
	mHasFocus = false;
	mIsTracking = false;
	mRenderer = UIGUI::GetRenderer();

	UIScreen::setNeedRedraw();
}

UIElement::~UIElement()
{
}

#if 0
void UIElement::DrawPixel(int x, int y,Uint8 R, Uint8 G,Uint8 B)
{
    Uint32 color = SDL_MapRGB(mSurface->format, R, G, B);
    Uint8 *  bufp= (Uint8 *)mSurface->pixels + y*mSurface->pitch + x*mSurface->format->BytesPerPixel;
    switch (mSurface->format->BytesPerPixel) {
        case 4:
            bufp[3] = color >> 24;
        case 3:
            bufp[2] = color >> 16;
        case 2:
            bufp[1] = color >> 8;
        case 1:
            bufp[0] = color;
    }
}

void UIElement::DrawPixel(int x, int y, SDL_Color &inColor)
{
    DrawPixel(x, y, inColor.r, inColor.g, inColor.b);
}

void UIElement::DrawPixel(int x, int y, const SDL_Color &inColor)
{
    DrawPixel(x, y, inColor.r, inColor.g, inColor.b);
}
#endif

bool UIElement::WithinBounds(uint32_t inX, uint32_t inY)
{
	return	((int32_t) inX >= mRect.x && (int32_t) inX <= mRect.x+mRect.w &&
			 (int32_t) inY >= mRect.y && (int32_t) inY <= mRect.y+mRect.h);
}

// An element starts tracking when the mouse button is pressed and the mouse is moved.
// When the mouse button is released the tracked element may reveive focus is the mouse
// pointer is withing the element's bounes.
void UIElement::ReceivedTracking()
{
	mIsTracking = true;
	// Notify element that tracking started
	StartedTracking();
	UIScreen::setNeedRedraw();
}

void UIElement::LostTracking()
{
	mIsTracking = false;
	// Notify element that tracking stopped
	StoppedTracking();
	UIScreen::setNeedRedraw();
}

// Return true if the element may receive focus. May be overridden.
bool UIElement::MayReceiveFocus()
{
	return false;
}

// An element receives focus when it is clicked
void UIElement::ReceivedFocus()
{
	mHasFocus = true;
	StartedFocus();
}

void UIElement::LostFocus()
{
	mHasFocus = false;
	StoppedFocus();
}

uint32_t UIElement::GetId()
{
	return mId;
}

// Virtual functions
void UIElement::Render() { }
void UIElement::StartedTracking() { }
void UIElement::StoppedTracking() { }
void UIElement::StartedFocus() { }
void UIElement::StoppedFocus() { }
uint32_t UIElement::HandleMouseUpEvent(SDL_Event *inEvent) { return msg_None; }
uint32_t UIElement::HandleKeyDownEvent(SDL_Event *inEvent) { return msg_None;  }


