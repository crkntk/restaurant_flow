#include <pthread.h>
#include <queue>
#ifndef MONITOR_H
#define MONITOR_H

class Monitor{

    Monitor();
    pthread_mutex_t mutex;
    pthread_cond_t  condition_cond;
    pthread_cond_t  VIPRoom;
    pthread_cond_t  normalRoom;
    int normalCapacity;
    int VIPCapacity;
};
#endif