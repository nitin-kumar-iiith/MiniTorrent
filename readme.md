to run tracker
./tracker tracker_info.txt 1
to run client
./client 127.0.0.1:5462 tracker_info.txt
to compile
g++ -pthread "client.cpp" -o client -pthread -lcrypto;
g++ -pthread "tracker.cpp" -o tracker -pthread -lcrypto
