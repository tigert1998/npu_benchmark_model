#include <iostream>

#include "hw_ai_wrapper.h"

using namespace std;

HwAiWrapper wrapper;

int main() {
  std::string model_name, model_path;
  bool mix_flag;
  wrapper.LoadModelFromFileSync(model_name, model_path, mix_flag);
  return 0;
}
