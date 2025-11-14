#include <atomic>
#include "seating.h"

using namespace std;
#ifndef PRODUCER_H
#define PRODUCER_H

class Producer{
public:
    Producer(int sleepTime, RequestType type);
    RequestType prodType;
    static void setReqParams(int numRequests);
    static int maxRequests;
    static atomic<int> numRequests;
    static void* produce(void* args);
};
#endif