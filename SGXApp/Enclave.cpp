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
#include "sgx_trts.h"

#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>

#include "training.hpp"
using namespace std;

string OriginalQuery;

struct relement {
	string title;
	string desciption;
	string link;
};

typedef struct relement relement;



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


vector <string> seperateWords(string S) {

	vector <string> words;
	transform(S.begin(), S.end(), S.begin(), ::tolower);
	string word = "";
	for (auto curr : S)
	{
		if (curr == ' ' || curr == '+' || curr == '|' || curr == '-') {
			words.push_back(word);
			word = "";
		}
		else {
			word = word + curr;
		}
	}
	words.push_back(word);
	return words;

}


int countCommon(string  Qi, string  Ri) {

	vector <string> QWrords = seperateWords(Qi);
	vector <string> RWrords = seperateWords(Ri);
	int score = 0;

	for (std::vector<string>::const_iterator Qwi = QWrords.begin(); Qwi != QWrords.end(); ++Qwi)
		for (std::vector<string>::const_iterator Rwi = RWrords.begin(); Rwi != RWrords.end(); ++Rwi) {
			if ((*Qwi).compare(*Rwi)==0)
				score++;
		}


	return score;
}

bool byscore(relement A, relement B){

	return ( countCommon(OriginalQuery, A.desciption) + countCommon(OriginalQuery, A.title) ) > ( countCommon(OriginalQuery, B.desciption) + countCommon(OriginalQuery, B.title) );

}

// Generate an obfuscated Query
/* INPUT  Q  : Initial Query
		  H  : History of Query
		  k  : Numbers of fake query
   OUTPUT OQ : obfuscatedQuery
*/
void obfuscated(string  Q, vector <string> H, int k, vector <string> * OQ) {


	int OQ_size = 0;
	int id, tmp;
	sgx_read_rand((unsigned char *)&id, 1);
	id = id % k;

	while (OQ_size <= k)
	{
		if (id == 0) {
			OQ->push_back(Q);
		}

		else {
			sgx_read_rand((unsigned char*) &tmp,1);
			OQ->push_back(H.at(tmp % H.size()));

		}

		id--;
		OQ_size++;
	}

}


// Fitrate Answere
/*INPUT  Q  : Initial Query
		 H  : History of Query
		 R  : Set of result
  OUTPUT ER : Estimatre result to the initial query
*/
void filtering(string  Q, vector <string> HQ, vector <relement> R, vector <relement> * ER) {


	int OQ_size = 0;

	HQ.insert(HQ.begin(), Q);   // Real query will be the first element 
	vector <int> scores;

	for (std::vector<relement>::const_iterator Ri = R.begin(); Ri != R.end(); ++Ri) {

		scores.clear();

		for (std::vector<string>::const_iterator Qi = HQ.begin(); Qi != HQ.end(); ++Qi) {
			scores.push_back(countCommon(*Qi, Ri->desciption) + countCommon(*Qi, Ri->title));
		}


		if (scores.at(0) == *max_element(std::begin(scores), std::end(scores))) {
			ER->push_back(*Ri);
		}

	}


}

/* 
 * printf: 
 *   Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char* fmt, ...)
{

	int k = 2;

	string Q = "fuck";

	// Answeres
	vector <string> OQ;
	obfuscated(Q, database, k, &OQ);


	string strQ = OQ.at(0);

	for (size_t i = 1; i < OQ.size() - 1; i++)
	{
		strQ = strQ + "+OR" + "+" + OQ.at(i);
	}


	char* res; // pointer to string in untrusted memory that we would get form OCALL
	size_t length = 0;  // length of string
	ocall_send((char *)strQ.c_str(), &res, &length);

	char *trusted_res = new char[length + 1];

	memcpy(trusted_res, res, length);

	string ress = std::string(trusted_res);
	//ocall_print_string(ress.c_str());



	vector <relement> R = parseJson(ress);


	vector <relement> ER; // reponse séléctioné
	filtering(Q, OQ, R, &ER);

	int stop = 0;
	for (std::vector<relement>::const_iterator Ri = ER.begin(); Ri != ER.end(); ++Ri) {
		//ocall_print_string((Ri->title).c_str());
		//ocall_print_string("\n");
		//ocall_print_string((Ri->desciption).c_str());
		//ocall_print_string("\n");
		ocall_print_string((Ri->link).c_str());
		//ocall_print_string("\n");
		ocall_print_string("\t");
		stop++;
		if (stop > 10) break;
	}
	ocall_print_string("\n");



 	 return 1;
}

void Xsearch(char * q)
{

	int k = 3;

	string Q = q;
	OriginalQuery = Q;
	// Answeres
	vector <string> OQ;
	obfuscated(Q, database, k, &OQ);


	string strQ = OQ.at(0);

	for (size_t i = 1; i < OQ.size() - 1; i++)
	{
		strQ = strQ + "+OR" + "+" + OQ.at(i);
	}


	char* res; // pointer to string in untrusted memory that we would get form OCALL
	size_t length = 0;  // length of string
	ocall_send((char *)strQ.c_str(), &res, &length);

	char *trusted_res = new char[length + 1];

	memcpy(trusted_res, res, length);

	string ress = std::string(trusted_res);
	//ocall_print_string(ress.c_str());



	vector <relement> R = parseJson(ress);


	vector <relement> ER; // reponse sÃ©lÃ©ctionÃ©
	filtering(Q, OQ, R, &ER);

	std::sort (ER.begin(), ER.end(), byscore);	


	int stop = 0;
	for (std::vector<relement>::const_iterator Ri = ER.begin(); Ri != ER.end(); ++Ri) {
		//ocall_print_string((Ri->title).c_str());
		//ocall_print_string("\n");
		//ocall_print_string((Ri->desciption).c_str());
		//ocall_print_string("\n");
		ocall_print_string((Ri->link).c_str());
		//ocall_print_string("\n");
		ocall_print_string("\t");
		stop++;
		if (stop > 15) break;
	}
	ocall_print_string("\n");


}

