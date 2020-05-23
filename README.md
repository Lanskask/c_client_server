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

# How to generate big file
Content of *file.txt*
```shell script
hello world
```
```shell script
for i in {1..15}; do cat file.txt file.txt > file2.txt && mv file2.txt file.txt; done
```


## Notes
- one client - read data from file - input_file.txt
- on server - write message from client into file - message log

## TODO:
- [x] ~~read all file content - not just first line~~
- [x] ~~demonize process~~
- [x] ~~check demonizing~~
- [ ] upscrew (turn on) demonizing 
- [ ] ~~try to send very big file - with size near 1 ml bytes~~
- [x] ~~serverc 63 не засовывается из text в buff~~