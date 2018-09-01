
#pragma once

#include <flutter_embedder.h>

#include <functional>
#include <mutex>

#include "macros.h"

namespace cast {

class FlutterApplication {
 public:
  FlutterApplication();

  ~FlutterApplication();

  bool IsValid() const;

  void ProcessEvents();

  bool SetWindowSize(size_t width, size_t height);

  using PresentCallback = std::function<
      void(const void* allocation, size_t row_bytes, size_t height)>;
  void SetOnPresentCallback(PresentCallback callback);

 private:
  bool valid_;
  FlutterEngine engine_ = nullptr;
  std::mutex mutex_;
  PresentCallback present_callback_;

  static bool PresentSurface(void* user_data,
                             const void* allocation,
                             size_t row_bytes,
                             size_t height);

  bool PresentSurface(const void* allocation, size_t row_bytes, size_t height);

  CAST_DISALLOW_COPY_AND_ASSIGN(FlutterApplication);
};

}  // namespace cast
