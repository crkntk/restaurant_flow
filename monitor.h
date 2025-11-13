#include <pthread.h>

#ifndef MONITOR_H
#define MONITOR_H

class Monitor{

    Monitor();
    pthread_mutex_t mutex;
    
};
#endif