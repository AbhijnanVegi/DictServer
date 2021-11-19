#if !defined(HANDLER_H)
#define HANDLER_H

#include <pthread.h>
#include <string>

using namespace std;

void init_handlers(int num_threads);

typedef struct val
{
    string s;
    pthread_mutex_t lock;
} val;

#endif // HANDLER_H
