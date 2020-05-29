# NPU Benchmark model

Encapsulate several NPU libraries' API into a benchmark tool in order to test NPU inference speed.
This repo contains the code for RKNN 1.2.0 and HIAI DDK 150.

Refer to [RKNN Readme](rknn.md) and [HIAI Readme](hiai.md) for more details.

## Clone

1. `git lfs pull` (in order to pull shared libraries stored with  `git-lfs`)

2. `git submodule update --init --recursive` to clone submodules.

3. Configure android sdk, ndk and `ndk-build` which is directly in the folder of ndk
   - Linux: ndk is located where you installed it
   - Windows: ndk usually lies in `~\AppData\Local\Android\Sdk\ndk-bundle`
