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
#include "range.h"
#include "gui/menu_item/patch_cable_strength/range.h"
#include "gui/ui/sound_editor.h"
#include "modulation/params/param_descriptor.h"
#include "regular.h"

namespace deluge::gui::menu_item::source_selection {
Range rangeMenu{};

ActionResult Range::handleEvent(const hid::Event& event) {
	hid::EventHandler handler{
	    [this, event](hid::ButtonEvent const& buttonEvent) {
		    if (buttonEvent.on && buttonEvent.which == hid::button::SELECT_ENC) {
			    soundEditor.tryEnterMenu(patch_cable_strength::rangeMenu);
			    return ActionResult::DEALT_WITH;
		    }
		    return SourceSelection::handleEvent(event);
	    },
	    [this, event](auto _) { return SourceSelection::handleEvent(event); },
	};

	return std::visit(handler, event);
}

ParamDescriptor Range::getDestinationDescriptor() {
	ParamDescriptor descriptor{};
	descriptor.setToHaveParamAndSource(soundEditor.patchingParamSelected, regularMenu.s);
	return descriptor;
}

MenuItem* Range::patchingSourceShortcutPress(PatchSource newS, bool previousPressStillActive) {
	return (MenuItem*)0xFFFFFFFF;
}

} // namespace deluge::gui::menu_item::source_selection
