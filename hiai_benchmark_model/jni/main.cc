#include <exception>
#include <iostream>
#include <map>

#include "command_line_flags.h"
#include "hw_ai_wrapper.h"

using namespace std;

HwAiWrapper wrapper;

int main(int argc, char **argv) {
  std::string offline_model_name, offline_model_path;
  std::string online_model_path, online_model_parameter;
  std::string framework = "tensorflow";
  bool mix_flag;

  if (!Flags::Parse(
          argc, argv,
          Flag<std::string>("offline_model_name", &offline_model_name),
          Flag<std::string>("offline_model_path", &offline_model_path),
          Flag<std::string>("online_model_path", &online_model_path),
          Flag<std::string>("online_model_parameter", &online_model_parameter),
          Flag<std::string>("framework", &framework),
          Flag<bool>("mix_flag", &mix_flag))) {
    puts("[ERROR] invalid arguments");
    return -1;
  }

  bool use_npu = wrapper.ModelCompatibilityProcessFromFile(
      online_model_path, online_model_parameter, framework, offline_model_path,
      mix_flag);
  printf("use_npu = %s\n", ToString(mix_flag).c_str());
  if (!use_npu) {
    puts(
        "[ERROR] model incompatible, no online compiler interface or fail to "
        "compile model");
    return -1;
  }

  if (0 != wrapper.LoadModelFromFileSync(offline_model_name, online_model_path,
                                         true)) {
    puts("[ERROR] fail to load model");
    return -1;
  }
  return 0;
}
