#pragma once

#include "gui/ui_timer_manager.h"
#include "hid/buttons.h"
#include "hid/encoders.h"
#include <variant>

namespace deluge::hid {

/// A button was pressed or released on the pad gridj
class PadEvent {
public:
	bool on;
	int x;
	int y;
};

/// A button in the top section was pressed
class ButtonEvent {
public:
	/// Which button was pressed
	Button which;
	/// True if the button is being pressed, false if it is being released
	bool on;
};

class EncoderEvent {
public:
	/// Which encoder moved
	encoders::EncoderName name;
	/// Which direction it moved.
	///
	/// A positive offset indicates clockwise rotation.
	///
	/// For the function (black) encoders this is in units of detents. For the mod (gold) encoders, it's raw encoder
	/// ticks (XXX: someone should check that that's actually true...)
	int32_t offset;
};

class TimerEvent {
public:
	TimerName which;
};

using Event = std::variant<PadEvent, ButtonEvent, EncoderEvent, TimerEvent>;

template <class... Ts>
struct EventHandler : Ts... {
	using Ts::operator()...;
};

}; // namespace deluge::hid
