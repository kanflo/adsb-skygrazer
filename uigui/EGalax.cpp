#include <UIGUI.h>
#include <EGalax.h>
#include <libusb.h>

static struct libusb_device_handle *dh;

#define VID (0x0eef)
#define PID (0x0001)

// Values from the 8" eGalax. Your milage may vary.
#define DEFAULT_EGALAX_LEFT     100
#define DEFAULT_EGALAX_TOP      290
#define DEFAULT_EGALAX_RIGHT   3696
#define DEFAULT_EGALAX_BOTTOM  3621

SDL_Point *gScreenSize;
SDL_Rect gCalibration;
SDL_Point gLastRawCoordinate;
bool gIsCalibrating;
bool gIsExiting;

// Convert eGalax x and y coordinates to screen coordinates according to the
// current calibration.
static int32_t eGalaxCalibratedX(int32_t x)
{
//	printf("x:%.3f (%d) ", 100*((float) (x-gCalibration.x) / (float) (gCalibration.w-gCalibration.x)), x);
	return gScreenSize->x * ((float) (x-gCalibration.x) / (float) (gCalibration.w-gCalibration.x));
}

static int32_t eGalaxCalibratedY(int32_t y)
{
//	printf("y:%.3f (%d)\n", 100*((float) (y-gCalibration.y) / (float) (gCalibration.h-gCalibration.y)), y);
	return gScreenSize->y * ((float) (y-gCalibration.y) / (float) (gCalibration.h-gCalibration.y));
}

// Handle tap and release
static void handleTap(uint32_t x, uint32_t y, bool isPressed)
{
	SDL_MouseButtonEvent event;
	event.type = isPressed ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
	event.timestamp = 0;
	event.windowID = 0;
	event.which = 0;
	event.button = 0;
	event.state = isPressed ? SDL_PRESSED : SDL_RELEASED;
	event.clicks = 1;
	if (gIsCalibrating) {
		event.x = event.y = 0;
	} else {
		event.x = eGalaxCalibratedX(x);
		event.y = eGalaxCalibratedY(y);
	}
	SDL_PushEvent((SDL_Event*) &event);
}

static void handleTrack(uint32_t x, uint32_t y, int32_t xrel, int32_t yrel)
{
	SDL_MouseMotionEvent event;
	event.type = SDL_MOUSEMOTION;
	event.timestamp = 0;
	event.windowID = 0;
	event.which = 0;
	event.state = SDL_PRESSED;
	if (gIsCalibrating) {
		event.x = event.y = 0;
	} else {
		event.x = eGalaxCalibratedX(x);
		event.y = eGalaxCalibratedY(y);
	}
	event.xrel = xrel;
	event.yrel = yrel;
	SDL_PushEvent((SDL_Event*) &event);
}

// This is where it happens. Listen to the eGalax interrupt endpoint and decode
// the data arriving. Keep track of tap, drag and release and push SDL events
// accordingly. If we're in calibration mode the coordinate (0, 0) will be send.
static int eGalaxThread(void *ptr)
{
	int32_t r, len;
	uint8_t data[8];
	uint32_t x = 0;
	uint32_t y = 0;
	uint32_t lastX = 0;
	uint32_t lastY = 0;
	bool isTracking = false;
	do {
		r = libusb_interrupt_transfer(dh, 0x81, data, sizeof(data), &len, 50);
		if(LIBUSB_ERROR_TIMEOUT == r) {
			if (isTracking) {
				// This usually means that the user lifted the finger and we
				// missed the USB transfer saying so. Not to worry, we simply
				// ignore that and claim the user having lifted the funger.
				handleTap(x, y, false);
				isTracking = false;
			}
			r = 0;
			continue;
		} else if(r) {
			goto release;
		}
		if(len == 5 && data[0] & 0x80) {
			// Proper eGalax datagram
			uint8_t tapped = data[0] & 1;
			x = data[1] << 8 | data[2];
			y = data[3] << 8 | data[4];
			// Note. The top left corner of the eGalax is really the top left
			// coordinate. If your x and y axis are inverted you probalby
			// connected the converter board wrong. Flip the cable around and
			// try again.
			gLastRawCoordinate.x = x;
			gLastRawCoordinate.y = y;
			if (tapped && !isTracking) {
				handleTap(x, y, true);
				isTracking = true;
			} else if (!tapped && isTracking) {
				handleTap(x, y, false);
				isTracking = false;
			} else {
				handleTrack(x, y, x-lastX, y-lastY);
			}
			lastX = x;
			lastY = y;
			continue;
		}
	} while(!r && !gIsExiting);

release:
	libusb_release_interface(dh, 0);

	if(dh) {
		libusb_close(dh);		
		dh = NULL;
	}
	libusb_exit(NULL);

	printf("eGalax thread is exiting\n");
	return 0;
}

EGalax::EGalax()
{
	mIsCalibrating = false;
	gIsCalibrating = mIsCalibrating;
	gScreenSize = UIGUI::getWindowSize();
	gCalibration.x = 
	gCalibration.y = 
	gCalibration.w = 
	gCalibration.h = 0;
	gIsExiting = false;
}

EGalax::~EGalax()
{
	gIsExiting = true;
}

// Start calibration. From now on the driver will return (0, 0) as tap
// coordinate until calibration is completed.
void EGalax::startCalibration()
{
	printf("eGalax starting calibration\n");
	mIsCalibrating = true;
	gIsCalibrating = mIsCalibrating;
}

void EGalax::defaultCalibration()
{
	printf("eGalax default calibration\n");
	mIsCalibrating = false;
	gIsCalibrating = mIsCalibrating;
	SDL_Rect defaultGalaxCalib = {DEFAULT_EGALAX_LEFT, DEFAULT_EGALAX_TOP, DEFAULT_EGALAX_RIGHT, DEFAULT_EGALAX_BOTTOM};
	gCalibration = defaultGalaxCalib;
}

SDL_Point EGalax::getRawCoordinate()
{
	return gLastRawCoordinate;
}

void EGalax::setCalibration(SDL_Rect &calibration)
{
	gCalibration = calibration;
	printf("eGalax calibrated to (%d, %d), (%d, %d)\n", gCalibration.x, gCalibration.y, gCalibration.w , gCalibration.h);
	mIsCalibrating = false;
	gIsCalibrating = mIsCalibrating;
}

bool EGalax::connect()
{
	int32_t ret;
	if((ret = libusb_init(NULL)) < 0) {
		return false;
	}

	if(NULL == (dh = libusb_open_device_with_vid_pid(NULL, VID, PID))) {
		fprintf(stderr,"Can't open device\n");
		if(dh) {
			libusb_close(dh);		
			dh = NULL;
		}
		return false;
	}

	if(libusb_claim_interface(dh, 4)<0 && libusb_claim_interface(dh, 0)<0) {
		// Interface 0 in Virtual Box
		fprintf(stderr, "Can't claim interface\n");
		if(dh) {
			libusb_close(dh);
			dh = NULL;
		}
		return false;
	}

	printf("Connected to eGalax\n");
	mThread = SDL_CreateThread(eGalaxThread, "eGalaxThread", (void *)NULL);
	return true;
}
