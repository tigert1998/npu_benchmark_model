#include <chrono>
#include <iostream>
#include <map>
#include <thread>

#include "command_line_flags.h"
#include "hw_ai_wrapper.h"
#include "stat.h"

using std::cout;

HwAiWrapper wrapper;

void Benchmark(const std::string &offline_model_name, int32_t num_runs,
               float run_delay, bool is_warmup) {
  Stat<double> stat;
  printf("Running benchmark for %d iterations%s.\n", num_runs,
         is_warmup ? " in warmup stage" : "");
  std::vector<std::vector<float>> data_buf(1);
  data_buf[0].resize(3 * 299 * 299);
  for (int i = 0; i < num_runs; i++) {
    if (run_delay > 0 && !is_warmup) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds((int32_t)run_delay * 1000));
    }
    auto res = wrapper.RunModelSync(offline_model_name, data_buf);
    stat.UpdateStat(res->time_ms);
  }
  cout << stat << "\n\n";
}

int main(int argc, char **argv) {
  std::string offline_model_name, offline_model_path;
  std::string online_model_path, online_model_parameter;
  std::string framework = "tensorflow";
  bool mix_flag;
  int32_t num_runs = 50, warmup_runs = 1;
  float run_delay = -1;

  if (!Flags::Parse(
          argc, argv,
          Flag<std::string>("offline_model_name", &offline_model_name),
          Flag<std::string>("offline_model_path", &offline_model_path),
          Flag<std::string>("online_model_path", &online_model_path),
          Flag<std::string>("online_model_parameter", &online_model_parameter),
          Flag<std::string>("framework", &framework),
          Flag<bool>("mix_flag", &mix_flag),
          Flag<int32_t>("num_runs", &num_runs),
          Flag<int32_t>("warmup_runs", &warmup_runs),
          Flag<float>("run_delay", &run_delay,
                      "delay between runs in seconds"))) {
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

  if (0 != wrapper.LoadModelFromFileSync(offline_model_name, offline_model_path,
                                         mix_flag)) {
    puts("[ERROR] fail to load model");
    return -1;
  }
  puts("[INFO] load successfully");

  Benchmark(offline_model_name, warmup_runs, run_delay, true);
  Benchmark(offline_model_name, num_runs, run_delay, false);
  return 0;
}
