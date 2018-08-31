
#include "cast_screen.h"

#include <stdlib.h>

namespace cast {

CastScreen::CastScreen(std::string name, size_t width, size_t height)
    : screen_name_(std::move(name)),
      screen_width_(width),
      screen_height_(height),
      screen_bytes_per_pixel_(4) {
  if (screen_width_ == 0 || screen_height_ == 0) {
    return;
  }

  screen_framebuffer_ = reinterpret_cast<char*>(
      ::malloc(screen_width_ * screen_height_ * screen_bytes_per_pixel_));

  if (screen_framebuffer_ == nullptr) {
    return;
  }

  int rfb_argc = 0;
  char* rfb_argv[] = {};

  screen_info_ = rfbGetScreen(&rfb_argc,               //
                              rfb_argv,                //
                              screen_width_,           //
                              screen_height_,          //
                              8,                       // bitsPerSample
                              3,                       // samplesPerPixel
                              screen_bytes_per_pixel_  // bytesPerPixel
  );

  screen_info_->desktopName = screen_name_.c_str();
  screen_info_->frameBuffer = screen_framebuffer_;

  rfbInitServer(screen_info_);

  valid_ = true;
}

CastScreen::~CastScreen() {
  rfbShutdownServer(screen_info_, true /* disconnect clients */);

  rfbScreenCleanup(screen_info_);

  free(screen_framebuffer_);
}

bool CastScreen::IsValid() const {
  return valid_;
}

bool CastScreen::Run() {
  if (!valid_) {
    return false;
  }

  while (rfbIsActive(screen_info_)) {
    rfbProcessEvents(screen_info_, screen_info_->deferUpdateTime * 1000);
  }

  return true;
}

}  // namespace cast
