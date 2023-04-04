# J(oe's)F(ile)T(ransfer)

Library I created to be used to send and recive files over a local network. Uses Boost ASIO to help with socket programing. Currently local network 
transfer speeds are arround 500 mb/s but working to improve that. The server is asynchronous in order to allow multiple clients to connect at the same 
time. The server will independantly hendle client requests and do so in a way that doesn't block other requests from coming in. I am currently working on 
implementing coroutines, which is another approach to deffering execution of tasks as to not block the main event thread.

## Install

To install and use this library
```
mkdir build && cd build
cmake ..
make
```

Currently this creates a static library and 2 test programs, (server and client) that if run will transfer the CMakeCache.txt file and assure that the 
files match.
