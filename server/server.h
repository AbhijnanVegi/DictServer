#if !defined(SERVER_H)
#define SERVER_H

#include <queue>
using namespace std;

#define PORT 8080
extern pthread_mutex_t requests_lock;
extern pthread_cond_t requests_cond;
extern queue<int> requests;

void start_server(int num_req);

#endif // SERVER_H
