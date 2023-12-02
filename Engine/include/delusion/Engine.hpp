#pragma once

#include <cassert>
#include <iostream>

#include <webgpu.h>

[[nodiscard]] WGPUAdapter requestAdapter(WGPUInstance instance, WGPURequestAdapterOptions const *options);

[[nodiscard]] WGPUDevice requestDevice(WGPUAdapter adapter, WGPUDeviceDescriptor const *descriptor);
