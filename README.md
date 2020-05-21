# C_Client_Server

Based on [tcp-server-client-implementation-in-c](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)  

Data is read from *input_file.txt*  

Log is written to *message_log.txt*

## How to build

```shell script
rm -r build
mkdir build
cd build

cmake .. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .

nohup ./server &
./client
```

Or int run
```shell script
sh run.sh
```

## How to use 

- to run server
```shell script
nohup ./server &
```

- to run client
```shell script
./client
```

## Notes
- one client - read data from file - input_file.txt
- on server - write message from client into file - message log
