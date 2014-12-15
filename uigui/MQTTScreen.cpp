#include <SDL.h>
#include <MQTTScreen.h>
#include <iostream>
#include <assert.h>

static int mqttScreenThread(void *ptr)
{
	MQTTScreen *screen = (MQTTScreen*) ptr;
	while(1) {
		int rc = screen->loop();
		if (rc) {
			printf("Notice: MQTT reconnect\n");
			screen->reconnect();
		}
	}
	return 0;
}

MQTTScreen::MQTTScreen(UIGUI *ui, string id, string host, uint16_t port, uint32_t command) : UIScreen(ui), mosquittopp(id.c_str())
{
	int keepalive = 60;
	mHost = host;
	mPort = port;
	mIsConnected = false;
	mCommand = command;
	mThread = SDL_CreateThread(mqttScreenThread, "mqttScreenThread", (void*) this);
	/*mosquittopp::*/connect(mHost.c_str(), mPort, keepalive);
}

MQTTScreen::~MQTTScreen()
{
}

bool MQTTScreen::handleEvent(SDL_Event *event)
{
//	printf("MQTTScreen::HandleEvent %d\n", event->type);
	if (event->type == getEventType()) {
		return false;
	} else {
		return UIScreen::handleEvent(event);
	}
	return false;
}

void MQTTScreen::on_connect(int rc)
{
	printf("MQTT Connected with code %d.\n", rc);
	if(rc == 0){
		mIsConnected = true;
		/* Only attempt to subscribe on a successful connect. */
	}
}

void MQTTScreen::on_message(const struct mosquitto_message *message)
{
//	printf("Topic:%s | %s [%d]\n", message->topic, (char*) message->payload, message->mid);
	SDL_Event event;
	SDL_zero(event);
	event.type = getEventType();
	event.user.code = mCommand;
	event.user.data1 = (void*) strdup((const char*) message->topic);
	event.user.data2 = (void*) strdup((const char*) message->payload);
	SDL_PushEvent(&event);
}

void MQTTScreen::on_subscribe(int mid, int qos_count, const int *granted_qos)
{
	printf("MQTT subscription succeeded.\n");
}
