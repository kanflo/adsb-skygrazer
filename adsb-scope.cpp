#include <stdio.h>
#include <string>
#include <time.h>
#include <jansson.h>
#include "INIReader.h"

#include <UIGUI.h>
#include <UIScreen.h>
#include <UIElement.h>
#include <UICaption.h>
#include <UIButton.h>
#include <UITextField.h>
#include <UIImage.h>
#include <UIImageButton.h>
#include <UITimer.h>
#include <UIMenu.h>
#include <UICanvasControl.h>
#include <UITouchCalibrator.h>
#include <UITouchCalibratorControl.h>
#include <EGalax.h>
#include <MQTTScreen.h>
#include "CURLDownloader.h"


enum screenCommands {
	kCalibrate = 1,
	kBackgroundTap,
	kMQTTMessage,
	kCancel,
	kQuit
};


#define SCREEN_WIDTH     1024
#define SCREEN_HEIGHT     768

#define EGALAX_DEFAULT_LEFT     100
#define EGALAX_DEFAULT_TOP      290
#define EGALAX_DEFAULT_RIGHT   3696
#define EGALAX_DEFAULT_BOTTOM  3621

#define MQTT_DEFAULT_SERVER  "gateway.local"
#define MQTT_DEFAULT_PORT    (1883)

string gMQTTServer = "";
uint16_t gMQTTPort = 0;

string gDownloadPath = "";

bool gEGalaxEnabled = false;
SDL_Rect eGalaxCalib;

UICaption *gAircraftType;
UICaption *gRoute;
UICaption *gDistanceBearing;
UICaption *gAltitudeSpeed;
UICaption *gHeading;

/*
static bool readTouchConfig(SDL_Rect &eGalaxCalib)
{
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int lineNumber = 1;

	fp = fopen("/etc/egalax.conf", "r");
	if (fp == NULL) {
		return false;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		printf("Retrieved line %d of length %zu :\n", lineNumber, read);
		printf("%s", line);
		switch(lineNumber) {
			case 1:
				eGalaxCalib.x = atoi(line);
				break;
			case 2:
				eGalaxCalib.y = atoi(line);
				break;
			case 3:
				eGalaxCalib.w = atoi(line);
				break;
			case 4:
				eGalaxCalib.h = atoi(line);
				break;
		}
		lineNumber++;
	}

	fclose(fp);
	if (line) {
		free(line);
	}

	return true;
}

static bool writeTouchConfig(SDL_Rect &eGalaxCalib)
{
	FILE * fp;
	char line[32];

	fp = fopen("/etc/egalax.conf", "w+");
	if (fp == NULL) {
		return false;
	}
	
	sprintf((char*) line, "%d\n", eGalaxCalib.x);
   
	fwrite(line, 1, strlen(line), fp);
	sprintf((char*) line, "%d\n", eGalaxCalib.y);
	fwrite(line, 1, strlen(line), fp);
	sprintf((char*) line, "%d\n", eGalaxCalib.w);
	fwrite(line, 1, strlen(line), fp);
	sprintf((char*) line, "%d\n", eGalaxCalib.h);
	fwrite(line, 1, strlen(line), fp);
	
	fclose(fp);

	return true;
}
*/

static bool readIniFile()
{
	INIReader reader("/etc/adsb-scope.ini");

	if (reader.ParseError() < 0) {
//		std::cout << "Can't load '/etc/adsb-scope.ini'\n";
		return false;
	}

	gMQTTServer = reader.Get("mqtt", "host", MQTT_DEFAULT_SERVER);
	gMQTTPort = reader.GetInteger("mqtt", "port", MQTT_DEFAULT_PORT);

	eGalaxCalib.x = reader.GetInteger("egalax", "left", EGALAX_DEFAULT_LEFT);
	eGalaxCalib.y = reader.GetInteger("egalax", "top", EGALAX_DEFAULT_TOP);
	eGalaxCalib.w = reader.GetInteger("egalax", "right", EGALAX_DEFAULT_RIGHT);
	eGalaxCalib.h = reader.GetInteger("egalax", "bottom", EGALAX_DEFAULT_BOTTOM);
	gEGalaxEnabled = reader.GetBoolean("egalax", "enabled", false);

	gDownloadPath = reader.Get("downloads", "path", "downloads");
	
	return true;
}

