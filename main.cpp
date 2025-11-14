#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
using namespace std;
int main(int argc, char **argv){

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
            sleepGen = atoi(optarg); // Get logging string
            break;
        default:
            break;
        }
    }

}