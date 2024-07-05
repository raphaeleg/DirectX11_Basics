#pragma once
#include <algorithm>

class InputClass
{
public:
	static auto constexpr MAX_KEYS = 256;

	InputClass() {
		std::fill(m_keys, m_keys + MAX_KEYS, false);
	}
	InputClass(const InputClass&) {
		std::fill(m_keys, m_keys + MAX_KEYS, false);
	}
	~InputClass() {};

	void KeyDown(unsigned int input) { m_keys[input] = true; }
	void KeyUp(unsigned int input) { m_keys[input] = false; }

	bool IsKeyDown(unsigned int key) const { return m_keys[key]; }

private:
	bool m_keys[MAX_KEYS];
};