static void handleADSBImage(string url, CURLDownloader *dloader, UIImageButton *planeImage)
{
	string fileName;
	string path;
	int32_t found = url.find_last_of("/");
	if (!found) {
		return;
	}
	fileName = url.substr(found+1);
	path = gDownloadPath + "/" + fileName;
	
	FILE *f = fopen(path.c_str(), "rb");
	if (!f) {
		dloader->asyncDownload(url);
	} else {
		planeImage->loadImage(path);
	}
}

char *degreeToString(int degree)
{
  char *str;
  if (degree <= 11.25) {
    str = (char*) "N";
  } else if (degree <= 33.75) {
    str = (char*) "NNO";
  } else if (degree <= 56.25) {
    str = (char*) "NO";
  } else if (degree <= 78.75) {
    str = (char*) "ONO";
  } else if (degree <= 101.25) {
    str = (char*) "O";
  } else if (degree <= 123.75) {
    str = (char*) "OSO";
  } else if (degree <= 146.25) {
    str = (char*) "SO";
  } else if (degree <= 168.75) {
    str = (char*) "SSO";
  } else if (degree <= 168.75) {
    str = (char*) "S";
  } else if (degree <= 213.75) {
    str = (char*) "SSV";
  } else if (degree <= 236.25) {
    str = (char*) "SV";
  } else if (degree <= 258.75) {
    str = (char*) "VSV";
  } else if (degree <= 281.25) {
    str = (char*) "V";
  } else if (degree <= 303.75) {
    str = (char*) "VNV";
  } else if (degree <= 326.25) {
    str = (char*) "NV";
  } else if (degree <= 348.75) {
    str = (char*) "NNV";
  } else {
    str = (char*) "N";
  }
  return str;
}

