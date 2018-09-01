
#pragma once

#include <rfb/rfb.h>

#include <memory>
#include <string>

#include "flutter_application.h"
#include "macros.h"

namespace cast {

class CastScreen {
 public:
  CastScreen(std::unique_ptr<FlutterApplication> application,
             std::string name,
             size_t width,
             size_t height);

  ~CastScreen();

  bool IsValid() const;

  bool Run();

 private:
  bool valid_ = false;
  std::unique_ptr<FlutterApplication> application_;
  std::string screen_name_;
  const int screen_width_;
  const int screen_height_;
  const int screen_bytes_per_pixel_;
  char* screen_framebuffer_ = nullptr;
  rfbScreenInfoPtr screen_info_ = nullptr;

  void OnApplicationDidPresent(const void* allocation,
                               size_t row_bytes,
                               size_t height);

  // rfbPtrAddEventProcPtr
  static void OnPointerEventCallback(int buttonMask,
                                     int x,
                                     int y,
                                     struct _rfbClientRec* cl);
  void OnPointerEvent(int buttonMask, int x, int y, struct _rfbClientRec* cl);

  CAST_DISALLOW_COPY_AND_ASSIGN(CastScreen);
};

}  // namespace cast
