#ifndef INPUT_H
#define INPUT_H

#include <vector>

struct KeyState {

	bool wasDown = false;
	bool isDown  = false;
};

class Input
{
public:
	Input() = delete;
	Input(const Input&) = delete;

	static void SetState(int key, bool isDown, int mods)
	{
		KeyState& key_ = keyStates_[key];
		key_.wasDown = key_.isDown;
		key_.isDown = isDown;
		mods_ = mods;
	}

	static bool IsModKeyDown(int key) { return mods_ & key;	}
	static bool IsKeyDown(int key) { return keyStates_[key].isDown; }
	static bool WasKeyDown(int key) { return keyStates_[key].wasDown; }

private:
	static KeyState   keyStates_[512];
	static int        mods_;
};

struct MouseState
{
public:
	MouseState() = delete;
	MouseState(const MouseState&) = delete;

	static void SetMousePosition(float x, float y)
	{
		prevX_ = currentX_;
		prevY_ = currentY_;

		currentX_ = x;
		currentY_ = y;
	}

	static std::tuple<float, float> GetMousePosition()
	{
		return std::make_tuple(currentX_, currentY_);
	}

	static std::tuple<float, float> GetMouseDelta()
	{
		return std::make_tuple(currentX_ - prevX_, currentY_ - prevY_);
	}

private:
	static float   prevX_, prevY_;
	static float   currentX_, currentY_;
};

#endif