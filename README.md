# FileTransfer

Client-server apps to transfer file. Server is a daemon

## Requirements

- [Boost 1.71.0](https://www.boost.org/users/history/version_1_71_0.html)
- C++17

## Running

```
server <port> <working directory>
```
```
client <ip-address> <port> <file path>
```
The File will be saved in the "working directory"