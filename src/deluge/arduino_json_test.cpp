#include "memory/general_memory_allocator.h"
#include "memory/memory_allocator_interface.h"
#include <ArduinoJson.hpp>
#include <cstddef>
#include <cstring>

// https://arduinojson.org/v7/api/

struct DelugeAllocator : ArduinoJson::Allocator {
	void* allocate(size_t n) override { return allocLowSpeed(n); }
	void deallocate(void* p) override { delugeDealloc(p); }

	// very hacky right now
	// Simply allocates a new size, copies from the old area, and deallocates the old pointer
	void* reallocate(void* ptr, size_t new_size) override {
		auto gma = GeneralMemoryAllocator::get();
		size_t old_size = gma.getAllocatedSize(ptr);
		// size_t diff = new_size - old_size;
		// uint32_t amount_extended_left;
		// uint32_t amount_extended_right;

		// gma.extend(ptr, diff, diff, &amount_extended_left, &amount_extended_right);
		// if (amount_extended_left >= diff) {

		// }

		void* new_ptr = allocLowSpeed(new_size);
		memcpy(new_ptr, ptr, old_size);
		delugeDealloc(ptr);
		return new_ptr;
	}
};

void test() {
	DelugeAllocator allocator;
	const char* json = R"({"hello":"world"})";
	ArduinoJson::JsonDocument doc{&allocator};
	ArduinoJson::deserializeJson(doc, json);
	const char* world = doc["hello"];
}
