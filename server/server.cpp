#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>

/////////////////////////////
#include <iostream>
#include <assert.h>
#include <tuple>
#include <queue>

#include "server.h"

using namespace std;

pthread_mutex_t requests_lock;
pthread_cond_t requests_cond;
queue<int> requests;

void start_server(int num_req)
{
    int wel_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;

    wel_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (wel_sock < 0)
    {
        perror("ERROR creating welcome socket: ");
        exit(1);
    }

    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(wel_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("ERROR on binding: ");
        exit(1);
    }

    listen(wel_sock, 1024);
    cout << "Server is listening on port: " << PORT << endl;
    socklen_t client_len = sizeof(client_addr);

    while (1)
    {
        client_sock = accept(wel_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0)
        {
            perror("ERROR on accept: ");
            exit(1);
        }
        cout << "Client connected on port " << ntohs(client_addr.sin_port) << " and IP " << inet_ntoa(client_addr.sin_addr) << endl;
        pthread_mutex_lock(&requests_lock);
        requests.push(client_sock);
        pthread_cond_signal(&requests_cond);
        pthread_mutex_unlock(&requests_lock);
    }
    close(wel_sock);

    cout << "Server closed" << endl;
}
