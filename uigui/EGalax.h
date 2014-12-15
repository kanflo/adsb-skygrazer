#ifndef __EGALAX_H__
#define __EGALAX_H__

#include <SDL.h>

class EGalax
{
	public:
		EGalax();
		virtual ~EGalax();
		bool connect();
		void defaultCalibration();
		void startCalibration();
		SDL_Point getRawCoordinate();
		void setCalibration(SDL_Rect &calibration);

	protected:
	 	SDL_Thread *mThread;
		bool mIsCalibrating;
};

#endif // __EGALAX_H__
