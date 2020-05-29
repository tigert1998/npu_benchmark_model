# HIAI benchmark model

## Environment

Tested under:

- host machine: ubuntu 18.04/Windows 10
- mobile: huawei mate 10 pro
- android: EMUI 9 (android 9)

## Build

1. `ndk-build -C hiai_benchmark_model`

2. ```
   cd hiai_benchmark_model/obj/local/arm64-v8a/
   adb push hiai_benchmark_model ${MOBILE_LOCATION}
   adb push libhiai.so ${MOBILE_LOCATION}
   adb push libtensorflow_adapter.so ${MOBILE_LOCATION}
   adb push libtensorflow_inference.so ${MOBILE_LOCATION}
   ```

## Model Conversion

### TensorFlow

- model generation required TF version: 1.4.0

- model format: `.pb` (freeze variables to constant before writing)

- huawei parameter file format (need to be generated additionally and manually):

  ```
  session_run{
    input_nodes(1):
    "${input_op_name}",${input_tensor_shape}
    output_nodes(1):
    "${output_op_name}"
  }
  ```

  e.g.

  ```
  session_run{
    input_nodes(1):
    "input",1,224,224,3
    output_nodes(1):
    "conv2d/BiasAdd"
  }
  ```

### Caution

- Parameter file should be in UNIX format which means instead of `\x0D\x0A`, it must use `\x0A` or huawei conversion tool will crash.
- Model conversion will be automatically finished at the beginning of `hiai_benchmark_model`. Therefore, you only need to take care of your `pb` and `txt` file.

## Benchmark

`./hiai_benchmark_model --help`

### Remark

- `online_model_path` refers to `*.pb`.
- `online_model_parameter` refers to the manually  generated `txt` parameter file along with `*.pb`.
- `hiai_benchmark_model` will convert the online model to Cambricon NPU format and dump the model to `offline_model_path`.