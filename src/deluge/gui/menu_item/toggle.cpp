#include "toggle.h"
#include "gui/l10n/l10n.h"
#include "gui/ui/sound_editor.h"
#include "hid/display/oled.h"
#include <algorithm>

namespace deluge::gui::menu_item {

ActionResult Toggle::handleEvent(hid::Event const& event) {
	hid::EventHandler handler{
	    [this, event](hid::EncoderEvent const& encoderEvent) {
		    if (encoderEvent.name == hid::encoders::EncoderName::SELECT) {
			    if (encoderEvent.offset != 0) {
				    this->setValue(!this->getValue());
			    }
		    }
		    return Value<bool>::handleEvent(event);
	    },
	    [this, event](auto _) { return Value<bool>::handleEvent(event); },
	};
	return std::visit(handler, event);
}

void Toggle::beginSession(MenuItem* navigatedBackwardFrom) {
	Value::beginSession(navigatedBackwardFrom);
	if (display->haveOLED()) {
		soundEditor.menuCurrentScroll = 0;
	}
	else {
		drawValue();
	}
}

void Toggle::drawValue() {
	if (display->haveOLED()) {
		renderUIsForOled();
	}
	else {
		display->setText(this->getValue() //<
		                     ? l10n::get(l10n::String::STRING_FOR_ENABLED)
		                     : l10n::get(l10n::String::STRING_FOR_DISABLED));
	}
}

void Toggle::drawPixelsForOled() {
	const int32_t val = static_cast<int32_t>(this->getValue());
	// Move scroll
	soundEditor.menuCurrentScroll = std::clamp<int32_t>(soundEditor.menuCurrentScroll, 0, 1);

	char const* options[] = {
	    l10n::get(l10n::String::STRING_FOR_DISABLED),
	    l10n::get(l10n::String::STRING_FOR_ENABLED),
	};
	int32_t selectedOption = this->getValue() - soundEditor.menuCurrentScroll;

	int32_t baseY = (OLED_MAIN_HEIGHT_PIXELS == 64) ? 15 : 14;
	baseY += OLED_MAIN_TOPMOST_PIXEL;

	for (int32_t o = 0; o < 2; o++) {
		int32_t yPixel = o * kTextSpacingY + baseY;

		deluge::hid::display::OLED::drawString(options[o], kTextSpacingX, yPixel,
		                                       deluge::hid::display::OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS,
		                                       kTextSpacingX, kTextSpacingY);

		if (o == selectedOption) {
			deluge::hid::display::OLED::invertArea(0, OLED_MAIN_WIDTH_PIXELS, yPixel, yPixel + 8,
			                                       &deluge::hid::display::OLED::oledMainImage[0]);
			deluge::hid::display::OLED::setupSideScroller(0, options[o], kTextSpacingX, OLED_MAIN_WIDTH_PIXELS, yPixel,
			                                              yPixel + 8, kTextSpacingX, kTextSpacingY, true);
		}
	}
}
} // namespace deluge::gui::menu_item
