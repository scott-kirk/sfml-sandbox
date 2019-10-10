#include <algorithm>
#include <list>
#include <SFML/Graphics.hpp>

void processEvents(sf::RenderWindow&);
void processEvents(sf::Drawable&);

const float DEFAULT_BULLET_SPEED = 200.0;

int WinMain()
{
	sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
	window.setVerticalSyncEnabled(true);
	sf::CircleShape player(50.f);
	player.setFillColor(sf::Color::Green);
	player.setPosition(sf::Vector2f(window.getSize().x / 2 - player.getRadius(), window.getSize().y - player.getRadius() * 2));
	sf::CircleShape bullet(10.f);
	bullet.setFillColor(sf::Color::Red);
	sf::Font font;
	font.loadFromFile("Resources/OpenSans-Regular.ttf");
	sf::Text gameOverText;
	gameOverText.setFont(font);
	gameOverText.setString("Game Over!");
	gameOverText.setCharacterSize(20);
	gameOverText.setFillColor(sf::Color::Red);
	gameOverText.setPosition(sf::Vector2f(window.getSize().x / 2 - player.getRadius(), window.getSize().y / 2 - player.getRadius()));
	sf::Text pausedText;
	pausedText.setFont(font);
	pausedText.setString("Game Paused");
	pausedText.setCharacterSize(20);
	pausedText.setFillColor(sf::Color::Blue);
	pausedText.setPosition(sf::Vector2f(window.getSize().x / 2 - player.getRadius(), window.getSize().y / 2 - player.getRadius()));
	float fraps = 0.0;
	float bulletSpeed = DEFAULT_BULLET_SPEED;
	float playerSpeed = 400.0;
	sf::Clock fps;
	sf::Clock difficultyTimer;
	bool upFlag = false, downFlag = false, leftFlag = false, rightFlag = false;
	bool paused = false;
	bool gameOver = false;
	std::list<sf::Drawable*> drawables;

	while (window.isOpen())
	{
		fraps = fps.restart().asSeconds();
		drawables.clear();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			if (event.type == sf::Event::KeyPressed)
			{
				switch (event.key.code)
				{
				case  sf::Keyboard::Escape:
				{
					paused = !paused;
					break;
				}
				case  sf::Keyboard::Space:
				{
					gameOver = false;
					paused = false;
					player.setPosition(sf::Vector2f(window.getSize().x / 2 - player.getRadius(), window.getSize().y - player.getRadius() * 2));
					sf::Vector2f newBulletPosition = bullet.getPosition();
					newBulletPosition.y = -bullet.getRadius();
					newBulletPosition.x = rand() % window.getSize().x;
					bullet.setPosition(newBulletPosition);
					bulletSpeed = DEFAULT_BULLET_SPEED;
					break;
				}

				case sf::Keyboard::W:      upFlag = true; break;
				case sf::Keyboard::S:    downFlag = true; break;
				case sf::Keyboard::A:    leftFlag = true; break;
				case sf::Keyboard::D:   rightFlag = true; break;
				default: break;
				}
			}
			if (event.type == sf::Event::KeyReleased)
			{
				switch (event.key.code)
				{
					// Process the up, down, left and right keys
				case sf::Keyboard::W:      upFlag = false; break;
				case sf::Keyboard::S:    downFlag = false; break;
				case sf::Keyboard::A:    leftFlag = false; break;
				case sf::Keyboard::D:   rightFlag = false; break;
				default: break;
				}
			}
		}
		if (!paused && !gameOver)
		{
			if (difficultyTimer.getElapsedTime().asSeconds() > 10)
			{
				bulletSpeed *= 1.1f;
				difficultyTimer.restart();
			}

			sf::Vector2f newBulletPosition = bullet.getPosition();
			newBulletPosition.y = bullet.getPosition().y + bulletSpeed * fraps;
			if (newBulletPosition.y >= window.getSize().y)
			{
				newBulletPosition.y = -bullet.getRadius();
				newBulletPosition.x = rand() % window.getSize().x;
			}
			bullet.setPosition(newBulletPosition);
			sf::Vector2f newPlayerPosition = player.getPosition();
			if (leftFlag) newPlayerPosition.x = std::max(player.getPosition().x - playerSpeed * fraps, -player.getRadius());
			if (rightFlag) newPlayerPosition.x = std::min(player.getPosition().x + playerSpeed * fraps, (float)window.getSize().x - player.getRadius());
			if (upFlag) newPlayerPosition.y = std::max(player.getPosition().y - playerSpeed * fraps, -player.getRadius());
			if (downFlag) newPlayerPosition.y = std::min(player.getPosition().y + playerSpeed * fraps, (float)window.getSize().y - player.getRadius());
			player.setPosition(newPlayerPosition);
		}
		drawables.push_back(&bullet);
		drawables.push_back(&player);

		if (player.getGlobalBounds().intersects(bullet.getGlobalBounds()))
		{
			gameOver = true;
		}

		if (gameOver)
		{
			drawables.push_back(&gameOverText);
		}
		else if (paused)
		{
			drawables.push_back(&pausedText);
		}

		window.clear();
		for (std::list<sf::Drawable*>::iterator it = drawables.begin(); it != drawables.end(); ++it)
		{
			window.draw(**it);
		}
		window.display();
		drawables.clear();
	}

	return 0;
}
