
#include "flutter_application.h"

namespace cast {

static_assert(FLUTTER_ENGINE_VERSION == 1, "");

FlutterApplication::FlutterApplication() {
  FlutterRendererConfig config = {};
  config.type = kOpenGL;
  config.open_gl.struct_size = sizeof(config.open_gl);
  config.open_gl.make_current = [](void* userdata) -> bool { return true; };
  config.open_gl.clear_current = [](void*) -> bool { return true; };
  config.open_gl.present = [](void* userdata) -> bool { return true; };
  config.open_gl.fbo_callback = [](void*) -> uint32_t { return 0; };

#define MY_PROJECT \
  "/home/buzzy/VersionControlled/flutter/examples/flutter_gallery"

  FlutterProjectArgs args = {
      .struct_size = sizeof(FlutterProjectArgs),
      .assets_path = MY_PROJECT "/build/flutter_assets",
      .main_path = "",
      .packages_path = "",
      .icu_data_path =
          "/home/buzzy/VersionControlled/engine/src/out/host_debug/icudtl.dat",
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

}  // namespace cast
