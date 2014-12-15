#ifndef __UITOUCHCALIBRATOR_H__
#define __UITOUCHCALIBRATOR_H__

#include <UIGUI.h>
#include <EGalax.h>
#include <string>

//using std::string;

enum calibrationState_t {
	kCenter = 0,
	kTopLeft,
	kBottomRight,
	kFinished
};

class UITouchCalibrator
{
	public:
		UITouchCalibrator(UIGUI *ui);
		~UITouchCalibrator();
		bool calibrate(EGalax *eGalax);
		SDL_Rect *getCalibration() { return &mCalibration; };

	protected:
		UIGUI *mUI;
		SDL_Rect mCalibration;
};

#endif // __UITOUCHCALIBRATOR_H__
