#include <iostream>
#include <ctype.h>
#include <cstring>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

int sock;
struct sockaddr_in client;
int PORT = 80;

int main(int argc, char const *argv[])
{
    struct hostent * host = gethostbyname("google.com");

    if ( (host == NULL) || (host->h_addr == NULL) ) {
		printf("Error reciving DNS information.\n");
        exit(1);
    }

    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons( PORT );
    memcpy(&client.sin_addr, host->h_addr, host->h_length);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
		printf("Error creating socket.\n");
        exit(1);
    }

    if ( connect(sock, (struct sockaddr *)&client, sizeof(client)) < 0 ) {
        close(sock);
		printf("Could not connect.\n");
        exit(1);
    }

    char * request = "GET /search?q=a+b HTTP/1.1\r\n \r\n\r\n";

    if (send(sock, request, sizeof(request), 0) != sizeof(request)) {
		printf("Error sending request.\n");
        exit(1);
    }

    char cur;
    while ( read(sock, &cur, 1) > 0 ) {
		printf(cur);
    }

    return 0;
}
