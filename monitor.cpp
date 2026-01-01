// Name: Carlos Reyes REDID: 131068259
#include<cmath>
#include "monitor.h"
#include "log.h"

Monitor::Monitor(int maxProdReq, sem_t *barrierSem,string policy, int genCapacity, int vipCapacity)
{
    /*
    This constructor is for our monitor to instantiate takes in max produced request allowed for producer threads
    the semaphore barrier to signal last consumer for main thread the genCapcity which is the general capacity of our queues
    and the vip capacity which is the vip slots within our general capacity
    */
    this->normalCapacity = genCapacity;              // Instantiate our normal capacity for our buffer
    this->VIPCapacity = vipCapacity;                 // Instantiate our vip capacity
    this->maxProdRequests = maxProdReq;              // Instantiate max amount of requests that can be produced
    this->queueGenReq = 0;                           // Instantiate requests in the general queue
    this->queueVipReq = 0;                           // Instantiate VIP requests in queue
    this->reqProduced = 0;                           // Instantiate total amount of requests produced
    this->barrierSem = barrierSem;                   // Instantiate our barrier semaphore pointer
    this->maxReqHit = false;                         // Instantiate our boolean to tell if we have hit the max amount of requests allowed
    this->unlockedBarrier = false;                   // Instantiate our boolean to check if our semaphore barrier has been unlocked by the last thread
    this->policy = policy;
    this->fifoPriority = 0; 
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
        this->consByRobType[j] = new unsigned int[RequestTypeN](); // How many requests has each robot consumed per type. This is an array of pointers that are instantiated to zero. Used for logging
    }
}

