#include <iostream>
#include <string>
#include <stdlib.h>
#include <pthread.h>

#include "client.h"

using namespace std;

void sim(void)
{
    int N;
    cin >> N;
    pthread_t threads[N];
    for (int i = 0; i < N; i++)
    {
        request* r = (request*) malloc(sizeof(request));
        int time;
        string s;
        cin >> r->time;
        getline(cin, s);
        s = s.substr(1);
        r->msg = s;
        r->index = i;
        pthread_create(&threads[i], NULL, begin_process, r);
    }
    for (int i = 0; i < N; i++)
    {
        pthread_join(threads[i],NULL);
    }
    return;
}

int main()
{
    sim();
    return 0;
}