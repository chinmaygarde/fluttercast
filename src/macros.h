#pragma once

#include <iostream>

#define CAST_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete;

#define CAST_DISALLOW_ASSIGN(TypeName) void operator=(const TypeName&) = delete;

#define CAST_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  CAST_DISALLOW_COPY(TypeName)                  \
  CAST_DISALLOW_ASSIGN(TypeName)

#define CAST_LOG std::cout
#define CAST_ERROR std::cerr << "Error "
