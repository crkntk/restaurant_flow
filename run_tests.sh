#!/bin/bash

./dineseating

echo -e "FINISHED TEST \n\n\n"
./dineseating -v 1 -g 2 -x 3 -r 4 -p vip_priority
echo -e "FINISHED TEST \n\n\n"

./dineseating -v 2 -g 4 -x 1 -r 5 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 2 -g 3 -x 1 -r 1 -p vip_priority 
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 2 -g 1 -x 3 -r 3 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 1 -g 2 -x 30 -r 30 -p vip_priority
echo -e "FINISHED TEST \n\n\n"

./dineseating -s 75 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -s 150 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -s 110 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 3 -g 9 -x 37 -r 40 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 0 -g 2 -x 5 -r 4 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 2000 -g 1 -x 3 -r 7 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -v 1 -g 1 -x 6 -r 6 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -s 5 -v 10 -g 25 -x 2 -r 3 -p vip_priority
echo -e "FINISHED TEST \n\n\n"
./dineseating -s 5 -p vip_priority
echo -e "FINISHED TEST \n\n\n"