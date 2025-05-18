/**
 * @file src/platform/linux/input/inputtino_touchpad.cpp
 * @brief Definitions for inputtino touch input handling.
 */
#include "inputtino_touchpad.h"

#include "inputtino_common.h"
#include "src/config.h"
#include "src/logging.h"
#include "src/platform/common.h"
#include "src/utility.h"

#include <boost/locale.hpp>
#include <inputtino/input.hpp>
#include <libevdev/libevdev.h>

using namespace std::literals;

namespace platf::touchpad {
  void
    update(input_raw_t *raw, const touch_port_t &touch_port, const touchpad_input_t &touchpad) {
    BOOST_LOG(info) << "Updating trackpad"sv;
    if (raw->trackpad) {
      switch (touchpad.eventType) {
        case LI_TRACKPAD_EVENT_BUTTON_DOWN:
          {
            (*raw->trackpad).set_left_btn(1);
            break;
          }
        case LI_TRACKPAD_EVENT_BUTTON_UP:
          {
            (*raw->trackpad).set_left_btn(0);
            break;
          }
        case LI_TRACKPAD_EVENT_FINGER_DOWN:
        case LI_TRACKPAD_EVENT_FINGER_MOVE:
          {
            // Convert our 0..360 range to -90..90 relative to Y axis
            int adjusted_angle = touchpad.rotation;

            if (adjusted_angle == LI_ROT_UNKNOWN) {
              adjusted_angle = 0;
            }

            if (adjusted_angle > 90 && adjusted_angle < 270) {
              // Lower hemisphere
              adjusted_angle = 180 - adjusted_angle;
            }

            // Wrap the value if it's out of range
            if (adjusted_angle > 90) {
              adjusted_angle -= 360;
            } else if (adjusted_angle < -90) {
              adjusted_angle += 360;
            }

            BOOST_LOG(info) << "Placing finger: ("sv << touchpad.x << ", "sv << touchpad.y << "):" << touchpad.pointerId << "@" << adjusted_angle;
            (*raw->trackpad).place_finger(touchpad.pointerId, touchpad.x, touchpad.y, 0.2, adjusted_angle);
            break;
          }
        case LI_TRACKPAD_EVENT_FINGER_UP:
          {
            (*raw->trackpad).release_finger(touchpad.pointerId);
            break;
          }
          // TODO: LI_TRACKPAD_EVENT_CANCEL_ALL
      }
    } else {
      BOOST_LOG(error) << "Touchpad not allocated"sv;
    }
  }
}  // namespace platf::touchpad
