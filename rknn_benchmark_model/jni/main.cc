#include <exception>
#include <iostream>
#include <string>
#include <thread>

#include "command_line_flags.h"
#include "rknn_api_aux.h"
#include "rknn_api_wrapper.h"
#include "stat.h"

using std::cout;

void Benchmark(const RknnApiWrapper &rknn_api_wrapper, int32_t num_runs,
               float min_secs, float max_secs, float run_delay, bool is_warmup,
               bool enable_op_profiling) {
  Stat<double> stat;
  printf("Running benchmark for around %d iterations%s.\n", num_runs,
         is_warmup ? " in warmup stage" : "");
  auto data_buf = rknn_api_wrapper.GenerateCnnRandomInput();
  decltype(data_buf) output_buf;

  auto from_time = std::chrono::high_resolution_clock::now();
  decltype(from_time) to_time = from_time;

  std::string perf_detail;

  for (int i = 0;
       i < num_runs ||
       to_time - from_time < std::chrono::milliseconds((int)(min_secs * 1000));
       i++) {
    if (run_delay > 0 && !is_warmup) {
      std::this_thread::sleep_for(
          std::chrono::milliseconds((int32_t)(run_delay * 1000)));
    }
    auto res = rknn_api_wrapper.Run(data_buf);
    stat.UpdateStat(res.time_ms);
    if (i == 0 && enable_op_profiling) perf_detail = res.perf_detail;

    to_time = std::chrono::high_resolution_clock::now();
    if (to_time - from_time >=
        std::chrono::milliseconds((int)(max_secs * 1000)))
      break;
  }

  if (!is_warmup && enable_op_profiling) {
    cout << PerfDetailTable(perf_detail);
  }
  cout << stat << "\n\n";
}

int main(int argc, char **argv) {
  std::string model_path = "";
  bool debug_flag = false, enable_op_profiling = false;
  int32_t num_runs = 50, warmup_runs = 1;
  float min_secs = 1, max_secs = 150, run_delay = -1, warmup_min_secs = 0.5;

  if (!Flags::Parse(
          argc, argv, Flag<std::string>("model_path", &model_path),
          Flag<bool>("debug_flag", &debug_flag),
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
                      "than warmup_runs"),
          Flag<bool>("enable_op_profiling", &enable_op_profiling,
                     "enable op profiling"))) {
    puts("[ERROR] invalid arguments");
    return -1;
  }

  try {
    RknnApiWrapper rknn_api_wrapper(model_path, enable_op_profiling,
                                    debug_flag);

    Benchmark(rknn_api_wrapper, warmup_runs, warmup_min_secs,
              std::numeric_limits<float>::max(), -1, true, enable_op_profiling);
    Benchmark(rknn_api_wrapper, num_runs, min_secs, max_secs, run_delay, false,
              enable_op_profiling);
  } catch (std::runtime_error error) {
    std::cout << "[ERROR] " << error.what() << std::endl;
    return -1;
  }
  return 0;
}