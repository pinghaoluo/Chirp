Chirp Project

Name: Pinghao Luo
Email: pinghaol@usc.edu


VAGRANT SETUP
Install vagrant as according to https://www.vagrantup.com/
Setup Vagrant box: https://app.vagrantup.com/bento/boxes/ubuntu-18.04

DEPENDENCIES
1. Update apt-get: `sudo apt-get update`
2. Install g++: `sudo apt-get install g++`
3. Install cmake: `sudo apt-get install cmake`
4. Install prereqs: `sudo apt-get install build-essential autoconf libtool pkg-config`
5. Install gtests & gflags: `sudo apt-get install libgflags-dev libgtest-dev`
   a. `cd /usr/src/gtest`
   b. `sudo cmake CMakeLists.txt`
   c. `sudo make`
   d. `sudo cp *.a /usr/lib`
6. Install clang: `sudo apt-get install clang libc++-dev`

GRPC INSTALL
1. $ `git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc`
   $ `cd grpc`
   $ `git submodule update --init`
2. Install gRPC
   a. `sudo make`
   b. `sudo make install`
   c. `cd grpc/third_party/protobuf`
   d. `sudo make install`
3. In grpc root folder: `sudo make install`

PROJECT SETUP
1. `cd to root folder of project (499-allenyhu)`
2. `cd src`
3. `make data_store.grpc.pb.cc data_store.pb.cc`
4. `mv data_store.* store`
5. `make service_layer.grpc.pb.cc service_layer.pb.cc`
6. `mv service_layer.* service`

RUNNING PROJECT
1. To make all executables & tests: `make`

You can also make executables individually:
1. `make chirp`
2. `make service_layer_server`
3. `make data_store_server`

Be sure to run service_layer_server and data_store_server before using cli: `./service_layer_server` and `./data_store_server`


USING CLI
To Register User: `./chirp -—register <username>`

To Chirp: `./chirp -—user <username> -—chirp <chirp text> -—reply <chirp id>`
  - reply is optional
  
To Follow: `./chirp -—user <username> -—follow <username>`

To Read: `./chirp -—read <chirp id>`

To Monitor: `./chirp -—user <username> -—monitor`

To Stream: `./chirp -—user <username> -—stream <hashtag>`

***Commands will not work together (must do one at a time)***

TESTING
Service_Layer: service/service_layer_tests.cc
- `make service_tests`
- `./service_tests`

Data_Store: store/data_store_tests.cc
- `make ds_tests`
- `./ds_tests`

No tests for cli because would be virtually pointless without GRPC 

CLEANING
1. `make clean`: removes all executables but will not remove proto generated files
