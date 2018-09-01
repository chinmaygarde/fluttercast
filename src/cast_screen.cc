
#include "cast_screen.h"

#include <stdlib.h>

namespace cast {

CastScreen::CastScreen(std::unique_ptr<FlutterApplication> application,
                       std::string name,
                       size_t width,
                       size_t height)
    : application_(std::move(application)),
      screen_name_(std::move(name)),
      screen_width_(width),
      screen_height_(height),
      screen_bytes_per_pixel_(4) {
  if (!application_ || !application_->IsValid()) {
    CAST_ERROR << "Invalid application to run." << std::endl;
    return;
  }

  if (screen_width_ == 0 || screen_height_ == 0) {
    CAST_ERROR << "Invalid screen dimensions." << std::endl;
    return;
  }

  screen_framebuffer_ = reinterpret_cast<char*>(
      ::malloc(screen_width_ * screen_height_ * screen_bytes_per_pixel_));

  if (screen_framebuffer_ == nullptr) {
    CAST_ERROR << "Could not allocate framebuffer." << std::endl;
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

  application_->SetOnPresentCallback(
      std::bind(&CastScreen::OnApplicationDidPresent,  //
                this,                                  //
                std::placeholders::_1,                 // allocation
                std::placeholders::_2,                 // row bytes
                std::placeholders::_3)                 // height
  );

  if (!application_->SetWindowSize(width, height)) {
    CAST_ERROR << "Could not set the application window size." << std::endl;
    return;
  }

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
    CAST_ERROR << "Could not run an invalid screen.";
    return false;
  }

  while (rfbIsActive(screen_info_)) {
    rfbProcessEvents(screen_info_, screen_info_->deferUpdateTime * 1000);
  }

  return true;
}

void CastScreen::OnApplicationDidPresent(const void* allocation,
                                         size_t row_bytes,
                                         size_t height) {
  if (!valid_) {
    CAST_ERROR << "Tried to present on an invalid screen." << std::endl;
    return;
  }

  if (allocation == nullptr || row_bytes == 0 || height == 0) {
    CAST_ERROR << "Invalid backing store to present." << std::endl;
    return;
  }

  // Do some basic sanity checks to ensure that the framebuffer and the backing
  // store sizes match.
  if ((row_bytes * height) !=
      (screen_width_ * screen_height_ * screen_bytes_per_pixel_)) {
    CAST_ERROR << "Framebuffer and Flutter rasterizer backing store dimensions "
                  "do not match."
               << std::endl;
    return;
  }

  memmove(screen_framebuffer_, allocation, row_bytes * height);
}

}  // namespace cast