static void handleMQTTMessage(SDL_Event *event, CURLDownloader *dloader, UIImageButton *planeImage)
{
	char *topic = (char*) event->user.data1;
	char *payload = (char*) event->user.data2;
	printf("> Topic:%s | %s\n", topic, payload);

	if (!strcmp(topic, "/adsb/proximity/json")) {
	    json_t *root, *data;
	    json_error_t error;

		int timestamp;
		double distance;
		const char *origin = 0, *destination = 0;
		
		string distanceString = "";
		string aircraftTypeString = "";
		string altitudeString = "";
		string headingString = "";

//	    payload = "{\"bearing\": 116, \"distance\": \"32.91\", \"destination\": \"ARN\", \"vspeed\": 0, \"speed\": 466, \"image\": \"http://img.planespotters.net/photo/437000/original/D-AIZO-Lufthansa-Airbus-A320-200_PlanespottersNet_437547.jpg\", \"registration\": \"D-AIZO\", \"lon\": 13.82638, \"operator\": \"Lufthansa\", \"heading\": 32, \"lat\": 55.53561, \"callsign\": \"LH808\", \"time\": 1417728572, \"icao24\": \"3C674F\", \"type\": \"Airbus A320-214\", \"altitude\": 37025, \"origin\": \"FRA\"}";

	    root = json_loads(payload, 0, &error);
	    if (!root) {
	        printf("ERROR: JSON decode error on line %d: %s\n", error.line, error.text);
	        return;
	    }

		timestamp = json_integer_value(json_object_get(root, "time"));
		if (time(0) > timestamp + 60) {
			printf("### Data is old\n");;
		}

		// Extract image URL
		data = json_object_get(root, "image");
	    if (json_is_string(data)) {
			handleADSBImage(json_string_value(data), dloader, planeImage);
		}

		// Extract distance
		data = json_object_get(root, "distance");
	    if (json_is_real(data)) {
			char buff[100];
			distance = json_real_value(data);
			if (distance < 1) {
				sprintf((char*) buff, "%d m", (int) distance*1000);
			} else {
				sprintf((char*) buff, "%.1f km", distance);
			}
			distanceString = buff;
		} else {
			printf("Error: distance is not a real\n");
		}

		// Extract bearing
		data = json_object_get(root, "bearing");
	    if (json_integer_value(data)) {
			distanceString += " ";
			distanceString += degreeToString(json_integer_value(data));
		}

		// Extract altitude
		data = json_object_get(root, "altitude");
	    if (json_integer_value(data)) {
			char buff[100];
			sprintf((char*) buff, "%u ft", (int32_t) json_integer_value(data));
			altitudeString = buff;
		}

		// Extract speed
		data = json_object_get(root, "speed");
	    if (json_integer_value(data)) {
			char buff[100];
			sprintf((char*) buff, "   %d kt", (int32_t) json_integer_value(data));
			altitudeString += buff;
		}

		// Extract heading
		data = json_object_get(root, "heading");
	    if (json_integer_value(data)) {
			headingString = "Hdg ";
			headingString += degreeToString(json_integer_value(data));
		}

		// Extract aircraft type
		data = json_object_get(root, "type");
	    if (json_is_string(data)) {
			aircraftTypeString = json_string_value(data);
		}

		// Extract origin and destination
		data = json_object_get(root, "destination");
	    if (json_is_string(data)) {
			destination = json_string_value(data);
			data = json_object_get(root, "origin");
		    if (json_is_string(data)) {
				origin = json_string_value(data);
			}
			if (origin && destination && strlen(origin) > 0 && strlen(destination) > 0) {
				string s = origin;
				s += " > ";
				s += destination;
				gRoute->setCaption(s.c_str());
			} else {
				gRoute->setCaption("");
			}
		}

		gDistanceBearing->setCaption(distanceString);
		gAircraftType->setCaption(aircraftTypeString);
		gAltitudeSpeed->setCaption(altitudeString);
		gHeading->setCaption(headingString);

		/*	
			// json_real_value / json_is_real
		    if(!json_is_string(speed)) {
		        fprintf(stderr, "error: speed %p is not a string %s\n", speed, json_string_value(speed));
		    }
		    if(!json_is_number(speed)) {
		        fprintf(stderr, "error: speed %p is not a number %s\n", speed, json_integer_value(speed));
		        return 1;
		    }

			printf("Speed : %d\n", json_integer_value(speed));
		*/
	    json_decref(root);
	} else {
		printf("Unknown topic:%s\n", topic);
	}

	if (event->user.data1) {
		free(event->user.data1);
	}
	if (event->user.data2) {
		free(event->user.data2);
	}
}

int main(int argc, char ** argv)
{
	if (!readIniFile()) {
		printf("Please copy adsb-scope.ini to /etc\n");
		exit(1);
	}
	(void) SDL_RegisterEvents(1); // First call seem to return SDL_USEREVENT :-/
	SDL_ShowCursor(0);
	
	time_t t;
	srand((unsigned) time(&t));
	lib_init(); // mosquittopp

	SDL_Event event;
	SDL_Point *p = NULL;
	SDL_Color bgColor = {0, 0, 0, 255};
	bool hasSubscribed = false;
	CURLDownloader dloader(gDownloadPath);
	
    mosqpp::lib_init(); // TODO: Move somewhere else

	printf("Creating UI\n");
	UIGUI *ui = new UIGUI(SCREEN_WIDTH, SCREEN_HEIGHT);
	printf("Creating screen\n");

	char clientId[32];
	snprintf((char*) clientId, sizeof(clientId), "adsbscope-%d", rand());
	MQTTScreen *screen = new MQTTScreen(ui, (char*) clientId, gMQTTServer.c_str(), gMQTTPort, kMQTTMessage);
	EGalax *eGalax = new EGalax();
	if (gEGalaxEnabled) {
		printf("Connecting to eGalax : ");
		if (eGalax->connect()) {
			printf("success\n");
			eGalax->setCalibration(eGalaxCalib);
		} else {
			printf("failed\n");
		}
	}

	screen->setBackgroundColor(bgColor);

	UIImageButton *image = new UIImageButton(id_Any, 0, 0, "splash.png", kBackgroundTap);

	gAircraftType =    new UICaption(id_Any,  10,              5, 0, 0, "", "impact", 30, UIGUI::White, leftAdjust);
	gRoute =           new UICaption(id_Any,  SCREEN_WIDTH-10, 5, 0, 0, "", "impact", 30, UIGUI::White, rightAdjust);
	gDistanceBearing = new UICaption(id_Any,  10,              SCREEN_HEIGHT-65, 0, 0, "", "impact", 48, UIGUI::White, leftAdjust);
	gAltitudeSpeed   = new UICaption(id_Any,  SCREEN_WIDTH/2,  SCREEN_HEIGHT-65, 0, 0, "", "impact", 48, UIGUI::White, centerAdjust);
	gHeading         = new UICaption(id_Any,  SCREEN_WIDTH-10, SCREEN_HEIGHT-65, 0, 0, "", "impact", 48, UIGUI::White, rightAdjust);

	screen->AddElement((UIElement *) image);
	screen->AddElement((UIElement *) gAircraftType);
	screen->AddElement((UIElement *) gRoute);
	screen->AddElement((UIElement *) gDistanceBearing);
	screen->AddElement((UIElement *) gAltitudeSpeed);
	screen->AddElement((UIElement *) gHeading);

#if 0
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "White",  "impact", 24, UIGUI::White,  leftAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Black",  "impact", 24, UIGUI::Black,  centerAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Red",    "impact", 24, UIGUI::Red,    rightAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Green",  "impact", 24, UIGUI::Green,  leftAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Blue",   "impact", 24, UIGUI::Blue,   centerAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Yellow", "impact", 24, UIGUI::Yellow, leftAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Purple", "impact", 24, UIGUI::Purple, centerAdjust));
	screen->AddElement((UIElement *) new UICaption(id_Any, SCREEN_WIDTH-200,  10+25*(i++), 0, 0, "Cyan",   "impact", 24, UIGUI::Cyan,   rightAdjust));
