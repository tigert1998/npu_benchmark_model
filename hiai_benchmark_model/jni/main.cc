#include <chrono>
#include <iostream>
#include <map>
#include <thread>

#include "command_line_flags.h"
#include "mix_model_manager_wrapper.h"
#include "stat.h"

using std::cout;

MixModelManagerWrapper wrapper;

void Benchmark(const std::string &offline_model_name, int32_t num_runs,
               float min_secs, float max_secs, float run_delay,
               bool is_warmup) {
  Stat<double> stat;
  printf("Running benchmark for around %d iterations%s.\n", num_runs,
         is_warmup ? " in warmup stage" : "");
  auto data_buf = wrapper.GenerateCnnRandomInput();

  auto from_time = std::chrono::high_resolution_clock::now();
  decltype(from_time) to_time = from_time;

  for (int i = 0;
       i < num_runs ||
       to_time - from_time < std::chrono::milliseconds((int)(min_secs * 1000));
       i++) {
    if (run_delay > 0 && !is_warmup) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds((int32_t)(run_delay * 1000)));
    }
    auto res = wrapper.RunModelSync(offline_model_name, data_buf);
    stat.UpdateStat(res->time_ms);

    to_time = std::chrono::high_resolution_clock::now();
    if (to_time - from_time >=
        std::chrono::milliseconds((int)(max_secs * 1000)))
      break;
  }
  cout << stat << "\n\n";
}

int main(int argc, char **argv) {
  std::string offline_model_name = "benchmark_model_name", offline_model_path;
  std::string online_model_path, online_model_parameter;
  std::string framework = "tensorflow";
  bool mix_flag;
  int32_t num_runs = 50, warmup_runs = 1;
  float min_secs = 1, max_secs = 150, run_delay = -1, warmup_min_secs = 0.5;

  if (!Flags::Parse(
          argc, argv,
          Flag<std::string>("offline_model_name", &offline_model_name),
          Flag<std::string>("offline_model_path", &offline_model_path),
          Flag<std::string>("online_model_path", &online_model_path),
          Flag<std::string>("online_model_parameter", &online_model_parameter),
          Flag<std::string>("framework", &framework),
          Flag<bool>("mix_flag", &mix_flag),
          Flag<int32_t>("num_runs", &num_runs),
          Flag<float>(
              "min_secs", &min_secs,
              "minimum number of seconds to rerun for, potentially making the "
              "actual number of runs to be greater than num_runs"),
          Flag<float>("max_secs", &max_secs,
                      "maximum number of seconds to rerun for, potentially "
                      "making the actual number of runs to be less than "
                      "num_runs. Note if --max-secs is exceeded in the middle "
                      "of a run, the benchmark will continue to the end of the "
                      "run but will not start the next run."),
          Flag<float>("run_delay", &run_delay, "delay between runs in seconds"),
          Flag<int32_t>("warmup_runs", &warmup_runs),
          Flag<float>("warmup_min_secs", &warmup_min_secs,
                      "minimum number of seconds to rerun for, potentially "
                      "making the actual number of warm-up runs to be greater "
                      "than warmup_runs"))) {
    puts("[ERROR] invalid arguments");
    return -1;
  }

  printf("TF_Version() = %s\n", MixModelManagerWrapper::GetTfVersion().c_str());

  bool use_npu = wrapper.ModelCompatibilityProcessFromFile(
      online_model_path, online_model_parameter, framework, offline_model_path,
      mix_flag);
  printf("use_npu = %s\n", ToString(use_npu).c_str());
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
  puts("[INFO] model loaded successfully");

  Benchmark(offline_model_name, warmup_runs, warmup_min_secs,
            std::numeric_limits<float>::max(), -1, true);
  Benchmark(offline_model_name, num_runs, min_secs, max_secs, run_delay, false);
  return 0;
}
