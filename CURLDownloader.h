#ifndef __CURLDOWNLOADER_H__
#define __CURLDOWNLOADER_H__

#include <string.h>
#include <SDL.h>

using namespace std;

class CURLDownloader
{
	public:
		CURLDownloader(string savePath);
		~CURLDownloader();
		void asyncDownload(string url);
		uint32_t getEventType() { return mEventType; };
		string getURL() { return mURL; };
		string getSavePath() { return mSavePath; };
		void downloadFinished();

	protected:
		
	 	SDL_Thread *mThread;
		string mSavePath;
		string mURL;
		uint32_t mEventType;
};

#endif // __CURLDOWNLOADER_H__
