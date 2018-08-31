
#include <rfb/rfb.h>
#include <stdlib.h>

#include "cast_screen.h"

int main(int argc, char* argv[]) {
  cast::CastScreen screen("Sample Cast", 800, 600);

  if (!screen.IsValid()) {
    return EXIT_FAILURE;
  }

  screen.Run();

  return EXIT_SUCCESS;
}
