#pragma once

#include <iostream>

#define CAST_DISALLOW_COPY(TypeName) TypeName(const TypeName&) = delete;

#define CAST_DISALLOW_ASSIGN(TypeName) void operator=(const TypeName&) = delete;

#define CAST_DISALLOW_COPY_AND_ASSIGN(TypeName) \
  CAST_DISALLOW_COPY(TypeName)                  \
  CAST_DISALLOW_ASSIGN(TypeName)

#define __CAST_LINE_PREFIX << __FILE__ << ":" << __LINE__ << ": "
#define CAST_LOG std::cout << "LOG: " __CAST_LINE_PREFIX
#define CAST_ERROR std::cerr << "ERROR: " __CAST_LINE_PREFIX
