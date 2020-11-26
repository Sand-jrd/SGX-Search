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
#include <iostream>
#include <sstream>
#include <algorithm>


#include "Enclave.h"
#include "Enclave_t.h" /* print_string */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <string.h>




vector <string> seperateWords(string S) {
	std::stringstream ss(S);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);
	return vstrings;
}


int countCommon(string  Qi, string  Ri) {

	vector <string> QWrords = seperateWords(Qi);
	vector <string> RWrords = seperateWords(Ri);
	int score = 0;

	for (std::vector<string>::const_iterator Qwi = QWrords.begin(); Qwi != QWrords.end(); ++Qwi)
		for (std::vector<string>::const_iterator Rwi = RWrords.begin(); Rwi != RWrords.end(); ++Rwi) {
			if (*Qwi == *Rwi)
				score++;
		}


	return score;
}


// Generate an obfuscated Query
/* INPUT  Q  : Initial Query
		  H  : History of Query
		  k  : Numbers of fake query
   OUTPUT OQ : obfuscatedQuery
*/
void obfuscated(string  Q, vector <string> H, int k, vector <string> * OQ) {

	int OQ_size = 0;
	int id = rand() % k + 1;

	while (OQ_size <= k)
	{
		if (id == 0) {
			OQ->push_back(Q);
		}

		else {
			OQ->push_back(H.at(rand() % 4));

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
void filtering(string  Q, vector <string> HQ, vector <string> R, vector <string> * ER) {


	int OQ_size = 0;

	HQ.insert(HQ.begin(), Q);   // Real query will be the first element 
	vector <int> scores;

	for (std::vector<string>::const_iterator Ri = R.begin(); Ri != R.end(); ++Ri) {

		scores.clear();

		for (std::vector<string>::const_iterator Qi = HQ.begin(); Qi != HQ.end(); ++Qi) {
			scores.push_back(countCommon(*Qi, *Ri));
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

	// Test Obfucated 
	cout << "\nStart Obfucated" << endl;

	string Q = "Comment dire Coucou";
	vector <string> H = { "il faut dire first","ou peux être sec"," 1 2 3 third","// é pm fourth" };
	vector <string> OQ;

	obfuscated(Q, H, 3, &OQ);

	// Test Filtering
	cout << "\n\nStart Filtering" << endl;

	vector <string> R = { "first of all"," let me one sec","third alors","bon bon fourth","les coucou amies" };
	vector <string> ER;

	filtering(Q, H, R, &ER);

    ocall_print_string("c'est bon :) ");

    return 1;
}
