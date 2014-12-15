#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include <SDL.h>

class UIElement
{
	friend class UIScreen;

	public:
		UIElement(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight);
		virtual void Render();
		virtual ~UIElement();
#if 0
		void DrawPixel(int x, int y,Uint8 R, Uint8 G,Uint8 B);
		void DrawPixel(int x, int y, SDL_Color &inColor);
		void DrawPixel(int x, int y, SDL_Color const &inColor);
#endif
		bool WithinBounds(uint32_t inX, uint32_t inY);
		// Called when the element receives focus
		void ReceivedFocus();
		// Called when the element looses focus
		void LostFocus();
		// Called when the element is being tracked
		void ReceivedTracking();
		// Called when the element is no longer being tracked
		void LostTracking();

		virtual bool MayReceiveFocus();
		virtual void StartedTracking();
		virtual void StoppedTracking();
		virtual void StartedFocus();
		virtual void StoppedFocus();

		virtual uint32_t HandleMouseUpEvent(SDL_Event *inEvent);
		virtual uint32_t HandleKeyDownEvent(SDL_Event *inEvent);

		uint32_t GetId();

	protected:
		uint32_t mId;
		SDL_Rect mRect;
		SDL_Texture *mTexture;
		SDL_Renderer *mRenderer;
		bool mHasFocus;
		bool mIsTracking;
};

#endif // __UIELEMENT_H__
