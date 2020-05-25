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

./server
./client "input_file.txt"
```

# How to generate big file
For example 

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
- [x] ~~try to send very big file - with size near 1 ml bytes~~
- [x] ~~serverc 63 не засовывается из text в buff~~
- [x] ~~add support for client argv[] - input file name~~  
- [x] ~~upscrew (turn on) demonizing~~
- [ ] signal handling stops only on the second client call - how to fix it? 
\- think that change position on signal handling part

