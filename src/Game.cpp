
#include "Game.h"

//////////////////////////////////////////////////////////////////////////////////////
////////////////                    MAIN FUNCTIONS START             /////////////////

void Game::handle()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			open = false;
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Key::Escape) restart();
			if (event.key.code == sf::Keyboard::Key::Right)
			{
				movingRight = true;
			}
			if (event.key.code == sf::Keyboard::Key::Left)
			{
				movingLeft = true;
			}
			if (event.key.code == sf::Keyboard::Key::Up)
			{
				movingUp = true;
			}
			if (event.key.code == sf::Keyboard::Key::Down)
			{
				movingDown = true;
			}
			if (event.key.code == sf::Keyboard::Key::Space)
			{
				space = true;
			}
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == sf::Keyboard::Key::Right)
			{
				movingRight = false;
			}
			if (event.key.code == sf::Keyboard::Key::Left)
			{
				movingLeft = false;
			}
			if (event.key.code == sf::Keyboard::Key::Up)
			{
				movingUp = false;
			}
			if (event.key.code == sf::Keyboard::Key::Down)
			{
				movingDown = false;
			}
			if (event.key.code == sf::Keyboard::Key::Space)
			{
				space = false;
			}
			break;
		}
	}
}

void Game::update()
{
	animateDeleted();

	updateDeath();
	if (end) restart();
	if (sprite1.getGlobalBounds().top + 30 <= 0) { winpos = window.getPosition(); window.close(); }

	if (!isVictory) updateGold();

	sf::Time deltaTime = deltaTimeClock.restart();

	isFromFly = false;
	if (updateFly(deltaTime)) return;

	updateMoveLR(deltaTime);
	updateMoveUD(deltaTime);
	updateSpace(deltaTime);
	if (isFromFly) sprite1.setTexture(texture13);
}

void Game::draw()
{
	window.clear(sf::Color::Black);

	for (int i = 0; i < levelSprites.size(); i++)
	{
		window.draw(levelSprites[i]);
	}

	window.draw(sprite1);

	for (sf::Sprite& enemy : enemies)
	{
		window.draw(enemy);
	}

	for (sf::Sprite& sprite : killedSprites)
	{
		window.draw(sprite);
	}

	window.draw(text);

	window.display();
}

//////////////////////          MAIN FUNCTIONS END         //////////////
///////////////////////////////////////////////////////////////////////////

Game::Game(std::string path, int level, sf::Vector2i windowPosition)
{
	pathToLevel = path;
	loadTextures();
	initVariables();
	setWindow(windowPosition);
	setIcon();
	setSprites();
	setSprite1();
	setEnemies();
	smoothTextures();
	setText(level);
}

void Game::restart()
{
	ignoreNextLevel = true;
	winpos = window.getPosition();
	window.close();
}

bool Game::isIgnoreLevel() const
{
	return ignoreNextLevel;
}

void Game::updateDeath()
{
	for (sf::Sprite& sprite : spaceBlocks)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
		{
			end = true;
			return;
		}
	}
	for (sf::Sprite& sprite : enemies)
	{
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection))
		{
			float area = intersection.width * intersection.height;
			if (area >= 350)
			{
				end = true;
				return;
			}
		}
	}
}

void Game::updateSpace(sf::Time deltaTime)
{
	if (space)
	{
		int vector = static_cast<int>(sprite1.getScale().x);
		for (sf::Sprite& block : forFly)
		{
			if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 30))
			{
				for (sf::Sprite& spaced : spaceBlocks)
				{
					if (
						vector == 1
						&& sprite1.getGlobalBounds().left + 20 >= block.getGlobalBounds().left + 15
						&& sprite1.getGlobalBounds().left + 20 <= block.getGlobalBounds().left + 30
						&& spaced.getGlobalBounds().contains(block.getGlobalBounds().left + 45, block.getGlobalBounds().top + 15)
						)
					{
						sprite1.setPosition(block.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
						removeBlock(spaced);
						return;
					}
					if (
						vector == -1
						&& sprite1.getGlobalBounds().left + 15 >= block.getGlobalBounds().left
						&& sprite1.getGlobalBounds().left + 15 <= block.getGlobalBounds().left + 15
						&& spaced.getGlobalBounds().contains(block.getGlobalBounds().left - 15, block.getGlobalBounds().top + 15)
						)
					{
						sprite1.setPosition(block.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
						removeBlock(spaced);
						return;
					}
				}
			}
		}
	}
}

