constexpr unsigned char CELL_SIZE = 8;
constexpr unsigned char COLUMNS = 10;
constexpr unsigned char INFO_VIEW = 80;
constexpr unsigned char ROWS = 20;
constexpr unsigned char SCREEN_RESIZE = 4;
constexpr unsigned char MOVE_SPEED = 4;
constexpr unsigned char START_FALL_SPEED = 32;

constexpr unsigned short FRAME_DURATION = 16667;

struct Position
{
	char x;
	char y;
};

enum GameState : unsigned char
{
	NOT_STARTED,
	IN_PROGRESS,
	GAME_OVER
};