#!/bin/bash
if [[ $1 == "-k" ]] ; then
	echo 'Socket closing!'
	sudo fuser -k -n tcp 9734
	pkill -9 server
else
	echo 'Compiling!'
	gcc server.c -o server -lm
	gcc client.c -o client -lpthread
fi
echo 'All operations performed'
