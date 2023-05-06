#include "Headers/Tetromino.hpp"
#include "Headers/Global.hpp"
#include <iostream>
#include <vector>

std::vector<Tetromino::Shape> availableShapes = {
	Tetromino::Shape::I,
	Tetromino::Shape::J,
	Tetromino::Shape::L,
	Tetromino::Shape::O,
	Tetromino::Shape::S,
	Tetromino::Shape::T,
	Tetromino::Shape::Z,
};

/// @brief Gets the tetromino of a given shape
std::vector<sf::Vector2i> Tetromino::getTetromino(Shape shape, unsigned char x, unsigned char y)
{
	std::vector<sf::Vector2i> outputTetromino(4);

	switch (shape)
	{
		// I
		case Shape::I: {
			outputTetromino[0] = { 1, -1 };
			outputTetromino[1] = { 0, -1 };
			outputTetromino[2] = { -1, -1 };
			outputTetromino[3] = { -2, -1 };
			break;
		}
		// L
		case Shape::L: {
			outputTetromino[0] = { 0, 0 };
			outputTetromino[1] = { 1, 0 };
			outputTetromino[2] = { -1, -1 };
			outputTetromino[3] = { -1, 0 };
			break;
		}
		// J
		case Shape::J: {
			outputTetromino[0] = { 0, 0 };
			outputTetromino[1] = { 1, 0 };
			outputTetromino[2] = { 1, -1 };
			outputTetromino[3] = { -1, 0 };
			break;
		}
		// O
		case Shape::O: {
			outputTetromino[0] = { 0, 0 };
			outputTetromino[1] = { 0, -1 };
			outputTetromino[2] = { -1, -1 };
			outputTetromino[3] = { -1, 0 };
			break;
		}
		// S
		case Shape::S: {
			outputTetromino[0] = { 0, 0 };
			outputTetromino[1] = { 1, -1 };
			outputTetromino[2] = { 0, -1 };
			outputTetromino[3] = { -1, 0 };
			break;
		}
		// T
		case Shape::T: {
			outputTetromino[0] = { 0, 1 };
			outputTetromino[1] = { 0, 0 };
			outputTetromino[2] = { -1, 1 };
			outputTetromino[3] = { 1, 1 };
			break;
		}
		// Z
		case Shape::Z: {
			outputTetromino[0] = { 0, 0 };
			outputTetromino[1] = { 1, 0 };
			outputTetromino[2] = { 0, -1 };
			outputTetromino[3] = { -1, -1 };
			break;
		}
		default:
			break;
	}

	for (sf::Vector2i& mino : outputTetromino)
	{
		mino.x += x;
		mino.y += y;
	}

	return outputTetromino;
}

Tetromino::Tetromino() :
	m_Rotation(0)
{
	m_Random.seed(std::chrono::system_clock::now().time_since_epoch().count());

	m_Shape = selectRandomShape();
	m_NextShape = selectRandomShape();
	m_Minos = getTetromino(m_Shape, COLUMNS / 2, 1);
}

bool Tetromino::moveDown(const std::vector<std::vector<unsigned char>>& matrix)
{
	for (sf::Vector2i mino : m_Minos)
	{
		// Check for collision with bottom of grid or with the rest of the already placed tetrominos
		if (ROWS == 1 + mino.y || matrix[mino.x][1 + mino.y] > 0)
		{
			return false;
		}
	}

	// Shift tetromino down
	for (sf::Vector2i& mino : m_Minos)
	{
		mino.y++;
	}

	return true;
}

bool Tetromino::reset(const std::vector<std::vector<unsigned char>>& matrix)
{
	auto tempShape = m_NextShape;
	m_NextShape = selectRandomShape();
	return reset(tempShape, matrix);
}

bool Tetromino::reset(Tetromino::Shape shape, const std::vector<std::vector<unsigned char>>& matrix)
{
	m_Rotation = 0;
	m_Shape = shape;

	// Get tetromino with the next shape and locate it at the top center
	m_Minos = getTetromino(m_Shape, COLUMNS / 2, 1);

	for (sf::Vector2i mino : m_Minos)
	{
		// Check if the starting location is occupied
		if (matrix[mino.x][mino.y] > 0)
		{
			return false;
		}
	}

	return true;
}

Tetromino::Shape Tetromino::getShape()
{
	return m_Shape;
}

