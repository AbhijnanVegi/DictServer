#if !defined(CLIENT_H)
#define CLIENT_H

#include <string>

using namespace std;

typedef struct request
{
    int index;
    int time;
    string msg;
} request;

void* begin_process(void *req);

#endif // CLIENT_H
