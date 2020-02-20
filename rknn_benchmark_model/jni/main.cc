#include <iostream>
#include <string>

#include "accuracy_test.h"
#include "command_line_flags.h"
#include "latency_test.h"
#include "util.h"

using std::cout;

int main(int argc, char **argv) {
  std::string model_path = "";
  std::string benchmark_type = "";
  bool debug_flag = false;

  // accuracy
  std::string ground_truth_images_path = "";
  std::string ground_truth_labels = "";
  std::string model_output_labels = "";
  std::string output_file_path = "";
  int32_t num_images = -1;
  int32_t num_ranks = 10;

  // latency
  int32_t num_runs = 50, warmup_runs = 1;
  float min_secs = 1, max_secs = 150, run_delay = -1, warmup_min_secs = 0.5;
  bool enable_op_profiling = false, disable_timeout = false;
  std::string op_profiling_dump_path = "";

  if (!Flags::Parse(
          argc, argv, Flag<std::string>("model_path", &model_path),
          Flag<std::string>("benchmark_type", &benchmark_type,
                            "accuracy or latency"),
          Flag<bool>("debug_flag", &debug_flag),
          Flag<bool>("disable_timeout", &disable_timeout),

          // accuracy
          Flag<std::string>(
              "ground_truth_images_path", &ground_truth_images_path,
              "The path to the directory containing ground truth images."),
          Flag<std::string>(
              "ground_truth_labels", &ground_truth_labels,
              "Path to ground truth labels file. This file should contain the "
              "same number of labels as the number images in the ground truth "
              "directory. The labels are assumed to be in the same order as "
              "the sorted filename of images. See ground truth label "
              "generation section for more information about how to generate "
              "labels for images."),
          Flag<std::string>(
              "model_output_labels", &model_output_labels,
              "Path to the file containing labels, that is used to interpret "
              "the output of the model. E.g. in case of mobilenets, this is "
              "the path to mobilenet_labels.txt where each label is in the "
              "same order as the output 1001 dimension tensor."),
          Flag<std::string>(
              "output_file_path", &output_file_path,
              "This is the path to the output file. The output is a CSV file "
              "that has top-10 accuracies in each row. Each line of output "
              "file is the cumulative accuracy after processing images in a "
              "sorted order. So first line is accuracy after processing the "
              "first image, second line is accuracy after processing first two "
              "images. The last line of the file is accuracy after processing "
              "the entire validation set."),
          Flag<int32_t>("num_images", &num_images,
                        "The number of images to process, if 0, all images in "
                        "the directory are processed otherwise only num_images "
                        "will be processed."),
          Flag<int32_t>(
              "num_ranks", &num_ranks,
              "The number of top-K accuracies to return. For example, if "
              "num_ranks=5, top-1 to top-5 accuracy fractions are returned."),

          // latency
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

  ASSERT(benchmark_type == "latency" || benchmark_type == "accuracy");

  if (benchmark_type == "accuracy") {
    AccuracyTest(model_path, debug_flag, ground_truth_images_path,
                 ground_truth_labels, model_output_labels, output_file_path,
                 num_images, num_ranks);
  } else if (benchmark_type == "latency") {
    LatencyTest(model_path, debug_flag, disable_timeout, warmup_runs,
                warmup_min_secs, num_runs, min_secs, max_secs, run_delay,
                enable_op_profiling, op_profiling_dump_path);
  }

  return 0;
}