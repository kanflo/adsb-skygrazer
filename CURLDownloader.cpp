#include <string>
#include <curl/curl.h>
#include "CURLDownloader.h"
#include "curl_fopen.h"
#include <unistd.h>

static int cURLDownloadThread(void *ptr)
{
	CURLDownloader *downloader = (CURLDownloader*) ptr;

	URL_FILE *handle;
	FILE *outf;
	int nread;
	char buffer[128*1024];
	string localPath, downloadPath;
	string fileName;
	string url = downloader->getURL();
	bool success = true;
	SDL_Event event;
	SDL_zero(event);
	
	int32_t found = url.find_last_of("/");
	if (!found) {
		goto fail;
	}
	fileName = url.substr(found+1);
	
	localPath = downloader->getSavePath() + "/" +  fileName;
	downloadPath = localPath + ".download";

	printf("cURLDownloadThread is downloading %s to %s\n", url.c_str(), localPath.c_str());
	
	/* Copy from url with fread */
	outf = fopen(downloadPath.c_str(), "w+");
	if(!outf) {
		perror("couldn't open fread output file\n");
		return false;
	}

	handle = url_fopen(downloader->getURL().c_str(), "r");

	if(!handle) {
		printf("couldn't url_fopen() testfile\n");
		fclose(outf);
		goto fail;
	}

	do {
		nread = url_fread(buffer, 1,sizeof(buffer), handle);
		printf(" Read %d bytes\n", nread);
		fwrite(buffer, 1, nread, outf);
	} while(nread);

	url_fclose(handle);

	fclose(outf);
	goto finish;

fail:
	success = false;
	unlink(downloadPath.c_str());
finish:
	rename(downloadPath.c_str(), localPath.c_str());

	event.type = downloader->getEventType();
	event.user.code = success;
	event.user.data1 = (void*) localPath.c_str();
	SDL_PushEvent(&event);

	printf("cURLDownloadThread completed, posting event %d\n", event.type);
	downloader->downloadFinished();
	
	return 0;
}

CURLDownloader::CURLDownloader(string savePath)
{
	mSavePath = savePath;
	mEventType = SDL_RegisterEvents(1);
	mThread = NULL;
}

CURLDownloader::~CURLDownloader()
{
}

// Downloads url (eg http://imgs.xkcd.com/static/terrible_small_logo.png)
// and saves to <savePath>/terrible_small_logo.png
void CURLDownloader::asyncDownload(string url)
{
	if (mThread) {
		printf("CURLDownloader already downloading\n");
	} else {
		printf("CURLDownloader starting download\n");
		mURL = url;
		mThread = SDL_CreateThread(cURLDownloadThread, "cURLDownloadThread", (void*) this);
	}
}

void CURLDownloader::downloadFinished()
{
	int32_t status;
	SDL_WaitThread(mThread, &status);
	mThread = NULL;
}