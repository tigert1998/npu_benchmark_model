# RKNN Benchmark Model

## Build

1. `ndk-build -C rknn_benchmark_model`

2. ```
   cd rknn_benchmark_model/obj/local/arm64-v8a
   adb push rknn_benchmark_model ${MOBILE_LOCATION}
   adb push librknn_api.so ${MOBILE_LOCATION}
   ```

## Usage and Parameters

`./rknn_benchmark_model --help`

- `model_path`: The model path to the RK model (ended with `.rknn`).

- `benchmark_type`: The field must be `accuracy` or `latency`.
`accuracy` is not implemented yet so don't use it.

- `debug_flag`: Verbose debug information will printed if set as `true`.

- `disable_timeout`: `bool`. When timeout is enabled,
the tool will force to quit if `rknn_init` takes an extremely long time.

- `num_runs`: `int`. Number of runs in a latency benchmark.

- `enable_op_profiling`: Detailed operator latency table will be given if set `true`.

- `op_profiling_dump_path`: This field works only if `enable_op_profiling` is set to `true`.
If it is set as a path, a csv format operator latency table will be stored there.
Otherwise the table will be directly printed to `stdout`.

