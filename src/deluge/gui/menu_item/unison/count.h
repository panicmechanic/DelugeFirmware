/*
 * Copyright (c) 2014-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include "gui/menu_item/integer.h"
#include "gui/ui/sound_editor.h"
#include "model/model_stack.h"
#include "processing/sound/sound.h"
#include "stereoSpread.h"

namespace deluge::gui::menu_item::unison {

class Count final : public Integer {
public:
	using Integer::Integer;
	void readCurrentValue() override { this->setValue(soundEditor.currentSound->numUnison); }
	void writeCurrentValue() override {
		char modelStackMemory[MODEL_STACK_MAX_SIZE];
		ModelStackWithSoundFlags* modelStack = soundEditor.getCurrentModelStack(modelStackMemory)->addSoundFlags();
		soundEditor.currentSound->setNumUnison(this->getValue(), modelStack);
	}
	[[nodiscard]] int32_t getMinValue() const override { return 1; }
	[[nodiscard]] int32_t getMaxValue() const override { return kMaxNumVoicesUnison; }

	ActionResult handleEvent(hid::Event const& event) override {
		hid::EventHandler handler{
		    [this, event](hid::ButtonEvent const& buttonEvent) {
				if (buttonEvent.on && buttonEvent.which == hid::button::SELECT_ENC) {
					soundEditor.tryEnterMenu(stereoSpreadMenu);
					return ActionResult::DEALT_WITH;
				}
				return Integer::handleEvent(event);
		    },
		    [this, event](auto _) { return Integer::handleEvent(event); },
		};

		return std::visit(handler, event);
	}

};
} // namespace deluge::gui::menu_item::unison
