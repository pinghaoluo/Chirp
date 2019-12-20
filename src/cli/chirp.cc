#include <iostream>

#include <gflags/gflags.h>

#include "cli_parser.h"
using namespace std;
// Main for Chirp App
// Runs CLI for client. service_layer_server and data_store_server must already
// be running
int main(int argc, char** argv) {
  CliParser parser;
  std::cout << parser.Parse(argc, argv) << std::endl;
  return 0;
}
