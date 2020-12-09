# AKFTP System
## Designed by Kiera and Peter

#### About
This software is not intended for commercial use and is a proof of concept of developing your own basic FTP server client using the C program language and sockets. 

* This software will not work on any Windows enviroment and is soley built for the purpose of UNIX and UNIX like operating systems.

#### Dependencies
* json-c
* base64
#### Building
After cloning please ensure you have the submodules.
```
git submodule update --init --recursive --depth 1
```
Once you have the submodules its time to generate the build scripts...
```
cmake -S . -B build
```
Now change into the build directory and run the following command.
```
make myftp && make myftpd
```
#### Using
Ensure the FTP server (myftpd) is running before connecting with the client (myftp). 