void Game::removeBlock(sf::Sprite& spaced)
{
	for (sf::Sprite& sprite : forFly)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top - 2000);
	}
	for (sf::Sprite& sprite : blocks)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top - 2000);
	}
	for (sf::Sprite& sprite : levelSprites)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top - 2000);
	}
	for (sf::Sprite& sprite : spaceBlocks)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top - 2000);
	}
	queueDeleted.push_back(spaced);
	sf::Clock timer;
	queueTimer.push_back(timer);
	isAnimatedDeletes.push_back(false);
	isPushedLS.push_back(false);
	animatedSprites.push_back(sf::Sprite());
	miniAnimateDelete.push_back(sf::Clock());
	counterDeletedTextures.push_back(0);
}

void Game::animateDeleted()
{
	size_t size = queueDeleted.size();
	for (size_t i = 0; i < size; i++)
	{
		if (!isAnimatedDeletes[i])
		{
			if (miniAnimateDelete[i].getElapsedTime().asMilliseconds() >= miniAnimateInterval)
			{
				animatedSprites[i].setPosition(queueDeleted[i].getGlobalBounds().left, queueDeleted[i].getGlobalBounds().top + 2000);
				for (sf::Sprite& sprite : killedSprites)
				{
					if (sprite.getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition())
					{
						sprite.setTexture(deletedTextures[counterDeletedTextures[i]++]);
						break;
					}
				}
				if (!isPushedLS[i])
				{
					killedSprites.push_back(animatedSprites[i]);
					isPushedLS[i] = true;
				}
				miniAnimateDelete[i].restart();
			}
			if (counterDeletedTextures[i] == deletedTextures.size())
			{
				for (int k = 0; k < killedSprites.size(); k++)
				{
					if (killedSprites[k].getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition())
					{
						killedSprites.erase(killedSprites.begin() + k);
						break;
					}
				}
				isAnimatedDeletes[i] = true;
				counterDeletedTextures[i] = 0;
				isPushedLS[i] = false;
			}
			break;
		}
		float reawakenedTime = reawakenedTextures.size() * reawakenedInterval;
		if (queueTimer[i].getElapsedTime().asMilliseconds() >= deletedBlockInterval * 1000.0f - reawakenedTime)
		{
			if (miniAnimateDelete[i].getElapsedTime().asMilliseconds() >= reawakenedInterval)
			{
				for (sf::Sprite& sprite : killedSprites)
				{
					if (sprite.getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition())
					{
						sprite.setTexture(reawakenedTextures[counterDeletedTextures[i]++]);
						break;
					}
				}
				if (!isPushedLS[i])
				{
					killedSprites.push_back(animatedSprites[i]);
					isPushedLS[i] = true;
				}
				miniAnimateDelete[i].restart();
			}
			if (counterDeletedTextures[i] == reawakenedTextures.size())
			{
				for (int k = 0; k < killedSprites.size(); k++)
				{
					if (killedSprites[k].getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition())
					{
						killedSprites.erase(killedSprites.begin() + k);
						break;
					}
				}
				backBlock(queueDeleted[i]);
				queueDeleted.erase(queueDeleted.begin() + i);
				queueTimer.erase(queueTimer.begin() + i);
				isAnimatedDeletes.erase(isAnimatedDeletes.begin() + i);
				animatedSprites.erase(animatedSprites.begin() + i);
				miniAnimateDelete.erase(miniAnimateDelete.begin() + i);
				counterDeletedTextures.erase(counterDeletedTextures.begin() + i);
				isPushedLS.erase(isPushedLS.begin() + i);
				break;
			}
		}
	}
}

