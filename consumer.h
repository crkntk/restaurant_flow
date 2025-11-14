


#ifndef CONSUMER_H
#define CONSUMER_H

class Consumer{
public:
    Consumer(int timeSleep);
    int timeSleep;
    static void* consume(void* args);
};
#endif