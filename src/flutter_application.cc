
#include "flutter_application.h"

namespace cast {

static_assert(FLUTTER_ENGINE_VERSION == 1, "");

FlutterApplication::FlutterApplication() {
  FlutterRendererConfig config = {};
  config.type = kSoftware;
  config.software.struct_size = sizeof(FlutterSoftwareRendererConfig);
  config.software.surface_present_callback =
      &FlutterApplication::PresentSurface;

#define MY_PROJECT \
  "/home/buzzy/VersionControlled/flutter/examples/flutter_gallery"

  FlutterProjectArgs args = {
      .struct_size = sizeof(FlutterProjectArgs),
      .assets_path = MY_PROJECT "/build/flutter_assets",
      .main_path = "",
      .packages_path = "",
      .icu_data_path =
          "/home/buzzy/VersionControlled/engine/src/out/host_debug_unopt/"
          "icudtl.dat",
  };

  FlutterEngine engine = nullptr;
  auto result = FlutterEngineRun(FLUTTER_ENGINE_VERSION, &config,  // renderer
                                 &args, this, &engine_);

  if (result != kSuccess) {
    CAST_ERROR << "Could not run the Flutter engine" << std::endl;
    return;
  }

  valid_ = true;
}

FlutterApplication::~FlutterApplication() {
  if (engine_ == nullptr) {
    return;
  }

  auto result = FlutterEngineShutdown(engine_);

  if (result != kSuccess) {
    CAST_ERROR << "Could not shutdown the Flutter engine." << std::endl;
  }
}

bool FlutterApplication::IsValid() const {
  return valid_;
}

bool FlutterApplication::SetWindowSize(size_t width, size_t height) {
  FlutterWindowMetricsEvent event = {};
  event.struct_size = sizeof(event);
  event.width = width;
  event.height = height;
  event.pixel_ratio = 1.0;
  return FlutterEngineSendWindowMetricsEvent(engine_, &event) == kSuccess;
}

void FlutterApplication::ProcessEvents() {
  __FlutterEngineFlushPendingTasksNow();
}

bool FlutterApplication::PresentSurface(void* user_data,
                                        const void* allocation,
                                        size_t row_bytes,
                                        size_t height) {
  return reinterpret_cast<FlutterApplication*>(user_data)->PresentSurface(
      allocation, row_bytes, height);
}

void FlutterApplication::SetOnPresentCallback(PresentCallback callback) {
  std::lock_guard<std::mutex> lock(mutex_);
  present_callback_ = callback;
}

bool FlutterApplication::PresentSurface(const void* allocation,
                                        size_t row_bytes,
                                        size_t height) {
  std::lock_guard<std::mutex> lock(mutex_);
  if (!present_callback_) {
    CAST_ERROR << "Present callback was not set." << std::endl;
    return false;
  }
  present_callback_(allocation, row_bytes, height);
  return true;
}

}  // namespace cast
