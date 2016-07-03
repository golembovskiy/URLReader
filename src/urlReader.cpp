
#include "stdafx.h"

#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "glob_const.h"

#pragma comment(lib, "wininet.lib")

using namespace std;

extern void consumer(int url_count, wstring *url_list, int id);

char content[MAX_NUM_URL][MAX_HTML_PAGE_SIZE];

int input_val(string msg) {
	int val;
	cout << msg;
	cin >> val;
	cout << endl;
	return val;
}

//producer
int create_url_list(string file_name, wstring *url_list) {
	int url_count = 0;
	wifstream file(file_name);
	if (!file.is_open()) {
		cout << "File open error" << endl;
		return 0;
	}
	else {
		for (wstring url; getline(file, url);) {
			*(url_list + url_count++) = url;
			if (url_count == MAX_NUM_URL) {
				cout << "Error: Max num URL must be " << MAX_NUM_URL << endl;
				return url_count;
			}
		}
	}
	return url_count;
}

void create_and_run_consumers(
	int num_threads,
	int url_count,
	wstring *url_list) {

	thread threads[THREADS_ARR_SIZE];

	cout << "Start reading..." << endl;

	for (int i = 0; i < num_threads; i++)
		threads[i] = thread(consumer, url_count, url_list, i + 1);

	for (int i = 0; i < num_threads; i++) threads[i].join();
}

void out_content(int url_count) {
	cout << "Reading is finished" << endl;
	int out_flag = input_val("Output content in (0 - file, 1 - cout): ");
	if (out_flag) {
		for (int i = 0; i < url_count; i++) {
			cout << content[i] << endl;
			cout << "------------------------------------------------------------------" << endl;
		}
	}
	else {
		wofstream file("content.txt");

		if (!file.is_open()) {
			cout << "Error creating file 'content.txt'" << endl;
			return;
		}

		for (int i = 0; i < url_count; i++) {
			file << content[i] << endl;
			file << "------------------------------------------------------------------" << endl;
		}

		cout << "The file 'content.txt' was created successfully" << endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	int num_threads = input_val("Input number threads: ");
	wstring url_list[MAX_NUM_URL];
	int url_count = create_url_list("links.txt", url_list);

	create_and_run_consumers(num_threads, url_count, url_list);

	out_content(url_count);

	system("pause");

	return 0;
}
