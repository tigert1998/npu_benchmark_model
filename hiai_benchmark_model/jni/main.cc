#include <exception>
#include <iostream>
#include <map>

#include "hw_ai_wrapper.h"
#include "command_line_flags.h"

using namespace std;

HwAiWrapper wrapper;

int main(int argc, char **argv) {
  std::string graph;

  if (!Flags::Parse(argc, argv, Flag<std::string>("graph", &graph))) {
    puts("Invalid arguments");
    return -1;
  }

  if (0 != wrapper.LoadModelFromFileSync("benchmark_model", graph, true)) {
    puts("Fail to load model");
    return -1;
  }
  return 0;
}
