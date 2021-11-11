#!/bin/bash
#echo "Compiling: server"$1".cpp"
#g++ -pthread "server"$1".cpp" -o server;
echo "Compiling: client.cpp"
g++ -pthread "client.cpp" -o client -pthread -lcrypto;
echo "Compiling: tracker.cpp"
g++ -pthread "tracker.cpp" -o tracker -pthread -lcrypto