void Game::backBlock(sf::Sprite& spaced)
{
	for (sf::Sprite& sprite : forFly)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top + 2000);
	}
	for (sf::Sprite& sprite : blocks)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top + 2000);
	}
	for (sf::Sprite& sprite : levelSprites)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top + 2000);
	}
	for (sf::Sprite& sprite : spaceBlocks)
	{
		if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
			sprite.setPosition(spaced.getGlobalBounds().left, spaced.getGlobalBounds().top + 2000);
	}
}

void Game::updateGold()
{
	if (goldSprites.size() == 0)
	{
		for (sf::Sprite& sprite : levelSprites)
		{
			if (sprite.getGlobalBounds().top < 0) sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
		}
		for (sf::Sprite& sprite : spritesUD)
		{
			if (sprite.getGlobalBounds().top < 0) sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
		}
		for (sf::Sprite& sprite : forFly)
		{
			if (sprite.getGlobalBounds().top < 0) sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
		}
		isVictory = true;
		return;
	}

	for (int i = 0; i < goldSprites.size(); i++)
	{
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(goldSprites[i].getGlobalBounds(), intersection))
		{
			float area = intersection.width * intersection.height;
			if (area >= 650)
			{
				for (int j = 0; j < levelSprites.size(); j++)
				{
					if (levelSprites[j].getGlobalBounds().top == goldSprites[i].getGlobalBounds().top && levelSprites[j].getGlobalBounds().left == goldSprites[i].getGlobalBounds().left)
					{
						levelSprites[j].setTexture(texture20);
					}
				}
				goldSprites.erase(goldSprites.begin() + i);
			}
		}
	}
}

