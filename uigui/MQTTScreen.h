#ifndef __MQTTSCREEN_H__
#define __MQTTSCREEN_H__

#include <UIScreen.h>
#include <mosquittopp.h>

using namespace std;
using namespace mosqpp;

class MQTTScreen : public UIScreen, public mosquittopp
{
	public:
		MQTTScreen(UIGUI *ui, string id, string host, uint16_t port, uint32_t command);
		~MQTTScreen();
		bool handleEvent(SDL_Event *event);

		void on_connect(int rc);
		void on_message(const struct mosquitto_message *message);
		void on_subscribe(int mid, int qos_count, const int *granted_qos);
		
		bool isConnected() { return mIsConnected; };

	protected:
		bool mIsConnected;
		string mHost;
		uint16_t mPort;
		uint32_t mCommand;
	 	SDL_Thread *mThread;
};


#endif // __MQTTSCREEN_H__
