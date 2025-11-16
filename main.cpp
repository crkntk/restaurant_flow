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

    int seatingReq = 110;
    int sleepTX = 0;
    int sleepRev9 = 0;
    int sleepGen = 0;
    int sleepVip = 0;

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
            // Number of addresses to process
            seatingReq = atoi(optarg);
            break;
        case 'x':
            sleepTX = atoi(optarg); // Number of available frames
            break;
        case 'r':
            sleepRev9 = atoi(optarg); // Interval extraction
            break;
        case 'g':
            sleepGen = atoi(optarg); // Get logging string
            break;
        case 'v':
            sleepVip = atoi(optarg); // Get logging string
            break;
        default:
            break;
        }
    }
    sem_t semBarrier;
    sem_init(&semBarrier, 0, 0);
    pthread_t genProdThread, vipProdThread;
    pthread_t robTxThread, robRevThread;

    Producer *genProd = new Producer(sleepGen, GeneralTable);
    Producer *vipProd = new Producer(sleepVip, VIPRoom);
    Consumer *robTx = new Consumer(sleepTX, TX);
    Consumer *robRev = new Consumer(sleepRev9, Rev9);
    Monitor *monitor = new Monitor(seatingReq, &semBarrier);
    entityProdArgs genProdArgs{&(*monitor), &(*genProd)};
    entityProdArgs vipProdArgs{&(*monitor), &(*vipProd)};
    entityConsArgs robTxArgs{&(*monitor), &(*robTx)};
    entityConsArgs robRevArgs{&(*monitor), &(*robRev)};
    pthread_create(&genProdThread, NULL, &Producer::produce, (void *)&genProdArgs);
    pthread_create(&vipProdThread, NULL, &Producer::produce, (void *)&vipProdArgs);
    pthread_create(&robTxThread, NULL, &Consumer::consume, (void *)&robTxArgs);
    pthread_create(&robRevThread, NULL, &Consumer::consume, (void *)&robRevArgs);
    pthread_join(genProdThread,NULL);
    pthread_join(vipProdThread,NULL);
    sem_wait(&semBarrier);
    pthread_join(robTxThread,NULL);
    pthread_join(robRevThread,NULL);

    sem_destroy(&semBarrier);
    return 0;
}