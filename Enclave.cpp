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

#include "./SFML/Network.hpp"
#include "./SFML/Network/Http.hpp"

using namespace std;
using namespace sf;


#define MAXSIZE  1024;
#define HISTSIZE 1024;

void requete() {

	// préparation de la requête
	sf::Http::Request request("search?q=", sf::Http::Request::Post);

	/*
	// encodage des paramètres dans le corps de la requête
	std::ostringstream stream;
	stream << "a";
	request.setBody(stream.str());

	// envoi de la requête au serveur
	sf::Http http("https://www.google.com/");
	sf::Http::Response response = http.sendRequest(request);

	// vérification du statut
	if (response.getStatus() == sf::Http::Response::Ok)
	{
		// affichage de la réponse du serveur
		std::cout << response.getBody() << std::endl;
	}
	else
	{
		std::cout << "request failed" << std::endl;
	}
	*/
}


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




int main() {


	// Test Obfucated 
	cout << "\nStart Obfucated" << endl;

	string Q = "Comment dire Coucou";
	vector <string> H = { "il faut dire first","ou peux être sec"," 1 2 3 third","// é pm fourth" };
	vector <string> OQ;

	obfuscated(Q, H, 3, &OQ);

	for (std::vector<string>::const_iterator i = OQ.begin(); i != OQ.end(); ++i)
		std::cout << *i << " OR ";

	// Test Filtering
	cout << "\n\nStart Filtering" << endl;

	vector <string> R = { "first of all"," let me one sec","third alors","bon bon fourth","les coucou amies" };
	vector <string> ER;

	filtering(Q, H, R, &ER);

	for (std::vector<string>::const_iterator i = ER.begin(); i != ER.end(); ++i)
		std::cout << *i << " \n ";

	return 1;
}
