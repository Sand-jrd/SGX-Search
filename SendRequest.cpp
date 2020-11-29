
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

#include <stdio.h>
#include <stdlib.h>
#define PORT 8080

int main(void)
{
	int test = 0;
	int ok = 0;

	SOCKET sock;
	SOCKADDR_IN sin;

	char reception[9999] = "";

	struct hostent *HostInfo;
	HostInfo = gethostbyname("www.aerofrance-fs.fr");
	char *szLocalIP;
	szLocalIP = inet_ntoa(*(struct in_addr *) *HostInfo->h_addr_list);

	if (!erreur)
	{
		/* Création de la socket */
		sock = socket(AF_INET, SOCK_STREAM, 0);

		/* Configuration de la connexion */
		sin.sin_addr.s_addr = inet_addr(szLocalIP);
		sin.sin_family = AF_INET;
		sin.sin_port = htons(PORT);

		do
		{
			test++;
			/* Si le client arrive à se connecter */
			if (connect(sock, (SOCKADDR *)& sin, sizeof(sin)) !=
				SOCKET_ERROR)
			{
				int ret;
				send(sock,
					"GET /index.php HTTP/1.1\r\nhost: www.aerofrance-fs.fr\r\n\r\n",
					sizeof
					("GET /index.php HTTP/1.1\r\nhost: www.aerofrance-fs.fr\r\n\r\n"),
					0);
				printf("Connexion à %s sur le port %d\n",
					inet_ntoa(sin.sin_addr), htons(sin.sin_port));
				ok = 1;


				while ((ret = recv(sock, reception, 9999, 0)))
				{
					reception[ret] = '\0';
					printf("%s", reception);
				}

				shutdown(sock, 2);
			}
			else
				printf("Tentative de connexion %d...\n", test);
		} while (test < 10 && ok == 0);



		/* On ferme la socket précédemment ouverte */
		closesocket(sock);

	}


	return EXIT_SUCCESS;
}

/*
int main()
{
	const ushort dim_buff_recv = 1024; // taille du buffer de reception
	char buff_recv[dim_buff_recv];
	string requete_dateHeure = "GET /timer.php HTTP/1.1\r\nHost: 10.187.120.33\r\n\r\n";

	// Envoie de la requete au serveur
	send(ids_client, requete.c_str(), requete.size() + 1, 0);

	// Réception de la reponse du serveur
	recv(ids_client, buff_recv, sizeof(buff_recv), 0);
}
*/