#endif

	do {
		if (screen->isConnected() && !hasSubscribed) {
			screen->subscribe(NULL, "/adsb/proximity/json");
			hasSubscribed = true;
		}

		ui->run(screen, &event);
//		fprintf(stderr, "App got event %x\n", event.type);
		if (event.type == screen->getEventType()) {
			switch(event.user.code) {
				case kMQTTMessage:
					handleMQTTMessage(&event, &dloader, image);
					break;
				case kBackgroundTap:
					p = screen->getLastTap();
					printf("Image tapped at (%d, %d)\n", p->x, p->y);
					{
						UIMenu *menu = new UIMenu(ui);
						int32_t cmd;
						if (gEGalaxEnabled) {
							menu->addCmd("Calibrate", kCalibrate);
						}
						menu->addCmd("Quit", kQuit);
						menu->addSpacer();
						menu->addCmd("Cancel", kCancel);
						cmd = menu->display();
						delete menu;
						printf("Got %d from menu\n", cmd);
						switch(cmd) {
							case kCalibrate:
								{
									UITouchCalibrator *calib = new UITouchCalibrator(ui);
									if (calib->calibrate(eGalax)) {
										SDL_Rect *c = calib->getCalibration();
										printf("Calibration done: {%d, %d}, {%d, %d}\n", c->x, c->y, c->w, c->h);
										// TODO: Save calibration
									}
									delete calib;
								}
								break;
							case kQuit:
								exit(0);
								break;
						}
					}
					break;
			}
		} if (event.type == dloader.getEventType()) {
			if (event.user.data1) {
				printf("Loaded image\n");
				image->loadImage((char*) event.user.data1);
			} else {
				printf("Error! Image loading failed\n");
			}
		}

		if (p) {
			SDL_SetRenderDrawColor(UIGUI::GetRenderer(), 0xff, 0, 0, 0xff);
			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x-15, p->y-1, p->x+15, p->y-1);
			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x-15, p->y, p->x+15, p->y);
			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x-15, p->y+1, p->x+15, p->y+1);

			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x-1, p->y-15, p->x-1, p->y+15);
			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x, p->y-15, p->x, p->y+15);
			SDL_RenderDrawLine(UIGUI::GetRenderer(), p->x+1, p->y-15, p->x+1, p->y+15);
			SDL_RenderPresent(UIGUI::GetRenderer());
		}
	} while(event.user.code != event_Quit);
	delete ui;

	lib_cleanup(); // mosquittopp
	return 0;
}
