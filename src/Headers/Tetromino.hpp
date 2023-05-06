#pragma once

class Tetromino
{
public:
	enum Shape : unsigned char
	{
		I,
		L,
		J,
		O,
		S,
		T,
		Z
	};

	Tetromino();

	bool moveDown(const std::vector<std::vector<unsigned char>>& matrix);
	bool reset(const std::vector<std::vector<unsigned char>>& matrix);
	bool reset(Tetromino::Shape shape, const std::vector<std::vector<unsigned char>>& matrix);

	Tetromino::Shape getShape();
	Tetromino::Shape getNextShape();
	Tetromino::Shape getHoldingShape();
	std::vector<sf::Vector2i> getNextShapeTetromino(unsigned char x, unsigned char y);

	int hardDrop(std::vector<std::vector<unsigned char>>& matrix);
	void moveLeft(const std::vector<std::vector<unsigned char>>& matrix);
	void moveRight(const std::vector<std::vector<unsigned char>>& matrix);
	void rotate(bool clockwise, const std::vector<std::vector<unsigned char>>& matrix);

	void updateMatrix(std::vector<std::vector<unsigned char>>& matrix);

	unsigned char getRotation();
	bool isHolding();
	std::vector<sf::Vector2i> getGhostMinos(const std::vector<std::vector<unsigned char>>& matrix);
	std::vector<sf::Vector2i> getMinos();
	std::vector<sf::Vector2i> getHoldMinos(unsigned char x, unsigned char y);
	std::vector<sf::Vector2i> getWallKickData(unsigned char nextRotation);
	void processHoldSwap(std::vector<std::vector<unsigned char>>& matrix);

protected:
	unsigned char m_Rotation;
	Tetromino::Shape m_Shape;
	Tetromino::Shape m_NextShape;
	Tetromino::Shape m_HoldShape;
	bool m_IsHolding = false;
	std::default_random_engine m_Random;

	std::vector<sf::Vector2i> m_Minos;

	std::vector<sf::Vector2i> getTetromino(Shape shape, unsigned char x, unsigned char y);
	Tetromino::Shape selectRandomShape();
};
