
#include "seating.h"
#include "monitor.h"
#ifndef CONSUMER_H
#define CONSUMER_H
/*
This class is the consumer class that keeps track of the sleep for the consumer thread and the name/type
of the robot for the thread. It has the static function that is for the posix thread library to run in our main function
to run the current consumer thread with its arguments structure. We grab our request types from the enumerator in seating.h
*/
class Consumer
{
public:
    Consumer(int timeSleep, ConsumerType consumerType); // Constructor for current consumer keeps track of the sleeping time and the type of the robot
    int timeSleep;                                      // Sleeping time for consumer thread
    ConsumerType ConsType;                              // Consumer Type/name for the current robot
    /*
    Static function for the consumer thread for the posix library to start the thread with a pointer to the arguments held by a structure
    The functions consumes an objec but passes the consuming part to the pointer to the monitor for delegation to manage teh buffer
    The fucntionality is to  loop call remove and sleep for the given amount
    */
    static void *consume(void *args);
    /*
    Our strucuture that holds the arguments for the posix thread creation for the static function in our consumer class
    It holds a pointer to our monitor and a pointer to the current conumer object
    */
    struct consEntityArgs
    {
        Monitor *simMonitor;   // The monitor for the current simulation manages all removes from consumer and its available slots
        Consumer *consumerObj; // The current consumer object with its attributes needed to runt he static function
    };
};
#endif