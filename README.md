# QuayLog
A simple kernel mode key logger written as an introduction into Linux kernel module development. This code has been written entirely for educational purposes and should never be used on a live system as it will record all keypress events!

## Building
Install linux headers.
```
apt-get install linux-headers-$(uname -r)
```
Build the module
```
make all
```
