#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#include <UIElement.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

using std::string;

class UIControl : protected UIElement
{
	public:
		UIControl(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inMessage);
		virtual void Render();
		~UIControl();

		virtual uint32_t HandleMouseUpEvent(SDL_Event *inEvent);

	protected:
		uint32_t mMessage;
};


#endif // __UICONTROL_H__