void Game::setSprites()
{
	std::ifstream file(pathToLevel);
	if (!file.is_open()) throw std::exception("Не вдалося відкрити файл :(");

	std::vector<sf::Vector2f> vectorOfPositions;
	for (int i = 0; i < 23; i++)
	{
		for (int j = 0; j < 32; j++)
		{
			vectorOfPositions.push_back(sf::Vector2f(j * 30.0f, i * 30.0f));
		}
	}
	int counter = 0;
	char ch;
	bool mainCharacter = false;
	while (file.get(ch))
	{
		if (!std::isdigit(ch))
		{
			continue;
		}
		if (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9')
		{
			bool udbool = false;
			bool blocksBool = false;
			bool notFly = false;
			bool workout = false;
			bool victoryUDBool = false;
			bool goldSpritesBool = false;
			bool enemy = false;
			bool spaced = false;
			sf::Sprite sprite;
			switch (ch)
			{
			case '0':
				sprite.setTexture(texture20);
				break;
			case '1':
				sprite.setTexture(texture15);
				blocksBool = true;
				notFly = true;
				spaced = true;
				break;
			case '2':
				sprite.setTexture(texture14);
				blocksBool = true;
				notFly = true;
				break;
			case '3':
				sprite.setTexture(texture16);
				udbool = true;
				notFly = true;
				break;
			case '4':
				sprite.setTexture(texture18);
				workout = true;
				break;
			case '5':
				goldSpritesBool = true;
				sprite.setTexture(texture17);
				break;
			case '6':
				sprite.setTexture(texture16);
				victoryUDBool = true;
				break;
			case '7':
				enemy = true;
				sprite.setTexture(texture20);
				break;
			case '8':
				mainPosition = vectorOfPositions[counter];
				counter++;
				continue;
			case '9':
				notFly = true;
				sprite.setTexture(texture19);
				break;
			}
			if (victoryUDBool)
				sprite.setPosition(vectorOfPositions[counter].x, vectorOfPositions[counter].y - 2000);
			else sprite.setPosition(vectorOfPositions[counter]);

			levelSprites.push_back(sprite);

			if (enemy) enemies.push_back(sprite);

			if (spaced) spaceBlocks.push_back(sprite);
			if (goldSpritesBool) goldSprites.push_back(sprite);
			if (udbool || victoryUDBool) spritesUD.push_back(sprite);
			if (blocksBool) blocks.push_back(sprite);
			if (notFly || victoryUDBool) forFly.push_back(sprite);
			if (workout) spritesWorkout.push_back(sprite);
			counter++;
		}
	}
}

bool Game::getOpen() const
{
	return open;
}

void Game::setText(int level)
{
	if (!font.loadFromFile("src/font.ttf")) throw std::exception("Не вдалося загрузити шрифт :(");
	text.setFont(font);
	text.setString(std::to_string(level));
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(32.0f * 30.0f / 2.0f, 23.0f * 30.0f);
	text.setCharacterSize(24);
	text.setStyle(sf::Text::Bold);
}

void Game::smoothTextures()
{
	texture0.setSmooth(true);
	texture1.setSmooth(true);
	texture2.setSmooth(true);
	texture3.setSmooth(true);
	texture4.setSmooth(true);
	texture5.setSmooth(true);
	texture6.setSmooth(true);
	texture7.setSmooth(true);
	texture8.setSmooth(true);
	texture9.setSmooth(true);
	texture10.setSmooth(true);
	texture11.setSmooth(true);
	texture12.setSmooth(true);
	texture13.setSmooth(true);
	texture14.setSmooth(true);
	texture15.setSmooth(true);
	texture16.setSmooth(true);
	texture17.setSmooth(true);
	texture18.setSmooth(true);
	texture19.setSmooth(true);
	texture20.setSmooth(true);
	texture21.setSmooth(true);
	texture22.setSmooth(true);
	texture23.setSmooth(true);
	texture24.setSmooth(true);
	texture25.setSmooth(true);
	texture26.setSmooth(true);
	texture27.setSmooth(true);
	texture28.setSmooth(true);
	texture29.setSmooth(true);
	texture30.setSmooth(true);
	texture31.setSmooth(true);
	texture32.setSmooth(true);
	texture33.setSmooth(true);
	texture34.setSmooth(true);
	texture35.setSmooth(true);
	texture36.setSmooth(true);
	texture37.setSmooth(true);
	texture38.setSmooth(true);
	texture39.setSmooth(true);
	texture40.setSmooth(true);
	texture41.setSmooth(true);
	texture42.setSmooth(true);
	texture43.setSmooth(true);
	texture44.setSmooth(true);
	texture45.setSmooth(true);
	texture46.setSmooth(true);
	texture47.setSmooth(true);
	texture48.setSmooth(true);
	texture49.setSmooth(true);
	texture50.setSmooth(true);
}

void Game::setSprite1()
{
	sprite1.setOrigin(15, 15);
	sprite1.setTexture(texture0);
	sprite1.setPosition(mainPosition.x + 15, mainPosition.y + 15);
}

bool Game::updateFly(sf::Time deltaTime)
{
	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (
			sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds())
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 1
			&& sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top + 1
			)
		{
			isFromFly = true;
			if (sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top) isFromFly = false;
			sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 15);
			return false;
		}
	}

	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds())) return false;
	}

	for (sf::Sprite& sprite : forFly)
	{
		if (
			sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left - 30.0
			&& sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left + 30.0
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 30.5
			&& sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - 30.0
			)
		{
			return false;
		}
	}
	sprite1.setTexture(texture13);
	sprite1.move(0, deltaTime.asSeconds() * fabs(mainSpeed));
	for (sf::Sprite& sprite : forFly)
	{
		if (sprite.getGlobalBounds().contains(sf::Vector2f(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 32)))
		{
			sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
		}
	}
	return true;
}

void Game::updateMoveUD(sf::Time deltaTime)
{
	if (movingRight || movingLeft) return;
	static bool tempIgnore = false;
	if ((!movingUp && !movingDown) || (movingUp && movingDown))
	{
		if (tempIgnore) sprite1.setTexture(texture0);
		return;
	}

	bool temp = true;
	sf::Sprite spriteToCenter;

	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds())
			&& sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left - 15
			&& sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left + 15
			)
		{
			for (sf::Sprite& block : blocks)
			{
				if (movingDown && block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 32))
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, block.getGlobalBounds().top - 15);
					return;
				}
				if (movingUp && block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top - 2))
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, block.getGlobalBounds().top + 45);
					return;
				}
			}
			temp = false;
			spriteToCenter = sprite;
		}
		if (sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left - 15 && sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left + 15
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 31 && sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - 29
			&& temp == true)
		{
			sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
			spriteToCenter = sprite;
			if (movingDown)
			{
				mainSpeed = fabs(mainSpeed);
				centrelizing(spriteToCenter);
				if (ignoreUD)
				{
					sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
					animateLR();
					tempIgnore = true;
					return;
				}
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				animateUD();
			}
			return;
		}
	}

	if (temp) return;

	if (movingUp) mainSpeed = -(fabs(mainSpeed));
	if (movingDown) mainSpeed = fabs(mainSpeed);

	centrelizing(spriteToCenter);

	if (ignoreUD)
	{
		tempIgnore = true;
		sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
		animateLR();
		return;
	}
	tempIgnore = false;

	sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
	animateUD();
}

