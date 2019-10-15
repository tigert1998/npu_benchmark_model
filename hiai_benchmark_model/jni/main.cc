#include <exception>
#include <iostream>
#include <map>

#include "command_line_flags.h"
#include "hw_ai_wrapper.h"

using namespace std;

HwAiWrapper wrapper;

int main(int argc, char **argv) {
  std::string model_name, model_path;

  if (!Flags::Parse(argc, argv, Flag<std::string>("model_name", &model_name),
                    Flag<std::string>("model_path", &model_path))) {
    puts("[ERROR] invalid arguments");
    return -1;
  }

  if (0 != wrapper.LoadModelFromFileSync(model_name, model_path, true)) {
    puts("[ERROR] fail to load model");
    return -1;
  }
  return 0;
}