int Monitor::insert(RequestType request)
{
    /*
    This function insert a request into the buffer by the current producer thread. In takes in a request of type GeneralRoom or VIProom
    It ensures mutual exclusion, progression and bounded waiting. It blocks and unblocks threads based on the general and vip capacities.
    It returns 1 for one request inserted and returns 0 when the maximum amount of requests that can be produced
    has been hit.
    */
    pthread_mutex_lock(&mutex);                                     // lock our mutex for mutual exclusion of critical section to manage our queue
    bool vipCapHit = this->queueVipReq >= this->VIPCapacity;        // We want to check if our vip capacity has been hit and store in boolean
    bool genCapHit = this->queueGenReq >= this->normalCapacity;     // check if we hit our general capacity of our queue max
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced); // We need to check if the max amount of requests has been hit
    if (this->maxReqHit == true)
    {
        // We must check if the last completed the max amount of requests allowed
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN); // This signals all the consumers and producers waiting on conditions so they are not blocked after no more producing
        pthread_mutex_unlock(&mutex);                                 // Unlock our mutex
        return 0;                                                     // return zero since we dont produce any request and hit  max amount of allowed requests to be produced
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
            pthread_mutex_unlock(&mutex);                                 // unlock our mutex to exit critical section
            return 0;                                                     // return zero since we were waiting and not request was inserted and we hit the max requests
        }
        if (genCapHit == true)
        {
            // This branch is for hitting our general capacity in our queue we need to wait for slots to be available to fill
            pthread_cond_wait(&seatsAvail, &this->mutex);
        }
        else
        {
            // This branch is to wait on vip slots available to fill. Since its not at the general capacity for queue as above checked
            // Then since we ran this loop we must be a vip request at vip capacity therefore we must wait for there to be a vip slot ot fill
            pthread_cond_wait(&VipSeatsAvail, &this->mutex);
        }
        // We must recheck and restore all our boolean for our max requests hit vip capacities and general queue capacity since those might have changed since
        // we put th threads to sleep. This avoids the loop to run forever on previous unupdate values
        this->maxReqHit = (this->maxProdRequests <= this->reqProduced); // update max requests amount since sleep
        vipCapHit = this->queueVipReq >= this->VIPCapacity;             // update vip capacity since sleep
        genCapHit = this->queueGenReq >= this->normalCapacity;          // update our queue general capacity since sleep
    }
    // We must recheck we have not hit the max requests since sleep
    this->maxReqHit = (this->maxProdRequests <= this->reqProduced);
    if (maxReqHit == true)
    {
        // This branch is if we hit the max amount of allowed requests since sleep
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN); // This signals all the consumers and producers waiting on conditions so they are not blocked after no more producing
        pthread_mutex_unlock(&mutex);                                 // unlock our mutex leaving critical section
        return 0;                                                     // return zero since we were waiting and not request was inserted and we hit the max requests
    }
    RequestObj insReqObj;
    if(this->policy == "fifo"){
        insReqObj = {this->fifoPriority,request};
        this->fifoPriority += 1;
    }
    else if(this->policy == "vip_priority"){
        if(request == VIPRoom){
            insReqObj = {1,request};
        }
        else{
            insReqObj = {2,request};
        }
    }
    else if (this->policy == "fair"){
        float genAverage = static_cast<float>(this->queueGenReq) / static_cast<float>(MONITOR_GEN_CAP);
        float vipAverage = static_cast<float>(this->queueVipReq) / static_cast<float>(MONITOR_GEN_CAP);
        float averageDiff = genAverage - vipAverage;
                        // same as (x/n) - (y/n)
        float k = 1.96f;                       // or 3.0f
        float threshold = (static_cast<float>(this->queueGenReq) + static_cast<float>(this->queueVipReq) > 0) ? k * sqrtf(static_cast<float>(this->queueGenReq) + static_cast<float>(this->queueVipReq)) / static_cast<float>(MONITOR_GEN_CAP) : 0.0f;
        bool significant = fabsf(averageDiff) > threshold;
        if(averageDiff > threshold){
            if(request == VIPRoom){
                insReqObj = {1,request};
            }
            else{
                insReqObj = {2,request};
            }
        }
        else if (averageDiff < -threshold){
            if(request == VIPRoom){
                insReqObj = {2,request};
            }
            else{
                insReqObj = {1,request};
            }
        }
        else{
            if(request == VIPRoom){
                insReqObj = {1,request};
            }
            else{
                insReqObj = {1,request};
            }
        }
    }
    this->buffer.push(insReqObj); // push to our buffer since we have done all the checks needed for the conditions and capcity
    // Update our values for logging and the amount of requests in the queue
    this->reqProduced += 1; // update amount of requests produced overall
    this->queueGenReq += 1; // updated the amount of requests in our queue
    if (request == VIPRoom)
    {
        // This branch is for if the request was vip we update our vip amount of requests that are in the queue
        this->queueVipReq += 1;
    }
    // We updated our arrays for produced amount by request type overall and the amount of each of that is actually in the queue
    this->prodByType[request] += 1;
    this->queueTypes[request] += 1;
    output_request_added(request, this->prodByType, this->queueTypes); // log output of our request that was added along with the overall requests by type and the amount of requests by type in the queue
    pthread_cond_signal(&this->unconsumedSeats);                       // Since we inserted a request we must signal the consumer threads waiting for the unconsumed condition to consume a seat
    this->maxReqHit = this->maxProdRequests <= this->reqProduced;      // We must check that we didn't hit our max amount of requests after added the request and signalling
    if (this->maxReqHit == true)
    {
        /*This branch treats the condition of after inserting we hit a the maximum amount of requests
        if we dont have this before we unlock we will hit a deadlock if our vip hit its capacity and both consumer threads
        are blocked and only one is signaled. On the consumer side the same thread may have a different value for hte boolean
        and get stuck at the top of remove function while loop if after waiting it pops and maximum requests hit is false
        We must do this check in the area of mutual exclusion/critical region to assert the value will be accurate
        */
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN); // We need to signal all condition on the number of threads for consumer and producer in order to avoid deadlocks and threads blocked after we have hit the max amount of produced items
        pthread_mutex_unlock(&mutex);                                 // unlock our mutex leaving critical section
        return 1;                                                     // We hit max here when we inserted so we return 1 to flag that we added 1 requests
    }
    pthread_mutex_unlock(&this->mutex); // exit of critical section for the whole function if we added a request without hitting the conditions to exit otherwise
    return 1;                           // return 1 since we inserted a request
}
int Monitor::remove(Consumers robot)
{
    /*
        This function removes a request from the buffer. The buffer is handled as FIFO. It takes in the robot that consumes the
        request as the argument to simulate the removal by the robot thread in a mutually exclusive way
    */
    RequestType request;        // Our request type that is removed from the queue
    pthread_mutex_lock(&mutex); // We lock our mutex for our critical section to be access in a mutually exclusive way
    while (this->queueGenReq == 0)
    {
        /*This while loop checks to see if there are any requests in the queue
        if there is none we wait for our producers to make more requests based on the unconsumed seats conditions
        If a consumer thread is woken and still there are no seats available to consume in the queue then we make it wait again
        */
        if (maxReqHit == true)
        {
            // We check if we have hit the max amount of requests that can be produced. We should not wait for a producer to signal
            // unconsumed seats since there will be no producer producing and the thread will just be blocked
            pthread_mutex_unlock(&mutex); // We release our mutex lock
            return 0;                     // return 0 since we hit max after waiting and did not consumer a request
        }
        pthread_cond_wait(&unconsumedSeats, &this->mutex); // We wait for more seats to be produced by our producer threads
    }
    request = (this->buffer.top()).request;           // Get the request from the front of our queue and store
    this->buffer.pop();                       // pop the request from our queue
    this->queueGenReq -= 1;                   // We update the amount of requests in our buffer
    this->consByRob[robot] += 1;              // Update the amount of request that have been consumed for this consumer/Robot
    this->consByRobType[robot][request] += 1; // Update the amount of request array of the current consumer/robot has consumed per type of request
    this->queueTypes[request] -= 1;           // Update the types of requests that are in the queue based on the request removed
    if (request == VIPRoom)
    {
        // If our request was a vip request we must update the amount of vip requests in the queue
        this->queueVipReq -= 1;
    }
    this->consByType[request] += 1; // Update our consumed requests by type array for the amount of requests consumed by type
    // This is the output function for logging the request that was removed takes in robot
    // the request removed the 2D array of requests consumed by robot type and request type and the request types that are currently in queue
    output_request_removed(robot, request, this->consByRobType[robot], this->queueTypes);
    if ((this->maxReqHit == true) && (this->queueGenReq <= 0) && (unlockedBarrier == false))
    {
        // This branch is for our last consumer that removed the last request in the queue and there are
        // no more requests being produced and its the first time we hit this branch
        // In order to guard from other threads posting as well we have a flag so that only one semaphore post happens as a safeguard
        unlockedBarrier = true;                                                            // set our boolean for posting once to the semaphore barrier to true
        output_production_history(this->prodByType, (unsigned int **)this->consByRobType); // We log our request history using the array of produced by request type and the 2D array of the robot and its amount of request by type
        this->signal_all_cond((int)ConsumerTypeN, (int)RequestTypeN);                      // We signal all our conditions per consumer thread and request threads so no threads are blocked
        sem_post(this->barrierSem);                                                        // We post to our semaphore so that our main thread can continue and kill off the threads still running
    }
    else
    {
        // This branch is if we removed and we are not in the last consumed request
        // We signal the thread based on what was removed
        if (request == VIPRoom)
        {
            // Branch to signal the condition for vip seats available to fill
            pthread_cond_signal(&this->VipSeatsAvail);
        }
        else
        {
            // Signal if request is of general type in case the blocked for general producing requests is blocked signal that there is general sits available to fill
            pthread_cond_signal(&this->seatsAvail);
        }
    }
    pthread_mutex_unlock(&this->mutex); // release the lock after removal
    return 1;                           // return one for the amount of removed requests
}

void Monitor::signal_all_cond(int numConsumers, int numProducers)
{
    /*
    This functions signals all the conditions that have to do with the amount of consumers and producers
    in respect to each of the conditions pertaining to the consumers or the producers
    In future iterations it would be beneficial to have this conditions in two array with pointers to the conditions
    This function is used at the end of producing and the end of consuming so no threads are blocked
    This is not assuming that we know the amount of threads blocked but it is the max amount of threads that could be blocked for this conditions
    */
    for (int i = 0; i < numConsumers; i++)
    {
        // Signals all consumer threads for its conditions so its not blocked
        pthread_cond_signal(&(this->unconsumedSeats));
    }
    for (int i = 0; i < numProducers; i++)
    {
        // Signal all producer threads if any for its conditions so they are not blocked
        pthread_cond_signal(&(this->VipSeatsAvail));
        pthread_cond_signal(&(this->seatsAvail));
    }
    return;
}