void Tetromino::moveLeft(const std::vector<std::vector<unsigned char>>& matrix)
{
	for (sf::Vector2i mino : m_Minos)
	{
		// Check for collision out of bounds or with another already placed tetrominio
		if (mino.x - 1 < 0 || (mino.y > 0 && matrix[mino.x - 1][mino.y] > 0))
		{
			return;
		}
	}

	for (sf::Vector2i& mino : m_Minos)
	{
		mino.x--;
	}
}

void Tetromino::moveRight(const std::vector<std::vector<unsigned char>>& matrix)
{
	for (sf::Vector2i mino : m_Minos)
	{
		// Check for collision out of bounds or with another already placed tetrominio
		if (mino.x + 1 > 9 || (mino.y > 0 && matrix[mino.x + 1][mino.y] > 0))
		{
			return;
		}
	}

	for (sf::Vector2i& mino : m_Minos)
	{
		mino.x++;
	}
}

void Tetromino::rotate(bool clockwise, const std::vector<std::vector<unsigned char>>& matrix)
{
	// Don't need to rotate Os
	if (m_Shape == Shape::O)
	{
		return;
	}

	unsigned char nextRotation;

	if (!clockwise)
	{
		nextRotation = (3 + m_Rotation) % 4;
	}
	else
	{
		nextRotation = (1 + m_Rotation) % 4;
	}

	std::vector<sf::Vector2i> currentMinos = m_Minos;

	if (m_Shape == Shape::I)
	{
		// I rotation is a specific location
		float centerX = 0.5f * (m_Minos[1].x + m_Minos[2].x);
		float centerY = 0.5f * (m_Minos[1].y + m_Minos[2].y);

		if (m_Rotation == 0)
		{
			centerY += 0.5f;
		}
		else if (m_Rotation == 1)
		{
			centerX -= 0.5f;
		}
		else if (m_Rotation == 2)
		{
			centerY -= 0.5f;
		}
		else if (m_Rotation == 3)
		{
			centerX += 0.5f;
		}

		for (sf::Vector2i& mino : m_Minos)
		{
			float x = mino.x - centerX;
			float y = mino.y - centerY;

			if (clockwise)
			{
				mino.x = static_cast<char>(centerX - y);
				mino.y = static_cast<char>(centerY + x);
			}
			else
			{
				mino.x = static_cast<char>(centerX + y);
				mino.y = static_cast<char>(centerY - x);
			}
		}
	}
	else
	{
		for (unsigned char i = 1; i < m_Minos.size(); i++)
		{
			char x = m_Minos[i].x - m_Minos[0].x;
			char y = m_Minos[i].y - m_Minos[0].y;

			if (clockwise)
			{
				m_Minos[i].x = m_Minos[0].x - y;
				m_Minos[i].y = x + m_Minos[0].y;
			}
			else
			{
				m_Minos[i].x = y + m_Minos[0].x;
				m_Minos[i].y = m_Minos[0].y - x;
			}
		}
	}

	for (sf::Vector2i& wallKickPoint : getWallKickData(nextRotation))
	{
		bool canTurn = true;

		for (sf::Vector2i& mino : m_Minos)
		{
			if (mino.x + wallKickPoint.x < 0 || mino.x + wallKickPoint.x >= COLUMNS || mino.y + wallKickPoint.y >= ROWS)
			{
				canTurn = false;
				break;
			}

			if (mino.y + wallKickPoint.y < 0)
			{
				continue;
			}
			else if (matrix[mino.x + wallKickPoint.x][mino.y + wallKickPoint.y] > 0)
			{
				canTurn = false;
				break;
			}
		}

		if (canTurn)
		{
			m_Rotation = nextRotation;

			for (sf::Vector2i& mino : m_Minos)
			{
				mino.x += wallKickPoint.x;
				mino.y += wallKickPoint.y;
			}

			return;
		}
	}

	m_Minos = currentMinos;
}

void Tetromino::updateMatrix(std::vector<std::vector<unsigned char>>& matrix)
{
	for (sf::Vector2i& mino : m_Minos)
	{
		if (mino.y < 0)
		{
			continue;
		}

		matrix[mino.x][mino.y] = 1 + getShape();
	}
}

std::vector<sf::Vector2i> Tetromino::getMinos()
{
	return m_Minos;
}

unsigned char Tetromino::getRotation()
{
	return m_Rotation;
}

