#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <list>

class Bullet {
 public:
  sf::RectangleShape shape;
  sf::Vector2f direction;
  float speed;
  Bullet(sf::Vector2u);
  void newPosition(sf::Vector2u windowSize) {
    sf::Vector2f newBulletPosition = shape.getPosition();
    int randomPos = rand();
    direction.x = 1.f;
    direction.y = 1.f;
    if (randomPos % 2 == 0) {
      direction.x = static_cast<float>(randomPos % 100 + 1);
      direction.y = static_cast<float>(randomPos % 200 - 100);
      if (rand() % 2 == 0) {
        newBulletPosition.x = 0;
      } else {
        newBulletPosition.x = static_cast<float>(windowSize.x);
        direction.x *= -1.f;
      }
      newBulletPosition.y = static_cast<float>(randomPos % windowSize.x);
    } else {
      direction.x = static_cast<float>(randomPos % 200 - 100);
      direction.y = static_cast<float>(randomPos % 100 + 1);
      newBulletPosition.x = static_cast<float>(randomPos % windowSize.y);
      if (rand() % 2 == 0) {
        newBulletPosition.y = 0;
      } else {
        newBulletPosition.y = static_cast<float>(windowSize.y);
        direction.y *= -1.f;
      }
    }
    direction /=
        sqrtf((direction.x * direction.x) + (direction.y * direction.y));
    shape.setPosition(newBulletPosition);
  }
  void tick(float fraps, sf::Vector2u windowSize) {
    sf::Vector2f nextPosition;
    nextPosition.x = shape.getPosition().x + (direction.x * speed * fraps);
    nextPosition.y = shape.getPosition().y + (direction.y * speed * fraps);
    if (nextPosition.x > windowSize.x || nextPosition.x < 0) {
      direction.x *= -1;
      if (nextPosition.x < 0) {
        nextPosition.x = 0;
      } else {
        nextPosition.x = static_cast<float>(windowSize.x);
      }
    }
    if (nextPosition.y > windowSize.y || nextPosition.y < 0) {
      direction.y *= -1;
      if (nextPosition.y < 0) {
        nextPosition.y = 0;
      } else {
        nextPosition.y = static_cast<float>(windowSize.y);
      }
    }
    shape.setPosition(nextPosition);
  }
  void difficultyTick() { speed *= 1.1f; }
};
Bullet::Bullet(sf::Vector2u windowSize) {
  speed = 200.0f;
  shape = sf::RectangleShape();
  shape.setSize(sf::Vector2f(10.0f, 10.0f));
  shape.setFillColor(sf::Color::Red);
  newPosition(windowSize);
}

std::unique_ptr<Bullet> createBullet(sf::Vector2u windowSize) {
  std::unique_ptr<Bullet> bullet(new Bullet(windowSize));
  return bullet;
}

std::unique_ptr<sf::RectangleShape> createPlayer(sf::Vector2u windowSize) {
  std::unique_ptr<sf::RectangleShape> player(
      new sf::RectangleShape(sf::Vector2f(50.f, 50.f)));
  player->setFillColor(sf::Color::Green);
  player->setPosition(sf::Vector2f(windowSize.x / 2 - player->getSize().x,
                                   windowSize.y - player->getSize().y * 2));
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

int WinMain() {
  sf::RenderWindow window(sf::VideoMode(500, 500), "Bullet Time");
  window.setVerticalSyncEnabled(true);
  auto player = createPlayer(window.getSize());
  auto pausedText = createText("Paused");
  auto gameOverText = createText("Game Over!");
  auto scoreText = createText("1");
  scoreText->setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) - (scoreText->getString().getSize() * 30), 0));
  float fraps;
  auto playerSpeed = 400.0f;
  sf::Clock fps;
  sf::Clock difficultyTimer;
  bool upFlag = false, downFlag = false, leftFlag = false, rightFlag = false;
  bool paused = false, gameOver = false;
  std::list<sf::Drawable*> drawables;
  std::list<std::unique_ptr<Bullet>> bullets;
  bullets.push_back(std::move(createBullet(window.getSize())));
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
          case sf::Keyboard::Escape: {
            if (paused) {
              paused = false;
            } else {
              paused = true;
            }
            break;
          }
          case sf::Keyboard::Space: {
            if (paused || gameOver) {
              gameOver = false;
              paused = false;
              bullets.clear();
              player->setPosition(
                  sf::Vector2f(window.getSize().x / 2 - player->getSize().x,
                               window.getSize().y - player->getSize().y * 2));
              bullets.push_back(std::move(createBullet(window.getSize())));
              scoreText->setString("1");
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
        for (auto&& bullet : bullets) {
          bullet->difficultyTick();
        }
        bullets.push_back(std::move(createBullet(window.getSize())));
        scoreText->setString(std::to_string(std::stoi(scoreText->getString().toAnsiString()) + 1 ));
        scoreText->setPosition(sf::Vector2f(static_cast<float>(window.getSize().x) - (scoreText->getString().getSize() * 25), 0));
        difficultyTimer.restart();
      }

      // make each bullet move and reset it to the top if it reached the bottom
      // of the screen
      for (auto&& bullet : bullets) {
        bullet->tick(fraps, window.getSize());
      }

      // move the player in the correct direction
      sf::Vector2f newPlayerPosition = player->getPosition();
      if (leftFlag) {
        newPlayerPosition.x =
            std::max(player->getPosition().x - playerSpeed * fraps,
                     0.f);
      }
      if (rightFlag) {
        newPlayerPosition.x =
            std::min(player->getPosition().x + playerSpeed * fraps,
                     (float)window.getSize().x - player->getSize().x);
      }
      if (upFlag) {
        newPlayerPosition.y =
            std::max(player->getPosition().y - playerSpeed * fraps,
                     0.f);
      }
      if (downFlag) {
        newPlayerPosition.y =
            std::min(player->getPosition().y + playerSpeed * fraps,
                     (float)window.getSize().y - player->getSize().y);
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
      drawables.push_back(&bullet->shape);
      if (player->getGlobalBounds().intersects(
              bullet->shape.getGlobalBounds())) {
        gameOver = true;
      }
    }

	// draw score
    drawables.push_back(scoreText.get());

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
