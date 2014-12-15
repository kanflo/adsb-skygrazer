#ifndef __UITOUCHCALIBRATORCONTROL_H__
#define __UITOUCHCALIBRATORCONTROL_H__

#include <UICanvasControl.h>

class UITouchCalibratorControl : public UICanvasControl
{
	public:
		UITouchCalibratorControl(uint32_t inId, uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, SDL_Color &bgColor, uint32_t inMessage);
		~UITouchCalibratorControl();
		void Render();
		void setRedMarker(SDL_Point &marker);
		void setGreenMarker(SDL_Point &marker);

	private:
		SDL_Point mRedMarker;
		SDL_Point mGreenMarker;
};

#endif // __UITOUCHCALIBRATORCONTROL_H__
