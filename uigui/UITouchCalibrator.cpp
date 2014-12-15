#include <UIGUI.h>
#include <UITouchCalibratorControl.h>
#include <UITouchCalibrator.h>
#include <EGalax.h>
#include <UIScreen.h>
#include <UICaption.h>
#include <UIButton.h>
#include <iostream>
#include <stdio.h>

using namespace std;

#define kMsgTap      1
#define kMsgRestart  2


UITouchCalibrator::UITouchCalibrator(UIGUI *ui)
{
	mUI = ui;
	mCalibration.x = mCalibration.y = 
	mCalibration.w = mCalibration.h = 0;
}

UITouchCalibrator::~UITouchCalibrator()
{
}

bool UITouchCalibrator::calibrate(EGalax *eGalax)
{
	bool quit = false;
	bool success = false;
	SDL_Event event;
	UIScreen *screen = new UIScreen(mUI);
	SDL_Point *screenSize = UIGUI::getWindowSize();
	calibrationState_t state = kCenter;
	SDL_Point marker = {screenSize->x/2 , screenSize->y/2};
	SDL_Rect calibration = {0, 0, 0, 0};
	UICaption *caption = new UICaption(id_Any, screenSize->x/2-100,  40, 0, 0, "Tap the center marker.",  "impact", 24, UIGUI::White,  rightAdjust);
	UITouchCalibratorControl *calibCtrl = new UITouchCalibratorControl(id_Any, 0, 0, screenSize->x, screenSize->y, UIGUI::Black, kMsgTap);
	UIButton *button = new UIButton(id_Any, screenSize->x/2-100, screenSize->y-80, 200, 40, event_Quit,   "Cancel", "impact", 24, UIGUI::White);
	calibCtrl->setRedMarker(marker);
	eGalax->startCalibration();

	screen->AddElement((UIElement *) calibCtrl);
	screen->AddElement((UIElement *) caption);
	screen->AddElement((UIElement *) button);

	do {
		mUI->run(screen, &event);
		SDL_Point *p;
		SDL_Point *screenSize = UIGUI::getWindowSize();
		fprintf(stderr, "UITouchCalibrator got command %x\n", event.type);
		if (event.type == screen->getEventType()) {
			SDL_Point raw;
			switch(event.user.code) {
				case event_Quit:
					quit = true;
					break;
				case kMsgTap:
					p = screen->getLastTap();
					printf("Image tapped at (%d, %d)\n", p->x, p->y);
					switch(state) {
						case kCenter:
							caption->setCaption("Tap top left marker.");
							marker.x = 10;
							marker.y = 10;
							state = kTopLeft;
							break;
						case kTopLeft:
							raw = eGalax->getRawCoordinate();
							calibration.x = raw.x;
							calibration.y = raw.y;
							caption->setCaption("Tap bottom right marker.");
							marker.x = screenSize->x-10;
							marker.y = screenSize->y-10;
							state = kBottomRight;
							break;
						case kBottomRight:
							raw = eGalax->getRawCoordinate();
							calibration.w = raw.x;
							calibration.h = raw.y;
							eGalax->setCalibration(calibration);
							caption->setCaption("Calibration done.");
							button->SetCaption("OK");
							marker.x = screenSize->x/2;
							marker.y = screenSize->y/2;
							state = kFinished;
							mCalibration = calibration;
							success = true;
							break;
						default:
							break;
							// Nothing
					}
					if (p) {
						printf("Setting green\n");
						calibCtrl->setGreenMarker(*p);
					}
					calibCtrl->setRedMarker(marker);
					break;
			}
		}
	} while(!quit);

	if (state != kFinished) {
		eGalax->defaultCalibration();
	}
	delete screen;
	UIScreen::setNeedRedraw();
	return success;
}
