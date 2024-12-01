/*
 * Copyright © 2019-2023 Synthstrom Audible Limited
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

#include "gui/ui/rename/rename_ui.h"
#include "hid/button.h"

class SoundDrum;

class RenameDrumUI final : public RenameUI {
public:
	RenameDrumUI();
	bool opened() override;
	ActionResult buttonAction(deluge::hid::Button b, bool on, bool inCardRoutine) override;
	ActionResult padAction(int32_t x, int32_t y, int32_t velocity) override;
	ActionResult verticalEncoderAction(int32_t offset, bool inCardRoutine) override;
	bool getGreyoutColsAndRows(uint32_t* cols, uint32_t* rows) override;

	// ui
	UIType getUIType() override { return UIType::RENAME_DRUM; }
	const char* getName() override { return "rename_drum_ui"; }
	bool exitUI() override;

protected:
	void enterKeyPress() override;

private:
	SoundDrum* getDrum();
};

extern RenameDrumUI renameDrumUI;
