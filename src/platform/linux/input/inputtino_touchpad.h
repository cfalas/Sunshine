/**
 * @file src/platform/linux/input/inputtino_touchpad.h
 * @brief Declarations for inputtino touchpad input handling.
 */
#pragma once

#include <boost/locale.hpp>
#include <inputtino/input.hpp>
#include <libevdev/libevdev.h>

#include "src/platform/common.h"

#include "inputtino_common.h"

using namespace std::literals;

namespace platf::touchpad {
  void
  update(input_raw_t *raw, const touch_port_t &touch_port, const touchpad_input_t &touchpad);
}