void Game::centrelizing(const sf::Sprite sprite)
{
	if (sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - 1 || sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left + 1)
	{
		mainSpeed = sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - 1 ? fabs(mainSpeed) : -fabs(mainSpeed);
		sprite1.setScale((float)sprite1.getGlobalBounds().left <= (float)sprite.getGlobalBounds().left - 1.0f ? 1.0f : -1.0f, 1.0f);
		ignoreUD = true;
		return;
	}
	sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
	ignoreUD = false;
}

void Game::animateUD()
{
	sf::Time elapsedTime = clockAnimationUD.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalUD)
	{
		frameIndexUD = (frameIndexUD + 1) % static_cast<int>(texturesToMoveUD.size());
		sprite1.setTexture(texturesToMoveUD[frameIndexUD]);
		clockAnimationUD.restart();
	}
}

void Game::initVariables()
{
	isFromFly = false;
	ignoreNextLevel = false;
	end = false;
	space = false;
	isVictory = false;
	isWorkout = false;
	open = true;
	ignoreUD = false;
	moveLR = false;
	movingRight = false;
	movingLeft = false;
	movingUp = false;
	movingDown = false;
	mainSpeed = 120.0f;
	frameIndexLR = 0;
	frameIndexUD = 0;
	frameIndexWorkout = 0;
	deletedBlockInterval = 4.0f;
	reawakenedInterval = 40.0f;
	miniAnimateInterval = 25.0f;
	animationMoveIntervalLR = 40.0f;
	animationMoveIntervalUD = 35.0f;
	animationMoveIntervalWorkout = 40.0f;
	texturesToMoveLR = { texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, texture10, texture11, texture12 };
	texturesToMoveUD = { texture21, texture22, texture23, texture24, texture25, texture26, texture25, texture24, texture23, texture22, texture21, texture27 };
	texturesForWorkout = { texture28, texture29, texture30, texture31, texture32, texture31, texture30, texture29 };
	deletedTextures = { texture33, texture34, texture35, texture36, texture37, texture38, texture39, texture20 };
	reawakenedTextures = { texture40, texture41, texture42, texture43, texture44, texture45, texture46, texture47, texture48, texture49, texture50, texture51, texture15 };
}

void Game::setWindow(sf::Vector2i windowPosition)
{
	name = "Lode Runner";
	int placeForExpr = 20;
	unsigned int w = 32 * 30, h = 23 * 30;
	desktop = sf::VideoMode(w, h + placeForExpr);

	sf::VideoMode bestMode;
	for (sf::VideoMode mode : sf::VideoMode::getFullscreenModes())
	{
		if (mode.height >= sf::VideoMode::getDesktopMode().height - 2 * placeForExpr) continue;
		bestMode = mode;
		break;
	}

	double k = static_cast<double>(bestMode.height) / static_cast<double>(h);
	double width = k * static_cast<double>(w);

	for (int i = 0; i < 5; i++) window.create(sf::VideoMode(0, 0), "", sf::Style::None);
	window.create(desktop, name, sf::Style::Close | sf::Style::Titlebar);
	window.setPosition(windowPosition);
	window.setSize(sf::Vector2u(static_cast<unsigned int>(width), bestMode.height + placeForExpr));
}

sf::Vector2i Game::getWinpos() const
{
	return winpos;
}

