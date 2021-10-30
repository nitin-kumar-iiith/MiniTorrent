#!/bin/bash
g++ -pthread "server"$1".cpp" -o server;
g++ -pthread "client"$1".cpp" -o client;
g++ -pthread "tracker"$1".cpp" -o tracker
