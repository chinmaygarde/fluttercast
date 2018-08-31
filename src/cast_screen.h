
#pragma once

#include <rfb/rfb.h>

#include <string>

#include "macros.h"

namespace cast {

class CastScreen {
 public:
  CastScreen(std::string name, size_t width, size_t height);

  ~CastScreen();

  bool IsValid() const;

  bool Run();

 private:
  bool valid_ = false;
  std::string screen_name_;
  const int screen_width_;
  const int screen_height_;
  const int screen_bytes_per_pixel_;
  char* screen_framebuffer_ = nullptr;
  rfbScreenInfoPtr screen_info_ = nullptr;

  CAST_DISALLOW_COPY_AND_ASSIGN(CastScreen);
};

}  // namespace cast
