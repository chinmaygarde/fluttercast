
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

  screen_framebuffer_ = reinterpret_cast<uint8_t*>(
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
  screen_info_->frameBuffer = reinterpret_cast<char*>(screen_framebuffer_);
  screen_info_->screenData = this;
  screen_info_->neverShared = true;
  screen_info_->newClientHook = &CastScreen::OnClientAddedCallback;
  screen_info_->ptrAddEvent = &CastScreen::OnPointerEventCallback;

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
    application_->ProcessEvents();
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

  const uint8_t* allocation8 = static_cast<const uint8_t*>(allocation);

  for (size_t i = 0, count = row_bytes * height; i < count; i += 4) {
    screen_framebuffer_[i + 0] = allocation8[i + 2];
    screen_framebuffer_[i + 1] = allocation8[i + 1];
    screen_framebuffer_[i + 2] = allocation8[i + 0];
    screen_framebuffer_[i + 3] = 255;  // Unused Padding.
  }

  rfbMarkRectAsModified(screen_info_, 0, 0, screen_width_, screen_height_);
}

void CastScreen::OnPointerEventCallback(int buttonMask,
                                        int x,
                                        int y,
                                        struct _rfbClientRec* cl) {
  reinterpret_cast<CastScreen*>(cl->screen->screenData)
      ->OnPointerEvent(buttonMask, x, y, cl);
}

void CastScreen::OnPointerEvent(int buttonMask,
                                int x,
                                int y,
                                struct _rfbClientRec* cl) {
  if (!valid_) {
    CAST_ERROR << "Pointer events on in invalid screen." << std::endl;
    return;
  }

  if (!application_->SendPointerEvent(buttonMask, x, y)) {
    CAST_ERROR << "Could not send pointer events to application." << std::endl;
    return;
  }
}

rfbNewClientAction CastScreen::OnClientAddedCallback(struct _rfbClientRec* cl) {
  bool should_add =
      reinterpret_cast<CastScreen*>(cl->screen->screenData)->OnClientAdded();
  if (!should_add) {
    return RFB_CLIENT_REFUSE;
  }
  cl->clientGoneHook = &CastScreen::OnClientRemovedCallback;
  return RFB_CLIENT_ACCEPT;
}

bool CastScreen::OnClientAdded() {
  client_count_++;
  CAST_LOG << "Client Count: " << client_count_;
  return true;
}

void CastScreen::OnClientRemovedCallback(struct _rfbClientRec* cl) {
  reinterpret_cast<CastScreen*>(cl->screen->screenData)->OnClientRemoved();
}

void CastScreen::OnClientRemoved() {
  client_count_--;
  CAST_LOG << "Client Count: " << client_count_;
}

}  // namespace cast
