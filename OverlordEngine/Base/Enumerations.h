#pragma once

#pragma region InputManager
enum class GamepadIndex : DWORD
{
	playerOne = 0,
	playerTwo = 1,
	playerThree = 2,
	playerFour = 3
};

enum class InputState
{
	pressed,
	released,
	down
};
enum class ControllerButton : WORD
{
	GAMEPAD_DPAD_UP = 0x0001,
	GAMEPAD_DPAD_DOWN = 0x0002,
	GAMEPAD_DPAD_LEFT = 0x0004,
	GAMEPAD_DPAD_RIGHT = 0x0008,
	GAMEPAD_START = 0x0010,
	GAMEPAD_BACK = 0x0020,
	GAMEPAD_LEFT_THUMB = 0x0040,
	GAMEPAD_RIGHT_THUMB = 0x0080,
	GAMEPAD_LEFT_SHOULDER = 0x0100,
	GAMEPAD_RIGHT_SHOULDER = 0x0200,
	GAMEPAD_BUTTON_SOUTH = 0x1000,
	GAMEPAD_BUTTON_EAST = 0x2000,
	GAMEPAD_BUTTON_WEST = 0x4000,
	GAMEPAD_BUTTON_NORTH = 0x8000
};
struct InputAction
{
	InputAction() :
		actionID(-1),
		triggerState(InputState::pressed),
		keyboardCode(-1),
		mouseButtonCode(-1),
		gamepadButtonCode(0),
		playerIndex(GamepadIndex::playerOne),
		isTriggered(false) {}

	InputAction(int actionID, InputState triggerState = InputState::pressed, int keyboardCode = -1, int mouseButtonCode = -1, WORD gamepadButtonCode = 0, GamepadIndex playerIndex = GamepadIndex::playerOne) :
		actionID(actionID),
		triggerState(triggerState),
		keyboardCode(keyboardCode),
		mouseButtonCode(mouseButtonCode),
		gamepadButtonCode(gamepadButtonCode),
		playerIndex(playerIndex),
		isTriggered(false) {}

	int actionID;
	InputState triggerState;
	int keyboardCode; //VK_... (Range 0x07 <> 0xFE)
	int mouseButtonCode; //VK_... (Range 0x00 <> 0x06)
	WORD gamepadButtonCode; //XINPUT_GAMEPAD_...
	GamepadIndex playerIndex;
	bool isTriggered;
};
#pragma endregion

#pragma region RigidBodyComponent
enum class RigidBodyConstraint
{
	None = 0,
	RotX = 1 << 0,
	RotY = 1 << 1,
	RotZ = 1 << 2,
	TransX = 1 << 3,
	TransY = 1 << 4,
	TransZ = 1 << 5,

	AllRot = RotX | RotY | RotZ,
	AllTrans = TransX | TransY | TransZ,
	All = AllRot | AllTrans
};
ENABLE_BITMASK_OPERATORS(RigidBodyConstraint)

enum class CollisionGroup : UINT32
{
	None = 0,
	Group0 = (1 << 0),
	Group1 = (1 << 1),
	Group2 = (1 << 2),
	Group3 = (1 << 3),
	Group4 = (1 << 4),
	Group5 = (1 << 5),
	Group6 = (1 << 6),
	Group7 = (1 << 7),
	Group8 = (1 << 8),
	Group9 = (1 << 9)
};
ENABLE_BITMASK_OPERATORS(CollisionGroup)
#pragma endregion

#pragma region TransformComponent
enum class TransformChanged
{
	NONE = 0x00,
	TRANSLATION = 0x01,
	ROTATION = 0x02,
	SCALE = 0x04,
};
ENABLE_BITMASK_OPERATORS(TransformChanged)
#pragma endregion