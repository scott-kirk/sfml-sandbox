#include <algorithm>
#include <list>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

std::unique_ptr<sf::CircleShape> createBullet(int width) {
	std::unique_ptr<sf::CircleShape> bullet(new sf::CircleShape);
	bullet->setRadius(10.0f);
	bullet->setFillColor(sf::Color::Red);
	sf::Vector2f newBulletPosition = bullet->getPosition();
	newBulletPosition.y = -bullet->getRadius();
	newBulletPosition.x = static_cast<float>(rand() % width);
	bullet->setPosition(newBulletPosition);
	return bullet;
}

std::unique_ptr<sf::CircleShape> createPlayer(sf::Vector2u windowSize) {
	std::unique_ptr<sf::CircleShape> player(new sf::CircleShape(50.f));
	player->setFillColor(sf::Color::Green);
	player->setPosition(sf::Vector2f(windowSize.x / 2 - player->getRadius(), windowSize.y - player->getRadius() * 2));
	return player;
}

sf::Font font;
std::unique_ptr<sf::Text> createText(sf::String characters) {
	if (font.getInfo().family == "") {
		font.loadFromFile("Resources/OpenSans-Regular.ttf");
	}
	std::unique_ptr<sf::Text> text(new sf::Text(characters, font, 30));
	text->setFillColor(sf::Color::Red);
	text->setPosition(sf::Vector2f(0, 0));
	return text;
}

const auto DEFAULT_BULLET_SPEED = 200.0f;

int WinMain()
{
	sf::RenderWindow window(sf::VideoMode(500, 500), "SFML works!");
	window.setVerticalSyncEnabled(true);
	auto player = createPlayer(window.getSize());
	auto pausedText = createText("Paused");
	auto gameOverText = createText("Game Over!");
	float fraps;
	auto bulletSpeed = DEFAULT_BULLET_SPEED;
	auto playerSpeed = 400.0f;
	sf::Clock fps;
	sf::Clock difficultyTimer;
	bool upFlag = false, downFlag = false, leftFlag = false, rightFlag = false;
	bool paused = false, gameOver = false;
	std::list<sf::Drawable*> drawables;
	std::list<std::unique_ptr<sf::CircleShape>> bullets;
	bullets.push_back(std::move(createBullet(window.getSize().x)));
	sf::Music music;
	music.openFromFile("Resources/background-music.wav");
	music.setLoop(true);
	music.play();
	sf::SoundBuffer bulletSoundBuffer;
	bulletSoundBuffer.loadFromFile("Resources/bullet.wav");
	sf::Sound bulletSound;
	bulletSound.setBuffer(bulletSoundBuffer);

	while (window.isOpen()) {
		// reset fps counter and list of shapes that need drawing
		fraps = fps.restart().asSeconds();
		drawables.clear();

		// user input logic
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
					case  sf::Keyboard::Escape: {
						if (paused) {
							paused = false;
						} else {
							paused = true;
						}
						break;
					}
					case  sf::Keyboard::Space: {
						if (paused || gameOver) {
							gameOver = false;
							paused = false;
							bullets.clear();
							player->setPosition(sf::Vector2f(window.getSize().x / 2 - player->getRadius(), window.getSize().y - player->getRadius() * 2));
							bulletSpeed = DEFAULT_BULLET_SPEED;
							bullets.push_back(std::move(createBullet(window.getSize().x)));
							difficultyTimer.restart();
						}
						break;
					}

					case sf::Keyboard::W: {
						upFlag = true;
						break;
					}
					case sf::Keyboard::S: {
						downFlag = true;
						break;
					}
					case sf::Keyboard::A: {
						leftFlag = true;
						break;
					}
					case sf::Keyboard::D: {
						rightFlag = true;
						break;
					}
				}
			}
			if (event.type == sf::Event::KeyReleased) {
				switch (event.key.code) {
					case sf::Keyboard::W: {
						upFlag = false;
						break;
					}
					case sf::Keyboard::S: {
						downFlag = false;
						break;
					}
					case sf::Keyboard::A: {
						leftFlag = false;
						break;
					}
					case sf::Keyboard::D: {
						rightFlag = false;
						break;
					}
				}
			}
		}

		// physics logic
		if (!paused && !gameOver) {
			if (music.getStatus() == music.Paused) {
				music.play();
			}
			// increase difficulty every 10 seconds
			if (difficultyTimer.getElapsedTime().asSeconds() > 10) {
				bulletSpeed *= 1.1f;
				bullets.push_back(std::move(createBullet(window.getSize().x)));
				difficultyTimer.restart();
			}

			// make each bullet move and reset it to the top if it reached the bottom of the screen
			for (auto&& bullet : bullets) {
				sf::Vector2f newBulletPosition = bullet->getPosition();
				if (newBulletPosition.y == -bullet->getRadius()) {
					bulletSound.play();
				}
				newBulletPosition.y = bullet->getPosition().y + bulletSpeed * fraps;
				if (newBulletPosition.y >= window.getSize().y) {
					newBulletPosition.y = -bullet->getRadius();
					newBulletPosition.x = static_cast<float>(rand() % window.getSize().x);
				}
				bullet->setPosition(newBulletPosition);
				if (newBulletPosition.y == -bullet->getRadius()) {
					bulletSound.play();
				}
			}

			// move the player in the correct direction
			sf::Vector2f newPlayerPosition = player->getPosition();
			if (leftFlag) {
				newPlayerPosition.x = std::max(player->getPosition().x - playerSpeed * fraps, -player->getRadius());
			}
			if (rightFlag) {
				newPlayerPosition.x = std::min(player->getPosition().x + playerSpeed * fraps, (float)window.getSize().x - player->getRadius());
			}
			if (upFlag) {
				newPlayerPosition.y = std::max(player->getPosition().y - playerSpeed * fraps, -player->getRadius());
			}
			if (downFlag) {
				newPlayerPosition.y = std::min(player->getPosition().y + playerSpeed * fraps, (float)window.getSize().y - player->getRadius());
			}
			player->setPosition(newPlayerPosition);
		} else if (paused) {
			if (music.getStatus() == music.Playing) {
				music.pause();
			}
		}

		// draw player and bullets and do collision detection
		drawables.push_back(player.get());
		for (auto&& bullet : bullets) {
			drawables.push_back(bullet.get());
			if (player->getGlobalBounds().intersects(bullet->getGlobalBounds())) {
				gameOver = true;
			}
		}

		// show proper menus based on flags
		if (gameOver) {
			drawables.push_back(gameOverText.get());
		} else if (paused) {
			drawables.push_back(pausedText.get());
		}


		// window draw logic
		window.clear();
		for (auto&& drawable : drawables) {
			window.draw(*drawable);
		}
		window.display();
		drawables.clear();
	}

	return 0;
}
