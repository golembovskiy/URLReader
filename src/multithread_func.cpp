
#include "stdafx.h"

#include <windows.h>
#include <wininet.h>

#include <iostream>
#include <string>
#include <mutex>

#include "glob_const.h"

using namespace std;

extern char content[MAX_NUM_URL][MAX_HTML_PAGE_SIZE];

mutex m;
volatile int url_list_index = 0;

void url_list_index_inc(int *index) {
	m.lock();

	*index = url_list_index;
	url_list_index++;

	m.unlock();
}

// The function we want to make the thread run
void getURLContent(wstring url, int content_index) {

	char * content_ptr = content[content_index];

	HINTERNET hSession = InternetOpen(L"MyAgent", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (!hSession) {
		cout << "Error open internet session" << endl;
		InternetCloseHandle(hSession);
		*content_ptr = '\0';
		return;
	}

	HINTERNET hConnect = InternetConnect(hSession, _T(""), INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
	HINTERNET hURL = InternetOpenUrl(hSession, url.c_str(), NULL, 0, 0, 0);

	if (!hURL) {
		cout << "Error open url" << endl;
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		*content_ptr = '\0';
		return;
	}

	DWORD bytesRead;
	int cnt = 0;
	while (InternetReadFile(hURL, content_ptr, 1024, &bytesRead) == TRUE && bytesRead > 0) {
		cnt += bytesRead;
		if (cnt >= MAX_HTML_PAGE_SIZE) {
			cout << "Error: html page too large" << endl;
			InternetCloseHandle(hURL);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);
			*content_ptr = '\0';
			return;
		}
		content_ptr += bytesRead;
	}
	*content_ptr = '\0';

	InternetCloseHandle(hURL);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);
}

void consumer(int url_count, wstring *url_list, int id) {
	cout << "Start thread " << id << endl;
	
	int index;
	do {
		url_list_index_inc(&index);
		if (index >= url_count) break;

		wcout << "Thread " << id << " started working with url: " << *(url_list + index) << endl;
		getURLContent(*(url_list + index), index);
		wcout << "Thread " << id << " finished with url: " << *(url_list + index) << endl;

	} while (index < url_count);
	
	cout << "Thread " << id << " finished" << endl;
}
