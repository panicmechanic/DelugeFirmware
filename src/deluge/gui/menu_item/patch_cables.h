#include "definitions_cxx.hpp"
#include "gui/menu_item/menu_item.h"

namespace deluge::gui::menu_item {
class PatchCables : public MenuItem {
public:
	PatchCables(char const* newName = nullptr) : MenuItem(newName) {}
	void beginSession(MenuItem* navigatedBackwardFrom = nullptr) final;
	void selectEncoderAction(int32_t offset) final;
	void readValueAgain() final;
	MenuItem* selectButtonPress() final;
	uint8_t shouldBlinkPatchingSourceShortcut(PatchSource s, uint8_t* colour) final;

#if HAVE_OLED
	void drawPixelsForOled() final;
	int scrollPos = 0; // Each instance needs to store this separately
#else
	void drawValue();
#endif

	void renderOptions();
	void blinkShortcuts();
	void blinkShortcutsSoon();
	ActionResult timerCallback() override;

	int32_t savedVal = 0;
	int32_t currentValue = 0;

	static_vector<std::string_view, kMaxNumPatchCables> options;

	PatchSource blinkSrc = PatchSource::NOT_AVAILABLE;
	PatchSource blinkSrc2 = PatchSource::NOT_AVAILABLE;
};

} // namespace deluge::gui::menu_item