/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <stdlib.h> 
#include <cstdlib>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <string.h>
#include <curl/curl.h>

#include<iostream>
#include<cstdlib>
#include<ctime>
#include <time.h>

#include "training.hpp"

using namespace std;


string OriginalQuery;

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}


struct relement {
	string title;
	string desciption;
	string link;
};

typedef struct relement relement;

/* OCall functions */
void ocall_print_string(const char *str)
{
	/* Proxy/Bridge will check the length and null-terminate
	 * the input string to prevent buffer overflow.
	 */
	printf("%s", str);
}

vector <relement> parseJson(string buff) {

	vector <relement> result;
	relement elem;
	std::size_t end;

	std::size_t found = buff.find("\"value\": [");
	std::string s = std::to_string(found);

	buff = buff.substr(found + 10);
	for (size_t i = 0; i < 50; i++)
	{

		found = buff.find("\"name\": ");
		end = buff.find("\"url\": ");
		if (found != std::string::npos && end != std::string::npos) {
			elem.title = buff.substr(found + 9, end - found - 12);
			//ocall_print_string(elem.title.c_str());
			//ocall_print_string("\n");
		}
		else continue;
		buff = buff.substr(end);

		found = buff.find("\"url\": ");
		end = buff.substr(found + 11).find("\"");
		if (found != std::string::npos && end != std::string::npos) {
			elem.link = buff.substr(found + 8, end + 11 - found - 11);
			//ocall_print_string(elem.link.c_str());
			//ocall_print_string("\n");
		}
		else continue;

		found = buff.find("\"snippet\": ");
		end = buff.find("\"dateLastCrawled\": ");

		if (found != std::string::npos && end != std::string::npos) {
			elem.desciption = buff.substr(found + 12, end - found - 15);
			//ocall_print_string(elem.desciption.c_str());
			//ocall_print_string("\n");
			//ocall_print_string("\n");
		}
		else continue;

		buff = buff.substr(end + 20);
		result.push_back(elem);

	}

	return result;
}


void ocall_send(char *str, char ** ress, size_t *length) {
	CURL *curl;
	CURLcode res;

	/*Create queary */
	char const * endpoint = "https://api.bing.microsoft.com/v7.0/search?q=";
	char const * parameters = "&responseFilter=webpages&count=50";
	char message[150000];

	strcpy(message, endpoint);
	strcat(message, str);
	strcat(message, parameters);
	curl = curl_easy_init();

	struct MemoryStruct chunk;
	chunk.memory = (char *)malloc(1);  /* will be grown as needed by the realloc above */
	chunk.size = 0;    /* no data at this point */

	if (curl) {
		struct curl_slist *cslist = NULL;
		/* Modify a header curl otherwise adds differently */
		cslist = curl_slist_append(cslist, "Ocp-Apim-Subscription-Key:6455f7bec7cf4d7691b41186344c985d");

		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, cslist);
		curl_easy_setopt(curl, CURLOPT_URL, message);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));



		*ress = (char *)malloc((chunk.size + 1) * sizeof(char));

		strcpy(*ress, chunk.memory);

		*length = chunk.size;

		/* always cleanup */
		free(chunk.memory);


		curl_easy_cleanup(curl);

		/* we're done with libcurl, so clean it up */

		curl_global_cleanup();

	}


}




/*
 * printf:
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int main(int argc, char *argv[])
{
	if (argc < 2) {
		ocall_print_string("usage : ./app query");
		return 0;
	}


	string Q = argv[1];

	for (int i = 2; i < argc; i++) {
		Q += " " + std::string(argv[i]);
	}

	int k = 3;
	OriginalQuery = Q;
	
	ocall_send((char *)strQ.c_str(), &res, &length);

	char *trusted_res = new char[length + 1];

	memcpy(trusted_res, res, length);

	string ress = std::string(trusted_res);

	vector <relement> R = parseJson(ress);

	int stop = 0;
	for (std::vector<relement>::const_iterator Ri = R.begin(); Ri != R.end(); ++Ri) {
		ocall_print_string((Ri->link).c_str());
		ocall_print_string("\t");
		stop++;
		if (stop > 15) break;
	}
	ocall_print_string("\n");

	return 1;
}
