#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <pthread.h>
#include "producer.h"
#include "consumer.h"
#include "monitor.h"

using namespace std;

int main(int argc, char **argv)
{
    /*This is our main entry point for our program. It simulate producer consumer problem
    with two producer threads and two consumer threads with some requests items having
    higher priority than others. It is a restaurant simulated producer consumer problems with
    vip seats and general seats in the queue. We distinguish between producers by what is being
    produced vip or general seats and two consumer robots are running as two threads which may have
    different sleep times for thread delays of consumption. Producers also have this sleep delays
    */
    int seatingReq = 110; // Default maximum number of requests that can be produced by both threads
    int sleepTX = 0;      // Default time for our TX robot thread
    int sleepRev9 = 0;    // Default time for our Rev 9 robot thread
    int sleepGen = 0;     // Default sleep time for our general producer thread
    int sleepVip = 0;     // Default time for our vip seat producer thread
    // We extract above default optional arguments
    int option;
    while ((option = getopt(argc, argv, "s:x:r:g:v:")) != -1)
    {
        /* If the option has an argument, optarg is set to point to the
         * argument associated with the option.  For example, if
         * -n is processed, optarg points to "56" in the example above.
         */
        switch (option)
        {
        case 's':
            // Maximum number of requests that can be produced
            seatingReq = atoi(optarg);
            break;
        case 'x':
            sleepTX = atoi(optarg); // Sleep time for tx consumer robot thread
            break;
        case 'r':
            sleepRev9 = atoi(optarg); // Sleep time for rev 9 consumer thread
            break;
        case 'g':
            sleepGen = atoi(optarg); // Sleep time for our general producer thread
            break;
        case 'v':
            sleepVip = atoi(optarg); // Sleep time for our vip producer thread
            break;
        default:
            break;
        }
    }
    // Start our semaphore barrier in order for our main thread to wait for last consumer to consume
    // before finishing
    sem_t semBarrier;
    sem_init(&semBarrier, 0, 0); // instantiating semaphore
    // We declare our threads for general and vip and robot consumer threads
    pthread_t genProdThread, vipProdThread;
    pthread_t robTxThread, robRevThread;
    // create our producer and consumer objects for the thread functions with its types and its time to sleep for
    // Types come from seating.h file enumerator
    Producer *genProd = new Producer(sleepGen, GeneralTable); // General producer with sleeping for thread
    Producer *vipProd = new Producer(sleepVip, VIPRoom);      // Vip producer with sleep for thread
    Consumer *robTx = new Consumer(sleepTX, TX);              // Tx robot object consumer with sleep time
    Consumer *robRev = new Consumer(sleepRev9, Rev9);         // Rev 9 consumer robot with sleep time
    // We instantiate our monitor object to pass to our threads to handle the producer consumer mechanism
    //  our monitor needs the max amount of requests that can be produced as as seating request and
    // the semaphore barrier for last consumer to signal
    Monitor *monitor = new Monitor(seatingReq, &semBarrier); // instantiate our monitor
    // We instantiate our arguments structures for our static functions in our producer and consumer class files
    // we need this in order to pass arguments and start our posix threads with the given functions
    // We use pointers for our argument objects our monitor pointer and our producer pointers with the needed attributes
    Producer::prodEntityArgs genProdArgs{&(*monitor), &(*genProd)}; // Start our general request argument structure
    Producer::prodEntityArgs vipProdArgs{&(*monitor), &(*vipProd)}; // Start our vip request arguments structure
    Consumer::consEntityArgs robTxArgs{&(*monitor), &(*robTx)};     // Start our TX robot argument structure
    Consumer::consEntityArgs robRevArgs{&(*monitor), &(*robRev)};   // Start our Rev9  argument structure
    // Create and start our producer and consumer threads with our pointer to our arguments and
    //  and the static class function
    pthread_create(&genProdThread, NULL, &Producer::produce, (void *)&genProdArgs); // Create general producer thread
    pthread_create(&vipProdThread, NULL, &Producer::produce, (void *)&vipProdArgs); // Create vip producer thread
    pthread_create(&robTxThread, NULL, &Consumer::consume, (void *)&robTxArgs);     // Create Tx robot consumer thread
    pthread_create(&robRevThread, NULL, &Consumer::consume, (void *)&robRevArgs);   // Create Rev 9 consumer thread
    // We join our producer thread mostly for the precedence constraint and the logging logging expects a
    // timing exit of the threads so sometimes if producers are still running when the signal is sent to
    // the barrier the expectations of the threads will mess the the output logging
    // This is also because by our constraint is that producers should finish first before the when reaching limit
    // this ensures this constraint and the semaphore barrier ensures the last consumer precedence constraint
    pthread_join(genProdThread, NULL); // Join general producer thread
    pthread_join(vipProdThread, NULL); // Join vip producer thread
    sem_wait(&semBarrier);             // Our main thread waits for the signal that the last consumer consumed the last request
    sem_destroy(&semBarrier);          // destroy semaphore to release resources
    return 0;
}