void Game::setIcon()
{
	std::string path = "sprites/icon.png";
	if (!windowIcon.loadFromFile(path))
	{
		throw std::exception("Не вдалося загрузити картинку :(");
	}
	window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, windowIcon.getPixelsPtr());
}

void Game::updateMoveLR(sf::Time deltaTime)
{
	for (sf::Sprite& sprite : spritesUD)
	{
		if (
			sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left &&
			(sprite1.getPosition().x <= 15.5 && movingLeft || sprite1.getPosition().x >= 32 * 30 - 15 && movingRight)
			&& sprite.getGlobalBounds().contains(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 31)
			) return;
		if (
			sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds())
			&& sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left
			)
		{
			if ((movingLeft || movingRight) && sprite1.getGlobalBounds().top + 15 >= sprite.getGlobalBounds().top - 15 && sprite1.getGlobalBounds().top + 15 <= sprite.getGlobalBounds().top)
			{
				for (sf::Sprite& spr : spritesUD)
				{
					if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return;
				}
				mainSpeed = -fabs(mainSpeed);
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				animateUD();
				if (sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 31 && sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - 29)
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
				}
				return;
			}
			if (movingLeft && sprite1.getPosition().x <= 15.5 || movingRight && sprite1.getPosition().x >= 32 * 30 - 15) return;
			if (
				sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top
				&& sprite.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15)
				&& (movingRight || movingLeft)
				)
			{
				for (sf::Sprite& block : blocks)
				{
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 0.01f, sprite1.getGlobalBounds().top + 15.0f) && movingLeft) return;
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.01f, sprite1.getGlobalBounds().top + 15.0f) && movingRight) return;
				}
				mainSpeed = sprite1.getGlobalBounds().top + 15 > sprite.getGlobalBounds().top + 15 ? -fabs(mainSpeed) : fabs(mainSpeed);
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				animateUD();
				if (sprite1.getGlobalBounds().top > sprite.getGlobalBounds().top - 1 && sprite1.getGlobalBounds().top < sprite.getGlobalBounds().top + 1)
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 15);
				}
				return;
			}
		}
	}

	if (((!movingLeft && !movingRight) || (movingLeft && movingRight)))
	{
		if (moveLR && !isWorkout)
		{
			sprite1.setTexture(texture0);
		}
		moveLR = false;
		return;
	}

	bool onUD = false;
	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left)
		{
			onUD = true;
			break;
		}
	}

	if (movingRight)
	{
		if (sprite1.getPosition().x >= 32 * 30 - 15 || checkRight())
		{
			if (!onUD) sprite1.setTexture(texture0);
			return;
		}
		moveLR = true;
		mainSpeed = fabs(mainSpeed);
		sprite1.setScale(1, 1);
	}

	if (movingLeft)
	{
		if (sprite1.getPosition().x <= 15.5 || checkLeft())
		{
			if (!onUD) sprite1.setTexture(texture0);
			return;
		}
		moveLR = true;
		mainSpeed = -(fabs(mainSpeed));
		sprite1.setScale(-1, 1);
	}

	sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
	isWorkout = false;
	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top)
		{
			isFromFly = false;
			isWorkout = true;
			animateWorkout();
			return;
		}
	}
	animateLR();
}

bool Game::checkRight()
{
	for (sf::Sprite& block : blocks)
	{
		if (
			block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 31.0f, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 31.0f, sprite1.getGlobalBounds().top + 29.999f)
			)
		{
			return true;
		}
	}
	return false;
}

bool Game::checkLeft()
{
	for (sf::Sprite& block : blocks)
	{
		if (
			block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 1.0f, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 1.0f, sprite1.getGlobalBounds().top + 29.999f)
			)
		{
			return true;
		}
	}
	return false;
}

void Game::animateLR()
{
	sf::Time elapsedTime = clockAnimationLR.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalLR)
	{
		frameIndexLR = (frameIndexLR + 1) % static_cast<int>(texturesToMoveLR.size());
		sprite1.setTexture(texturesToMoveLR[frameIndexLR]);
		clockAnimationLR.restart();
	}
}

