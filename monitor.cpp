#include "monitor.h"
#include "log.h"
Monitor::Monitor(int maxProdReq, sem_t *barrierSem, int genCapacity, int vipCapacity)
{
    /*
    This contructor is for our monitor to instantiate takes in max produced request allowed for producer threads
    the semaphore barries to signal last consumer the genCapcity which is the general capacity of our queues
    and the vip capacity which is the vip slots within our general capacity
    */
    this->normalCapacity = genCapacity;              // instantiate our normal capacity for our buffer
    this->VIPCapacity = vipCapacity;                 //  instantiate our vip cacpacity
    this->maxProdRequests = maxProdReq;              // instantiate max amounf of requests that can be produced
    this->queueGenReq = 0;                           // instantiate requests in the general queue
    this->queueVipReq = 0;                           // instantiate VIP requests in queueu
    this->reqProduced = 0;                           // instantiate total amount of requests produced
    this->barrierSem = barrierSem;                   // instantiate our barrier semaphore pointer
    this->maxReqHit = false;                         // Instantiate our boolean to tell if we have hit the max amount of requests allowed
    this->unlockedBarrier = false;                   // Instantiate our boolean to check if our semaphore barrier has been unlocked by the last thread
    pthread_cond_init(&this->seatsAvail, NULL);      // Instantiate our condition for seats are available to fill or wait
    pthread_cond_init(&this->unconsumedSeats, NULL); // Instantiate our condition to signal that there are unconsumed seats on buffer or wait on seats by consumers
    pthread_cond_init(&this->VipSeatsAvail, NULL);   // We instantiate our condition to signal that there are vip seats available to fill
    pthread_mutex_init(&this->mutex, NULL);          // Instantiate our mutex for mutual exclusion in our critical section
    for (int i = 0; i < RequestTypeN; i++)
    {
        // This for loop instantiates our arrays to keep track of the requests by their types on either produced or consumed
        // as well as the amounts of each type in the queue
        this->prodByType[i] = 0; // Instantiate for the request types that have been produced so far
        this->consByType[i] = 0; // Instantiate for the request types that have been consumed so far
        this->queueTypes[i] = 0; // Instantiate for the amount of types of requests that are in the queue
    }
    for (int j = 0; j < ConsumerTypeN; j++)
    {
        // This for loop instantiates our array on the bases on consumer type robot
        this->consByRob[j] = 0;                                    // How many requests has each robot consumed
        this->consByRobType[j] = new unsigned int[RequestTypeN](); // How many requests has each robot consumed per type. This is an array of pointers that are instantiated to zero
    }
}

