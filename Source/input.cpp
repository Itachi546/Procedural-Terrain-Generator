#include "input.h"

KeyState Input::keyStates_[512] = {};
int Input::mods_ = 0;
float MouseState::prevX_ = 0.0f;
float MouseState::prevY_ = 0.0f;
float MouseState::currentX_ = 0.0f;
float MouseState::currentY_ = 0.0f;
