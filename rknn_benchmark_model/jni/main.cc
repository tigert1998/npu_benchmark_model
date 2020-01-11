#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include "command_line_flags.h"
#include "rknn_api_wrapper.h"
#include "stat.h"
#include "util.h"

using std::cout;

void Benchmark(const RknnApiWrapper &rknn_api_wrapper, int32_t num_runs,
               float min_secs, float max_secs, float run_delay, bool is_warmup,
               bool enable_op_profiling,
               const std::string &op_profiling_dump_path) {
  Stat<double> stat;
  printf("Running benchmark for around %d iterations%s.\n", num_runs,
         is_warmup ? " in warmup stage" : "");
  fflush(stdout);
  auto data_buf = rknn_api_wrapper.GenerateCnnRandomInput();
  decltype(data_buf) output_buf;

  auto from_time = std::chrono::high_resolution_clock::now();
  decltype(from_time) to_time = from_time;

  PerfDetailTable perf_detail;

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
    if (!is_warmup && enable_op_profiling) {
      if (i == 0) {
        perf_detail = res.perf_detail;
      } else {
        perf_detail.Merge(res.perf_detail);
      }
    }

    to_time = std::chrono::high_resolution_clock::now();
    if (to_time - from_time >=
        std::chrono::milliseconds((int)(max_secs * 1000)))
      break;
  }

  if (!is_warmup && enable_op_profiling) {
    if (op_profiling_dump_path != "") {
      std::ofstream ofs(op_profiling_dump_path);
      if (ofs.is_open())
        ofs << perf_detail;
      else {
        printf("[WARNING] cannot open %s\n", op_profiling_dump_path.c_str());
      }
    } else {
      cout << perf_detail;
    }
  }
  cout << stat << "\n\n";
}

int main(int argc, char **argv) {
  std::string model_path = "", op_profiling_dump_path = "";
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
                     "enable op profiling"),
          Flag<std::string>("op_profiling_dump_path",
                            &op_profiling_dump_path))) {
    puts("[ERROR] invalid arguments");
    return -1;
  }

  do {
    try {
      std::unique_ptr<RknnApiWrapper> rknn_api_wrapper;
      Timeout(
          [&]() {
            rknn_api_wrapper = std::make_unique<RknnApiWrapper>(
                model_path, enable_op_profiling, debug_flag);
          },
          3 * 60);

      Benchmark(*rknn_api_wrapper, warmup_runs, warmup_min_secs,
                std::numeric_limits<float>::max(), -1, true,
                enable_op_profiling, op_profiling_dump_path);
      Benchmark(*rknn_api_wrapper, num_runs, min_secs, max_secs, run_delay,
                false, enable_op_profiling, op_profiling_dump_path);
    } catch (std::runtime_error error) {
      std::cerr << "[ERROR] " << error.what() << std::endl;
      return -1;
    } catch (RknnError error) {
      std::cerr << "[ERROR] " << error.what() << std::endl;
      if (error.code() == RKNN_ERR_DEVICE_UNAVAILABLE) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cerr << "[WARN] Retrying..." << std::endl;
        continue;
      } else {
        return -1;
      }
    }
    break;
  } while (1);
  return 0;
}