int Monitor::insert(RequestType request)
{
    /*
    This function insert a request into the buffer by the current producer thread. In takes in a request of type GeneralRoom or VIProom
    It ensurse mutual exclusion, progression and bounded waiting. It blocks and unblocks threads based on the general and vip capacites.
    It returns 1 for one request inserted and returns 0 when the maximum amount of requests that can be produced
    has been hit.
    */
    bool onlyItem;
    pthread_mutex_lock(&mutex);                                     // lock our mutex for mutual exclusion of ciritical section to manage our queue
    bool vipCapHit = this->queueVipReq >= this->VIPCapacity;        // We want to check if our vip capacity has been hit and store in boolean
    bool genCapHit = this->queueGenReq >= this->normalCapacity;     // check if we hit our general capacity of our queue max
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced); // We need to check if the max amount of reqeusts has been hit
    if (this->maxReqHit == true)
    {
        // We must check if the last completed the max amount of requests allowed
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN); // This signals all the consumers and producers waiting on conditions so they are not blocked after no more producing
        pthread_mutex_unlock(&mutex);                                 // Unlock our mutex
        return 0;                                                     // return zero since we dont produce any request and hit  max amount of allwoer requests to be produced
    }
    while ((request == VIPRoom && vipCapHit == true) || genCapHit == true)
    {
        /*This while loop checks for our vip capacity and the type of request being inserted or it checks
        the general capacity if its general request. THis loop will check if we are at max request
        if not it will wait depending on the the condition which is connected to the type of the request
        This enables the producer to wait depending on the request it is producing for a slot to be available to fill
        in the queue
        */
        if (this->maxReqHit == true)
        {
            // We must check if after waiting another producer thread hit the maximum amount of requests allowed to be produced
            this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN); // We need to signal all condition on the number of threads for consumer and producer in order to avoid deadlocks and threads blocked after we have hit the max amount of produced items
            pthread_mutex_unlock(&mutex);                                 // unlock our mutex to exit ciritical section
            return 0;                                                     // return zero since we were waiting and not request was inserted and we hit the max requests
        }
        if (genCapHit == true)
        {
            // This branch is for hitting our general capcacity in our queue we need to wait for slots to be avialbale to fill
            pthread_cond_wait(&seatsAvail, &this->mutex);
        }
        else
        {
            // This branch is to wait on vip slots available to fill. Since its not at the general capacity for queue as above checked
            // Then since we ran this loop we must be a vip request at vip capacity therefore we must wait for there to be a vip slot ot fill
            pthread_cond_wait(&VipSeatsAvail, &this->mutex);
        }
        // We must recheck and restore all our boolean for our max requests hit vip capacities and general queeu capacity since those might have changed since
        // we put th threads to sleep. This avoids the loop to run forever on previous unupdated values
        this->maxReqHit = (this->maxProdRequests <= this->reqProduced); // update max requests amount since sleep
        vipCapHit = this->queueVipReq >= this->VIPCapacity;             // update vip capacity since sleep
        genCapHit = this->queueGenReq >= this->normalCapacity;          // update our queue general capacity since sleep
    }
    // We must recheck we have not hit the max requests since sleep
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced);
    if (maxReqHit == true)
    {
        // This branch is if we hit the max amount of allowe requests since sleep
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN);
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    this->buffer.push(request);
    this->reqProduced += 1;
    this->queueGenReq += 1;
    if (request == VIPRoom)
    {
        this->queueVipReq += 1;
    }
    this->prodByType[request] += 1;
    this->queueTypes[request] += 1;
    output_request_added(request, this->prodByType, this->queueTypes);
    pthread_cond_signal(&this->unconsumedSeats);
    this->maxReqHit = this->maxProdRequests <= this->reqProduced;
    if (this->maxReqHit == true)
    {
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN);
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&this->mutex);
    return 1;
}
int Monitor::remove(Consumers robot)
{
    RequestType request;
    bool atCapacity;
    pthread_mutex_lock(&mutex);
    while (this->queueGenReq == 0)
    {
        // exit if production ended dont wait
        if (maxReqHit == true)
        {
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        pthread_cond_wait(&unconsumedSeats, &this->mutex);
    }
    atCapacity = (queueGenReq == this->normalCapacity);
    request = this->buffer.front();
    this->buffer.pop();
    this->queueGenReq -= 1;
    this->consByRob[robot] += 1;
    this->consByRobType[robot][request] += 1;
    this->queueTypes[request] -= 1;
    if (request == VIPRoom)
    {
        this->queueVipReq -= 1;
    }
    this->consByType[request] += 1;
    output_request_removed(robot, request, this->consByRobType[robot], this->queueTypes);
    if (maxReqHit && this->queueGenReq <= 0 && unlockedBarrier == false)
    {
        unlockedBarrier = true;
        sem_post(this->barrierSem);
        output_production_history(this->prodByType, (unsigned int **)this->consByRobType);
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN);
    }
    if (request == VIPRoom)
    {
        pthread_cond_signal(&this->VipSeatsAvail);
    }
    pthread_cond_signal(&this->seatsAvail);
    pthread_cond_signal(&this->seatsAvail);
    pthread_mutex_unlock(&this->mutex);
    return 1;
}

void Monitor::signal_all_cond(int numConsumers, int numProducers)
{
    for (int i = 0; i < numConsumers; i++)
    {
        pthread_cond_signal(&(this->unconsumedSeats));
    }
    for (int i = 0; i < numProducers; i++)
    {
        pthread_cond_signal(&(this->VipSeatsAvail));
        pthread_cond_signal(&(this->seatsAvail));
    }
    return;
}