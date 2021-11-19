#include <iostream>
#include <stdlib.h>

using namespace std;

#include "server.h"
#include "handler.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Usage: ./srvr <num_threads>" << endl;
        return 1;
    }
    int num_threads = atoi(argv[1]);
    init_handlers(num_threads);
    start_server(num_threads);
}