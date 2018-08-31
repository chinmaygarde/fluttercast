
#pragma once

#include <flutter_embedder.h>

#include "macros.h"

namespace cast {

class FlutterApplication {
 public:
  FlutterApplication();

  ~FlutterApplication();

  bool IsValid() const;

 private:
  bool valid_;
  FlutterEngine engine_ = nullptr;

  CAST_DISALLOW_COPY_AND_ASSIGN(FlutterApplication);
};

}  // namespace cast
