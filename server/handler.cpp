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

//////////////
#include <iostream>
#include <assert.h>
#include <tuple>
#include <queue>
#include <string>
#include <map>

using namespace std;

#include "server.h"
#include "handler.h"

map<int, val> M;

void init_dict(void)
{
    for (int i = 0; i <= 100; i++)
    {
        pthread_mutex_init(&M[i].lock, NULL);
        M[i].s = "";
    }
}

pair<string, int> read_string_from_socket(const int &fd, int bytes)
{
    std::string output;
    output.resize(bytes);

    int bytes_received = read(fd, &output[0], bytes - 1);
    if (bytes_received <= 0)
    {
        cerr << "Failed to read data from socket. \n";
    }

    output[bytes_received] = 0;
    output.resize(bytes_received);
    return {output, bytes_received};
}

int send_string_on_socket(int fd, const string &s)
{
    int bytes_sent = write(fd, s.c_str(), s.length());
    if (bytes_sent < 0)
    {
        cerr << "Failed to SEND DATA via socket.\n";
    }

    return bytes_sent;
}

string process_cmd(string cmd)
{
    string op = cmd.substr(0, cmd.find(" "));
    string ret = "Invalid operation";
    if (op == "insert")
    {
        string tmp = cmd.substr(cmd.find(" ") + 1);
        int key = stoi(tmp.substr(0, tmp.find(" ")));
        string value = tmp.substr(tmp.find(" ") + 1);
        pthread_mutex_lock(&M[key].lock);
        if (M[key].s.empty())
        {
            M[key].s = value;
            ret = "Insertion successful";
        }
        else
        {
            ret = "Key already exists";
        }
        pthread_mutex_unlock(&M[key].lock);
    }
    else if (op == "delete")
    {
        string tmp = cmd.substr(cmd.find(" ") + 1);
        int key = stoi(tmp);

        pthread_mutex_lock(&M[key].lock);
        if (M[key].s.size())
        {
            M.erase(key);
            ret = "Deletion successful";
        }
        else
        {
            ret = "No such key exists";
        }
        pthread_mutex_unlock(&M[key].lock);
    }
    else if (op == "update")
    {
        string tmp = cmd.substr(cmd.find(" ") + 1);
        int key = stoi(tmp.substr(0, tmp.find(" ")));
        string value = tmp.substr(tmp.find(" ") + 1);

        pthread_mutex_lock(&M[key].lock);
        if (M[key].s.size())
        {
            M[key].s = value;
            ret = M[key].s;
        }
        else
        {
            ret = "No such key exists";
        }
        pthread_mutex_unlock(&M[key].lock);
    }
    else if (op == "fetch")
    {
        string tmp = cmd.substr(cmd.find(" ") + 1);
        int key = stoi(tmp);
        cout << "key: " << key << endl;

        pthread_mutex_lock(&M[key].lock);
        if (M[key].s.size())
        {
            ret = M[key].s;
        }
        else
        {
            ret = "Key does not exist";
        }
        pthread_mutex_unlock(&M[key].lock);
    }
    else if (op == "concat")
    {
        string tmp = cmd.substr(cmd.find(" ") + 1);
        int key1 = stoi(tmp.substr(0, tmp.find(" ")));
        int key2 = stoi(tmp.substr(tmp.find(" ") + 1));

        while (true)
        {
            int k1_lock = pthread_mutex_trylock(&M[key1].lock);
            int k2_lock = pthread_mutex_trylock(&M[key2].lock);
            if (k1_lock == 0 && k2_lock == 0)
            {
                break;
            }
            else
            {
                pthread_mutex_unlock(&M[key1].lock);
                pthread_mutex_unlock(&M[key2].lock);
            }
        }
        if (M[key1].s.size() && M[key2].s.size())
        {
            string t = M[key1].s;
            M[key1].s += M[key2].s;
            M[key2].s += t;
            ret = M[key2].s;
        }
        else
        {
            ret = "Concat failed as at least one of the keys does not exist";
        }
        pthread_mutex_unlock(&M[key1].lock);
        pthread_mutex_unlock(&M[key2].lock);
    }
    return to_string(pthread_self()) + ":" + ret;
}

void *handler(void *arg)
{
    for (;;)
    {
        pthread_mutex_lock(&requests_lock);
        while (requests.empty())
        {
            pthread_cond_wait(&requests_cond, &requests_lock);
        }
        int client_sock = requests.front();
        requests.pop();
        pthread_mutex_unlock(&requests_lock);
        int received_num, sent_num;

        int ret_val = 1;

        string cmd;
        string msg_to_send_back;
        int sent_to_client;
        tie(cmd, received_num) = read_string_from_socket(client_sock, 1048576);
        ret_val = received_num;
        if (ret_val <= 0)
        {
            perror("Error reading from socket: ");
            goto close_client_socket_ceremony;
        }
        cout << "Client sent : " << cmd << endl;
        msg_to_send_back = process_cmd(cmd);

        sleep(2);
        sent_to_client = send_string_on_socket(client_sock, msg_to_send_back);
        // debug(sent_to_client);
        if (sent_to_client == -1)
        {
            perror("Error while writing to client. Seems socket has been closed");
            goto close_client_socket_ceremony;
        }
    close_client_socket_ceremony:;
        close(client_sock);
        cout << "Thread " << pthread_self() << " finished handling request" << endl;
    }
    return NULL;
}

void init_handlers(int num_threads)
{
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&threads[i], NULL, handler, NULL);
    }
}