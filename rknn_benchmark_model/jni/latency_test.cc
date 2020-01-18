#include "latency_test.h"

#include <chrono>
#include <fstream>
#include <memory>
#include <thread>

#include "rknn_api_wrapper.h"
#include "stat.h"
#include "util.h"

using std::cout;

namespace {
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
}  // namespace

void LatencyTest(const std::string &model_path, bool debug_flag,
                 int32_t warmup_runs, double warmup_min_secs, int32_t num_runs,
                 double min_secs, double max_secs, double run_delay,
                 bool enable_op_profiling,
                 const std::string &op_profiling_dump_path) {
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
      exit(-1);
    } catch (RknnError error) {
      std::cerr << "[ERROR] " << error.what() << std::endl;
      if (error.code() == RKNN_ERR_DEVICE_UNAVAILABLE) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cerr << "[WARN] Retrying..." << std::endl;
        continue;
      } else {
        exit(-1);
      }
    }
    break;
  } while (1);
}