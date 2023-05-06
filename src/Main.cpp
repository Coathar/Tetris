#include <iostream>
#include <random>
#include <set>

#include "Headers/Global.hpp"
#include "Headers/Tetromino.hpp"
#include "Platform/Platform.hpp"

int main()
{
	// Cell colors for each shape and the background
	std::vector<sf::Color> cellColors = {
		sf::Color(36, 36, 85),
		sf::Color(0, 219, 255),
		sf::Color(0, 36, 255),
		sf::Color(255, 146, 0),
		sf::Color(255, 219, 0),
		sf::Color(0, 219, 0),
		sf::Color(146, 0, 255),
		sf::Color(219, 0, 0),
		sf::Color(73, 73, 85)
	};

	sf::Font font;
	font.loadFromFile("content/arial.ttf");

	// Timing Setup
	unsigned frameTimer = 0;
	std::chrono::time_point<std::chrono::steady_clock> previous_time;
	previous_time = std::chrono::steady_clock::now();

	// Game Variables
	std::vector<std::vector<unsigned char>> matrix(COLUMNS, std::vector<unsigned char>(ROWS));
	std::set<unsigned char> clearedLines;
	unsigned char clearLineTimer = 0;
	Tetromino tetromino;
	GameState currentGameState = GameState::NOT_STARTED;
	unsigned char fallTimer;
	unsigned char currentFallSpeed = START_FALL_SPEED;
	int score = 0;
	unsigned char level = 1;
	int totalLinesCleared = 0;
	bool hasHeld;

	// Input Variables
	float fallSpeedMultiplier = 1;
	bool hardDropPressed;
	bool hardDropProcessed = true;
	bool rotatePressed;
	signed char moveTimer = 0;

	// Window Setup
	sf::RenderWindow window(sf::VideoMode((CELL_SIZE * COLUMNS * SCREEN_RESIZE) + (INFO_VIEW * SCREEN_RESIZE), CELL_SIZE * ROWS * SCREEN_RESIZE), "Tetris");
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * COLUMNS + INFO_VIEW, CELL_SIZE * ROWS)));
	// in Windows at least, this must be called before creating the window
	// Use the screenScalingFactor
	window.setFramerateLimit(60);

	sf::Event event;
	while (window.isOpen())
	{
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();
		previous_time += std::chrono::microseconds(delta_time);
		frameTimer += delta_time;

		// Parse each frame
		while (frameTimer >= FRAME_DURATION)
		{
			frameTimer -= FRAME_DURATION;

			while (window.pollEvent(event))
			{
				switch (event.type)
				{
					case sf::Event::Closed: {
						window.close();
						break;
					}
					case sf::Event::KeyPressed: {
						switch (event.key.code)
						{
							case sf::Keyboard::Z: {
								if (!rotatePressed)
								{
									rotatePressed = true;
									tetromino.rotate(false, matrix);
								}
								break;
							}
							case sf::Keyboard::X: {
								if (!rotatePressed)
								{
									rotatePressed = true;
									tetromino.rotate(true, matrix);
								}

								break;
							}
							case sf::Keyboard::C: {
								if (!hasHeld)
								{
									hasHeld = true;
									tetromino.processHoldSwap(matrix);
								}
								break;
							}
							case sf::Keyboard::Space: {
								if (!hardDropPressed)
								{
									hardDropPressed = true;
									hardDropProcessed = false;
									score += tetromino.hardDrop(matrix) * 2;
								}
								break;
							}
							default:
								break;
						}
						break;
					}
					case sf::Event::KeyReleased: {
						switch (event.key.code)
						{
							case sf::Keyboard::X:
							case sf::Keyboard::Z: {
								rotatePressed = false;

								break;
							}
							case sf::Keyboard::Space: {
								hardDropPressed = false;
								break;
							}
							default:
								break;
						}
						break;
					}
					default:
						break;
				}
			}

			if (currentGameState == GameState::GAME_OVER)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					moveTimer = 0;
					fallTimer = 0;
					fallSpeedMultiplier = 1;
					currentFallSpeed = START_FALL_SPEED;
					rotatePressed = false;

					for (std::vector<unsigned char>& a : matrix)
					{
						std::fill(a.begin(), a.end(), 0);
					}

					currentGameState = GameState::IN_PROGRESS;
				}
			}
			else if (currentGameState == GameState::IN_PROGRESS)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && moveTimer >= 0)
				{
					moveTimer = -4;
					tetromino.moveLeft(matrix);
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && moveTimer <= 0)
				{
					moveTimer = 4;
					tetromino.moveRight(matrix);
				}

				if (moveTimer < 0)
					moveTimer++;
				else if (moveTimer > 0)
					moveTimer--;

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
				{
					fallSpeedMultiplier = 0.25;
				}
				else
				{
					fallSpeedMultiplier = 1;
				}

				if (clearLineTimer > 0)
				{
					if (--clearLineTimer == 0)
					{
						for (unsigned char clearedLine : clearedLines)
						{
							for (unsigned char x = 0; x < COLUMNS; x++)
							{
								matrix[x][clearedLine] = 0;

								for (unsigned char i = clearedLine; i > 0; i--)
								{
									matrix[x][i] = matrix[x][i - 1];
									matrix[x][i - 1] = 0;
								}
							}
						}

						if (clearedLines.size() == 1)
						{
							score += 100 * level;
						}
						else if (clearedLines.size() == 2)
						{
							score += 300 * level;
						}
						else if (clearedLines.size() == 3)
						{
							score += 500 * level;
						}
						else if (clearedLines.size() == 4)
						{
							score += 800 * level;
						}

						totalLinesCleared += clearedLines.size();
						level = std::max((totalLinesCleared / 10.0) + 1, 1.0);
						clearedLines.clear();
						hasHeld = false;

						if (!tetromino.reset(matrix))
						{
							currentGameState = GameState::GAME_OVER;
						}
					}
				}
				// Auto move piece down or force an update if a hard drop was sent
				else if (fallTimer >= (currentFallSpeed * fallSpeedMultiplier) || !hardDropProcessed)
				{
					if (fallSpeedMultiplier != 1)
					{
						score += 1;
					}

					if (!tetromino.moveDown(matrix))
					{
						tetromino.updateMatrix(matrix);

						// Check if lines should be cleared
						int lowY = ROWS;
						int highY = 0;

						for (sf::Vector2i mino : tetromino.getMinos())
						{
							if (mino.y < lowY)
							{
								lowY = mino.y;
							}

							if (mino.y > highY)
							{
								highY = mino.y;
							}
						}

						for (int i = lowY; i <= highY; i++)
						{
							bool lineCleared = true;

							for (int j = 0; j < COLUMNS; j++)
							{
								if (matrix[j][i] == 0)
								{
									lineCleared = false;
									break;
								}
							}

							if (lineCleared)
							{
								clearedLines.insert(i);
								clearLineTimer = 30;
							}
						}

						// If there wasn't a line cleared, setup the next shape.
						if (clearLineTimer == 0)
						{
							if (!tetromino.reset(matrix))
							{
								currentGameState = GameState::GAME_OVER;
							}

							hasHeld = false;
						}
					}

					fallTimer = 0;
					hardDropProcessed = true;
				}
				else
				{
					fallTimer++;
				}
			}
			else if (currentGameState == GameState::NOT_STARTED)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
				{
					currentGameState = GameState::IN_PROGRESS;
				}
			}

			sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
			float centerOffset = (CELL_SIZE - 1) / 2;
			cell.setOrigin(centerOffset, centerOffset);

			window.clear();

			sf::Text previewText;
			previewText.setFont(font);
			previewText.setString("NEXT");
			sf::FloatRect textRect = previewText.getLocalBounds();
			previewText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			previewText.setScale(sf::Vector2f(0.25, 0.25));
			previewText.setPosition(sf::Vector2f((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * 1));
			window.draw(previewText);

			sf::RectangleShape previewBorder(sf::Vector2f(5 * CELL_SIZE, 5 * CELL_SIZE));
			previewBorder.setFillColor(sf::Color(0, 0, 0));
			sf::FloatRect previewRect = previewBorder.getLocalBounds();
			previewBorder.setOrigin(previewRect.left + previewRect.width / 2.0f, previewRect.top + previewRect.height / 2.0f);
			previewBorder.setOutlineThickness(-1);
			previewBorder.setPosition(sf::Vector2f((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * 4.5));
			window.draw(previewBorder);

			previewText.setString("HOLD");
			previewText.setPosition(sf::Vector2f((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * 8));
			previewBorder.setPosition((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * 11.5);
			window.draw(previewBorder);
			window.draw(previewText);

			sf::Text scoreText;
			scoreText.setFont(font);
			scoreText.setString(std::string("Score: ") + std::to_string(score) + std::string("\nLevel: ") + std::to_string(level));
			sf::FloatRect scoreTextRect = scoreText.getLocalBounds();
			scoreText.setOrigin(scoreTextRect.left + scoreTextRect.width / 2.0f, scoreTextRect.top + scoreTextRect.height / 2.0f);
			scoreText.setScale(sf::Vector2f(0.25, 0.25));
			scoreText.setPosition(sf::Vector2f((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * 16));
			window.draw(scoreText);

			for (unsigned char x = 0; x < COLUMNS; x++)
			{
				for (unsigned char y = 0; y < ROWS; y++)
				{
					cell.setPosition((CELL_SIZE * x) + centerOffset, (CELL_SIZE * y) + centerOffset);

					// Draw cells grayed out when in the game over state
					if (currentGameState == GameState::GAME_OVER && matrix[x][y] > 0)
					{
						cell.setFillColor(cellColors[8]);
					}
					else
					{
						cell.setScale(1, 1);
						cell.setFillColor(cellColors[matrix[x][y]]);

						if (clearedLines.count(y) == 1)
						{
							cell.setScale(((float)clearLineTimer) / 30 + 0.2, ((float)clearLineTimer) / 30 + 0.2);
						}
					}

					window.draw(cell);
				}
			}

			cell.setScale(1, 1);

			// Render tetromino while gameplay is active
			if (currentGameState == GameState::IN_PROGRESS)
			{
				cell.setFillColor(cellColors[8]);

				if (clearLineTimer == 0)
				{
					for (sf::Vector2i mino : tetromino.getGhostMinos(matrix))
					{
						cell.setPosition((CELL_SIZE * mino.x) + centerOffset, (CELL_SIZE * mino.y) + centerOffset);
						window.draw(cell);
					}

					cell.setFillColor(cellColors[1 + tetromino.getShape()]);

					for (sf::Vector2i mino : tetromino.getMinos())
					{
						cell.setPosition((CELL_SIZE * mino.x) + centerOffset, (CELL_SIZE * mino.y) + centerOffset);

						window.draw(cell);
					}
				}

				for (sf::Vector2i mino : tetromino.getNextShapeTetromino(1.5f * COLUMNS, 0.25f * ROWS))
				{
					//Shifting the tetromino to the center of the preview border
					unsigned short nextTetrominoX = (CELL_SIZE * mino.x) + centerOffset;
					unsigned short nextTetrominoY = (CELL_SIZE * mino.y) + centerOffset;

					if (tetromino.getNextShape() == Tetromino::Shape::I)
					{
						nextTetrominoY += round(0.5f * CELL_SIZE);
					}
					else if (tetromino.getNextShape() != Tetromino::Shape::O)
					{
						nextTetrominoX -= round(0.5f * CELL_SIZE);
					}

					cell.setFillColor(cellColors[1 + tetromino.getNextShape()]);
					cell.setPosition(nextTetrominoX, nextTetrominoY);
					window.draw(cell);
				}

				if (tetromino.isHolding())
				{
					for (sf::Vector2i mino : tetromino.getHoldMinos(1.5f * COLUMNS, 0.64f * ROWS))
					{
						//Shifting the tetromino to the center of the preview border
						unsigned short nextTetrominoX = CELL_SIZE * mino.x + centerOffset;
						unsigned short nextTetrominoY = CELL_SIZE * mino.y + centerOffset;

						if (tetromino.getHoldingShape() == Tetromino::Shape::I)
						{
							nextTetrominoY += round(0.5f * CELL_SIZE);
						}
						else if (tetromino.getHoldingShape() != Tetromino::Shape::O)
						{
							nextTetrominoX -= round(0.5f * CELL_SIZE);
						}

						cell.setFillColor(cellColors[1 + tetromino.getHoldingShape()]);
						cell.setPosition(nextTetrominoX, nextTetrominoY);
						window.draw(cell);
					}
				}
			}

			if (currentGameState == GameState::GAME_OVER || currentGameState == GameState::NOT_STARTED)
			{
				sf::Text startText;
				startText.setFont(font);
				startText.setString("Press ENTER to start");
				sf::FloatRect textRect = startText.getLocalBounds();
				startText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
				startText.setScale(sf::Vector2f(0.20, 0.20));
				startText.setPosition(sf::Vector2f((CELL_SIZE * COLUMNS) + (INFO_VIEW / 2), CELL_SIZE * (ROWS - 1.5f)));
				window.draw(startText);
			}

			window.display();
		}
	}

	return 0;
}
