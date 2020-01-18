#include <stdint.h>
#include <string>

void LatencyTest(const std::string &model_path, bool debug_flag,
                 int32_t warmup_runs, double warmup_min_secs, int32_t num_runs,
                 double min_secs, double max_secs, double run_delay,
                 bool enable_op_profiling,
                 const std::string &op_profiling_dump_path);