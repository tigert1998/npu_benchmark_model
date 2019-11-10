#include <iostream>
#include <string>

#include "command_line_flags.h"

int main(int argc, char **argv) {
  std::string model_path = "";
  bool debug_flag = false, enable_op_profiling = true;
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

  return 0;
}