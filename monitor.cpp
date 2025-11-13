#include "monitor.h"

Monitor::Monitor(int genCapacity, int vipCapacity)
{
    this->normalCapacity = genCapacity;
    this->availVIPSeats = availVIPSeats;
    this->queueGenReq = 0;
    this->queueVipReq = 0;
}