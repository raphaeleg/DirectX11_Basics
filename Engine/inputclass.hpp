#pragma once
#include <algorithm>

class InputClass
{
public:
	static auto constexpr MAX_KEYS = 256;

	InputClass() {
		std::fill(keys, keys + MAX_KEYS, false);
	}
	InputClass(const InputClass&) {
		std::fill(keys, keys + MAX_KEYS, false);
	}
	~InputClass() {};

	void KeyDown(unsigned int input) { keys[input] = true; }
	void KeyUp(unsigned int input) { keys[input] = false; }

	bool IsKeyDown(unsigned int key) const { return keys[key]; }

private:
	bool keys[MAX_KEYS];
};