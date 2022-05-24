#pragma once

// Croissant namesapce
#define _CR_NAMESPACE_BEGIN     namespace cr {
#define _CR_NAMESPACE_END       }

// headers
#include "glm/glm.hpp"
#include <memory>   // shared_ptr

// constants
const float     _INFINITY = std::numeric_limits<float>::infinity();
const float     _EPSILON = 1e-8;