std::vector<sf::Vector2i> Tetromino::getWallKickData(unsigned char nextRotation)
{
	if (m_Shape == Shape::I)
	{
		if (m_Rotation == 0)
		{
			if (nextRotation == 1)
			{
				return { { 0, 0 }, { -2, 0 }, { 1, 0 }, { -2, 1 }, { 1, -2 } };
			}
			else if (nextRotation == 3)
			{
				return { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, -2 }, { 2, 1 } };
			}
		}
		else if (m_Rotation == 1)
		{
			if (nextRotation == 0)
			{
				return { { 0, 0 }, { 2, 0 }, { -1, 0 }, { 2, -1 }, { -1, 2 } };
			}
			else if (nextRotation == 2)
			{
				return { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, -2 }, { 2, 1 } };
			}
		}
		else if (m_Rotation == 2)
		{
			if (nextRotation == 1)
			{
				return { { 0, 0 }, { 1, 0 }, { -2, 0 }, { 1, 2 }, { -2, -1 } };
			}
			else if (nextRotation == 3)
			{
				return { { 0, 0 }, { 2, 0 }, { -1, 0 }, { 2, -1 }, { -1, 2 } };
			}
		}
		else if (m_Rotation == 3)
		{
			if (nextRotation == 0)
			{
				return { { 0, 0 }, { 1, 0 }, { -2, 0 }, { 1, 2 }, { -2, -1 } };
			}
			else if (nextRotation == 2)
			{
				return { { 0, 0 }, { -2, 0 }, { 1, 0 }, { -2, 1 }, { 1, -2 } };
			}
		}
	}
	else
	{
		if (m_Rotation == 0 || m_Rotation == 2)
		{
			if (nextRotation == 1)
			{
				return { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } };
			}
			else if (nextRotation == 3)
			{
				return { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } };
			}
		}
		else if (m_Rotation == 1)
		{
			return { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } };
		}
		else if (m_Rotation == 3)
		{
			return { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 } };
		}
	}

	return { { 0, 0 } };
}

std::vector<sf::Vector2i> Tetromino::getGhostMinos(const std::vector<std::vector<unsigned char>>& matrix)
{
	int distance = 0;
	bool collisionFound = false;

	std::vector<sf::Vector2i> ghostMinos = m_Minos;

	while (!collisionFound)
	{
		distance++;

		for (sf::Vector2i mino : m_Minos)
		{
			if (distance + mino.y == ROWS || (distance + mino.y > 0 && matrix[mino.x][distance + mino.y] > 0))
			{
				collisionFound = true;
				break;
			}
		}
	}

	for (sf::Vector2i& mino : ghostMinos)
	{
		mino.y += distance - 1;
	}

	return ghostMinos;
}

Tetromino::Shape Tetromino::getNextShape()
{
	return m_NextShape;
}

Tetromino::Shape Tetromino::getHoldingShape()
{
	return m_HoldShape;
}

std::vector<sf::Vector2i> Tetromino::getNextShapeTetromino(unsigned char x, unsigned char y)
{
	return getTetromino(m_NextShape, x, y);
}

std::vector<sf::Vector2i> Tetromino::getHoldMinos(unsigned char x, unsigned char y)
{
	if (m_IsHolding)
		return getTetromino(m_HoldShape, x, y);

	return std::vector<sf::Vector2i> { { 0, 0 } };
}

bool Tetromino::isHolding()
{
	return m_IsHolding;
}

int Tetromino::hardDrop(std::vector<std::vector<unsigned char>>& matrix)
{
	std::vector<sf::Vector2i> ghostMinos = getGhostMinos(matrix);

	int distance = ghostMinos[0].y - m_Minos[0].y;

	m_Minos = getGhostMinos(matrix);

	return distance;
}

void Tetromino::processHoldSwap(std::vector<std::vector<unsigned char>>& matrix)
{
	if (!m_IsHolding)
	{
		m_HoldShape = m_Shape;
		m_IsHolding = true;
		reset(matrix);
	}
	else
	{
		auto tempShape = m_HoldShape;
		m_HoldShape = m_Shape;
		reset(tempShape, matrix);
	}
}

Tetromino::Shape Tetromino::selectRandomShape()
{
	if (availableShapes.size() == 0)
	{
		availableShapes.insert(availableShapes.end(),
			{
				Tetromino::Shape::I,
				Tetromino::Shape::J,
				Tetromino::Shape::L,
				Tetromino::Shape::O,
				Tetromino::Shape::S,
				Tetromino::Shape::T,
				Tetromino::Shape::Z,
			});
	}

	std::shuffle(std::begin(availableShapes), std::end(availableShapes), m_Random);

	Tetromino::Shape toReturn = availableShapes.back();
	availableShapes.pop_back();
	return toReturn;
}