void Game::animateWorkout()
{
	sf::Time elapsedTime = clockAnimationWorkout.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalWorkout)
	{
		frameIndexWorkout = (frameIndexWorkout + 1) % static_cast<int>(texturesForWorkout.size());
		sprite1.setTexture(texturesForWorkout[frameIndexWorkout]);
		clockAnimationWorkout.restart();
	}
}

void Game::loadTextures()
{
	if (
		!texture0.loadFromFile("sprites/move/0.png") ||
		!texture1.loadFromFile("sprites/move/1.png") ||
		!texture2.loadFromFile("sprites/move/2.png") ||
		!texture3.loadFromFile("sprites/move/3.png") ||
		!texture4.loadFromFile("sprites/move/4.png") ||
		!texture5.loadFromFile("sprites/move/5.png") ||
		!texture6.loadFromFile("sprites/move/6.png") ||
		!texture7.loadFromFile("sprites/move/7.png") ||
		!texture8.loadFromFile("sprites/move/8.png") ||
		!texture9.loadFromFile("sprites/move/9.png") ||
		!texture10.loadFromFile("sprites/move/10.png") ||
		!texture11.loadFromFile("sprites/move/11.png") ||
		!texture12.loadFromFile("sprites/move/12.png") ||
		!texture13.loadFromFile("sprites/move/13.png") ||
		!texture14.loadFromFile("sprites/elements/1.png") ||
		!texture15.loadFromFile("sprites/elements/2.png") ||
		!texture16.loadFromFile("sprites/elements/3.png") ||
		!texture17.loadFromFile("sprites/elements/4.png") ||
		!texture18.loadFromFile("sprites/elements/5.png") ||
		!texture19.loadFromFile("sprites/elements/6.png") ||
		!texture20.loadFromFile("sprites/elements/7.png") ||
		!texture21.loadFromFile("sprites/move/14.png") ||
		!texture22.loadFromFile("sprites/move/15.png") ||
		!texture23.loadFromFile("sprites/move/16.png") ||
		!texture24.loadFromFile("sprites/move/17.png") ||
		!texture25.loadFromFile("sprites/move/18.png") ||
		!texture26.loadFromFile("sprites/move/19.png") ||
		!texture27.loadFromFile("sprites/move/20.png") ||
		!texture28.loadFromFile("sprites/move/21.png") ||
		!texture29.loadFromFile("sprites/move/22.png") ||
		!texture30.loadFromFile("sprites/move/23.png") ||
		!texture31.loadFromFile("sprites/move/24.png") ||
		!texture32.loadFromFile("sprites/move/25.png") ||
		!texture33.loadFromFile("sprites/elements/8.png") ||
		!texture34.loadFromFile("sprites/elements/9.png") ||
		!texture35.loadFromFile("sprites/elements/10.png") ||
		!texture36.loadFromFile("sprites/elements/11.png") ||
		!texture37.loadFromFile("sprites/elements/12.png") ||
		!texture38.loadFromFile("sprites/elements/13.png") ||
		!texture39.loadFromFile("sprites/elements/14.png") ||
		!texture40.loadFromFile("sprites/elements/15.png") ||
		!texture41.loadFromFile("sprites/elements/16.png") ||
		!texture42.loadFromFile("sprites/elements/17.png") ||
		!texture43.loadFromFile("sprites/elements/18.png") ||
		!texture44.loadFromFile("sprites/elements/19.png") ||
		!texture45.loadFromFile("sprites/elements/20.png") ||
		!texture46.loadFromFile("sprites/elements/21.png") ||
		!texture47.loadFromFile("sprites/elements/22.png") ||
		!texture48.loadFromFile("sprites/elements/23.png") ||
		!texture49.loadFromFile("sprites/elements/24.png") ||
		!texture50.loadFromFile("sprites/elements/25.png") ||
		!texture51.loadFromFile("sprites/elements/26.png")
		)
	{
		throw std::exception("Не вдалося загрузити картинку :(");
	}
}

void Game::setEnemies()
{
	for (sf::Sprite& enemy : enemies)
	{
		enemy.setTexture(texture13);
		enemy.setColor(sf::Color::Magenta);
	}
}


