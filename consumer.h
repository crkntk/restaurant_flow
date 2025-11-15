


#ifndef CONSUMER_H
#define CONSUMER_H
struct entityFlowArgs {
    Monitor* simMonitor;
    Consumer* consumerObj;
};
class Consumer{
public:
    Consumer(int timeSleep);
    int timeSleep;
    static void* consume(void* args);
};
#endif