
#include "Game.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////          MAIN FUNCTIONS START             /////////////////

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

	if (updateHelp(deltaTime)) return;

	if (updateFly())
	{
		sprite1.move(0, fabs(mainSpeed) * deltaTime.asSeconds());
		sprite1.setTexture(texture13);
		isFromFly = true;
		return;
	}

	updateMoveLR(deltaTime);
	updateMoveUD(deltaTime);
	updateSpace(deltaTime);

	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top)
		{
			isFromFly = false;
			break;
		}
	}
	
	if (isFromFly) sprite1.setTexture(texture13);
	isFromFly = false;
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

//////////////////////////////            MAIN FUNCTIONS END            //////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Game::Game(std::string path, int level, sf::Vector2i windowPosition, sf::Vector2u windowSize)
{
	pathToLevel = path;
	loadTextures();
	initVariables();
	setWindow(windowPosition, windowSize);
	setIcon();
	setSprites();
	setSprite1();
	setEnemies();
	smoothTextures();
	setText(level);
}

bool Game::updateHelp(sf::Time& deltaTime)
{
	if (movingLeft && movingRight || movingUp && movingDown || !movingLeft && !movingRight || !movingUp && !movingDown) return false;

	bool isHelp = false;

	sf::RectangleShape rect(sf::Vector2f(30.0f, 2.0f));
	rect.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 28);

	for (sf::Sprite& sprite : spritesUD)
	{
		if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds()) && movingUp 
			&& rect.getGlobalBounds().left >= sprite.getGlobalBounds().left - 14 && rect.getGlobalBounds().left <= sprite.getGlobalBounds().left + 14)
		{
			if (sprite1.getGlobalBounds().left != sprite.getGlobalBounds().left)
			{
				
			}
		}
	}


	return isHelp;
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
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection))
		{
			if (intersection.height * intersection.width >= 60)
			{
				end = true;
				return;
			}
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
					for (sf::Sprite& sprite : forFly)
					{
						//if (spaced.getGlobalBounds().contains(sprite.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 45)) return;
					}
					/*for (sf::Sprite& sprite : spritesWorkout)
					{
						if (sprite.getGlobalBounds().contains(spaced.getGlobalBounds().left + 15, spaced.getGlobalBounds().top) + 45) return;
					}*/
					if (sprite1.getGlobalBounds().left + 20 >= block.getGlobalBounds().left + 15 && sprite1.getGlobalBounds().left + 20 <= block.getGlobalBounds().left + 30
						&& spaced.getGlobalBounds().contains(block.getGlobalBounds().left + 45, block.getGlobalBounds().top + 15) && vector == 1)
					{
						sprite1.setPosition(block.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
						removeBlock(spaced);
						return;
					}
					if (sprite1.getGlobalBounds().left + 15 >= block.getGlobalBounds().left && sprite1.getGlobalBounds().left + 15 <= block.getGlobalBounds().left + 15
						&& spaced.getGlobalBounds().contains(block.getGlobalBounds().left - 15, block.getGlobalBounds().top + 15) && vector == -1)
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
			if (sprite.getGlobalBounds().top < 0)
			{
				bool temp = false;
				for (sf::Sprite& spr : spaceBlocks) 
				{
					if (spr.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
					{
						temp = true;
						break;
					}
				}
				if (temp) continue;
				sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
			}
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
			if (area >= 800)
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
	while (file.get(ch))
	{
		if (!std::isdigit(ch) && ch != 'i') continue;

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
		case 'i':
			sprite.setTexture(texture15);
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

bool Game::updateFly()
{
	sf::RectangleShape rect(sf::Vector2f(30.0f, 2.0f));
	rect.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);

	sf::FloatRect intersection;
	for (sf::Sprite& sprite : forFly)
	{
		bool temp = false;

		for (sf::Sprite& sp : spritesWorkout)
		{

			sf::FloatRect area;
			if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width < 2)
			{
				int count = 0;
				for (sf::Sprite& s : spritesWorkout)
				{
					if (sprite1.getGlobalBounds().intersects(s.getGlobalBounds()) && sp.getGlobalBounds().getPosition() != s.getGlobalBounds().getPosition())
					{
						count++;
						break;
					}
				}
				if (count != 0)
				{
					temp = true;
					break;
				}
			}
		}

		if (temp) break;

		for (sf::Sprite& sp : spritesUD)
		{
			sf::FloatRect area;
			if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width * area.height >= 450)
			{
				temp = true;
				break;
			}
			if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width < 2)
			{
				int count = 0;
				for (sf::Sprite& s : spritesUD)
				{
					if (sprite1.getGlobalBounds().intersects(s.getGlobalBounds()) && sp.getGlobalBounds().getPosition() != s.getGlobalBounds().getPosition())
					{
						count++;
						break;
					}
				}
				for (sf::Sprite& spri : spritesWorkout)
				{
					sf::FloatRect intersec;
					if (sprite1.getGlobalBounds().intersects(spri.getGlobalBounds(), intersec) && intersec.width * intersec.height >= 450)
					{
						count++;
						break;
					}
				}
				if (count != 0)
				{
					temp = true;
					break;
				}
			}
		}
		
		if (temp) break;
		
		if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.width < 2)
		{
			bool work = false;
			for (sf::Sprite& sprt : spritesWorkout)
			{
				sf::FloatRect area;
				if (sprite1.getGlobalBounds().intersects(sprt.getGlobalBounds(), area) && area.width > 28)
				{
					work = true;
					break;
				}
			}
			if (work) return false;
			int counts = 0;
			for (sf::Sprite& spr : forFly)
			{
				if (rect.getGlobalBounds().intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprite.getGlobalBounds().getPosition())
				{
					counts++;
					break;
				}
			}
			if (counts == 0 && rect.getGlobalBounds().top == sprite.getGlobalBounds().top)
			{
				int left = static_cast<int>(rect.getGlobalBounds().left);
				int mod = left % 30;
				if (mod > 15)
				{
					mod = 30 - mod;
					left += mod;
				}
				else if (mod < 15) left -= mod;

				sprite1.setPosition(static_cast<float>(left) + 15, sprite1.getGlobalBounds().top + 15);
				return true;
			}
		}
	}

	for (sf::Sprite& sprite : spritesWorkout)
	{
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection)
			&& sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top + 0.75
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 0.75)
		{
			if (sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top) 
				sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 15);

			if (movingDown)
			{ 
				if (movingUp) return false;
				for (sf::Sprite& spr : blocks)
				{
					if (rect.getGlobalBounds().intersects(spr.getGlobalBounds())) return false;
				}
				for (sf::Sprite& spr : spritesUD) 
				{
					if (rect.getGlobalBounds().intersects(spr.getGlobalBounds()) && (rect.getGlobalBounds().left != spr.getGlobalBounds().left || movingLeft || movingRight)) return false;
				}
				sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15.76f);
			}
			sf::FloatRect area;
			if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), area) && area.width < 2)
			{
				int counts = 0;
				for (sf::Sprite& spr : spritesWorkout)
				{
					if (sprite1.getGlobalBounds().intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprite.getGlobalBounds().getPosition())
					{
						counts++;
						break;
					}
				}
				for (sf::Sprite& spr : forFly)
				{
					sf::FloatRect rct;
					if (rect.getGlobalBounds().intersects(spr.getGlobalBounds()) || sprite1.getGlobalBounds().intersects(spr.getGlobalBounds()))
					{
						counts++;
						break;
					}
				}
				if (counts == 0)
				{
					float left = sprite1.getGlobalBounds().left > sprite.getGlobalBounds().left ? sprite.getGlobalBounds().left + 30 : sprite.getGlobalBounds().left - 30;
					sprite1.setPosition(left + 15, sprite1.getGlobalBounds().top + 15);
					return true;
				}
			}
			return false;
		}
	}

	for (sf::Sprite& sprite : spritesUD)
	{
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection))
		{
			if (intersection.width * intersection.height <= 450 
				&& sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left 
				&& sprite1.getGlobalBounds().top > sprite.getGlobalBounds().top)
			{
				int counter = 0;
				for (sf::Sprite& ladder : spritesUD)
				{
					if (sprite1.getGlobalBounds().intersects(ladder.getGlobalBounds()) && ladder.getGlobalBounds().getPosition() != sprite.getGlobalBounds().getPosition())
					{
						counter++;
						break;
					}
				}
				if (counter == 0) return true;
			}
			if (sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left && intersection.height < 1 && sprite1.getGlobalBounds().top < sprite.getGlobalBounds().top)
			{
				sf::Image img1 = (*sprite1.getTexture()).copyToImage();
				sf::Image img2 = texture13.copyToImage();
				for (int i = 0; i < 30; i++) {
					for (int j = 0; j < 30; j++) {
						if (img1.getPixel(i, j) != img2.getPixel(i, j)) return false;
					}
				}
				if (sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top - 30) sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
			}
			if (intersection.width < 2)
			{
				int counts = 0;
				for (sf::Sprite& spr : spritesUD)
				{
					if (sprite1.getGlobalBounds().intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprite.getGlobalBounds().getPosition())
					{
						counts++;
						break;
					}
				}
				for (sf::Sprite& spr : forFly)
				{
					if (rect.getGlobalBounds().intersects(spr.getGlobalBounds()))
					{
						counts++;
						break;
					}
				}
				if (counts == 0)
				{
					float left = sprite1.getGlobalBounds().left > sprite.getGlobalBounds().left ? sprite.getGlobalBounds().left + 30 : sprite.getGlobalBounds().left - 30;
					sprite1.setPosition(left + 15, sprite1.getGlobalBounds().top + 15);
					return true;
				}
			}
			return false;
		}
	}

	int tmpCounter = 0;
	for (sf::Sprite& sprite : forFly)
	{
		if (!rect.getGlobalBounds().intersects(sprite.getGlobalBounds())) tmpCounter++;
	}
	if (tmpCounter == forFly.size()) return true;

	for (sf::Sprite& sprite : forFly)
	{
		if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds()))
		{
			sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
			return false;
		}
	}

	return false;
}

void Game::updateMoveUD(sf::Time deltaTime)
{
	bool tmpLR = true;
	if (sprite1.getGlobalBounds().left == 0 && movingLeft || sprite1.getGlobalBounds().left == 32 * 30 - 30 && movingRight) tmpLR = false;
	for (sf::Sprite& block : blocks)
	{
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 0.01f, sprite1.getGlobalBounds().top + 15.0f) && movingLeft) { tmpLR = false; break; }
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.01f, sprite1.getGlobalBounds().top + 15.0f) && movingRight) { tmpLR = false; break; }
	}

	if ((movingRight || movingLeft) && tmpLR) return;

	static bool tempIgnore = false;
	if ((!movingUp && !movingDown) || (movingUp && movingDown))
	{
		bool tmp = true;
		for (sf::Sprite& sprite : spritesWorkout)
		{
			if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
			{
				tmp = false;
				break;
			}
		}
		if (tempIgnore && tmp) sprite1.setTexture(texture0);
		return;
	}

	bool temp = true;
	sf::Sprite spriteToCenter;

	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left  && sprite1.getGlobalBounds().top + 30 >= sprite.getGlobalBounds().top
			&& sprite1.getGlobalBounds().top + 30 <= sprite.getGlobalBounds().top + 1 && movingUp)
		{
			bool temp = true;
			for (sf::Sprite& sp : spritesUD)
			{
				sf::FloatRect intersection;
				if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), intersection))
				{
					if (intersection.width * intersection.height > 750)
					{
						temp = false;
						break;
					}
				}
			}
			if (temp && sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top - 30)
			{
				sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
				return;
			}
		}

		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) 
			&& sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left - 15 && sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left + 15)
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
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 31 && sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - 29 && temp == true)
		{
			spriteToCenter = sprite;
			if (movingDown)
			{
				mainSpeed = fabs(mainSpeed);
				centrelizing(spriteToCenter);
				if (ignoreUD)
				{
					sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
					bool tmp = false;
					for (sf::Sprite& sprite : spritesWorkout)
					{
						if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
						{
							tmp = true;
							break;
						}
					}
					tmp ? animateWorkout() : animateLR();
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
		bool tmp = false;
		for (sf::Sprite& sprite : spritesWorkout)
		{
			if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
			{
				tmp = true;
				break;
			}
		}
		tmp ? animateWorkout() : animateLR();
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
	mainSpeed = 125.0f;
	//mainSpeed = 0;
	frameIndexLR = 0;
	frameIndexUD = 0;
	frameIndexWorkout = 0;
	deletedBlockInterval = 9.0f;
	reawakenedInterval = 40.0f;
	miniAnimateInterval = 15.0f;
	animationMoveIntervalLR = 35.0f;
	//animationMoveIntervalLR = 350.0f;
	animationMoveIntervalUD = 30.0f;
	animationMoveIntervalWorkout = 35.0f;
	texturesToMoveLR = { texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8, texture9, texture10, texture11, texture12 };
	texturesToMoveUD = { texture21, texture22, texture23, texture24, texture25, texture26, texture25, texture24, texture23, texture22, texture21, texture27 };
	texturesForWorkout = { texture28, texture29, texture30, texture31, texture32, texture31, texture30, texture29 };
	deletedTextures = { texture33, texture34, texture35, texture36, texture37, texture38, texture39, texture20 };
	reawakenedTextures = { texture40, texture41, texture42, texture43, texture44, texture45, texture46, texture47, texture48, texture49, texture50, texture51, texture15 };
}

void Game::setWindow(sf::Vector2i windowPosition, sf::Vector2u windowSize)
{
	for (int i = 0; i < 5; i++) window.create(sf::VideoMode(0, 0), "", sf::Style::None);
	window.create(sf::VideoMode(32 * 30, 23 * 30 + 20), "Lode Runner");
	window.setPosition(windowPosition);
	window.setSize(windowSize);
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
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left)
		{
			if ((movingLeft || movingRight) && sprite1.getGlobalBounds().top + 15 >= sprite.getGlobalBounds().top - 15 
				&& sprite1.getGlobalBounds().top + 15 <= sprite.getGlobalBounds().top)
			{
				if (movingLeft && sprite1.getPosition().x <= 15.5 || movingRight && sprite1.getPosition().x >= 32 * 30 - 15) return;
				for (sf::Sprite& block : blocks)
				{
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 0.01f, sprite1.getGlobalBounds().top + 15.0f) && movingLeft) return;
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.01f, sprite1.getGlobalBounds().top + 15.0f) && movingRight) return;
				}
				for (sf::Sprite& spr : spritesUD)
				{
					if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return;
				}
				mainSpeed = -fabs(mainSpeed);
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				if (sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - 31 && sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - 29)
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
				}
				return;
			}
			if (movingLeft && sprite1.getPosition().x <= 15.5 || movingRight && sprite1.getPosition().x >= 32 * 30 - 15) return;
			if (sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top
				&& sprite.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15) && (movingRight || movingLeft))
			{
				for (sf::Sprite& block : blocks)
				{
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 0.01f, sprite1.getGlobalBounds().top + 15.0f) && movingLeft) return;
					if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.01f, sprite1.getGlobalBounds().top + 15.0f) && movingRight) return;
				}
				mainSpeed = sprite1.getGlobalBounds().top + 15 > sprite.getGlobalBounds().top + 15 ? -fabs(mainSpeed) : fabs(mainSpeed);
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				if (sprite1.getGlobalBounds().top > sprite.getGlobalBounds().top - 1 && sprite1.getGlobalBounds().top < sprite.getGlobalBounds().top + 1)
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 15);
				}
				return;
			}
		}
	}
	if (checkLeft() && checkRight()) return;
	sf::Image image1 = sprite1.getTexture()->copyToImage();
	sf::Image image2 = texture13.copyToImage();
	int counts = 0;
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (image1.getPixel(i, j) == image2.getPixel(i, j)) counts++;
		}
	}
	if ((movingLeft && checkLeft() || movingRight && checkRight()) && counts == 900) return;

	bool onUD = false;
	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left)
		{
			onUD = true;
			break;
		}
	}
	bool onHorizontalLadder = false;
	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
		{
			onHorizontalLadder = true;
			break;
		}
	}
	
	bool isLadderTexture = false;
	sf::Image img1 = sprite1.getTexture()->copyToImage();
	for (sf::Texture& texture : texturesToMoveUD)
	{
		sf::Image img2 = texture.copyToImage();
		int counter = 0;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				if (img1.getPixel(i, j) == img2.getPixel(i, j)) counter++;
			}
		}
		if (counter == 900)
		{
			isLadderTexture = true;
			break;
		}
	}

	if (((!movingLeft && !movingRight) || (movingLeft && movingRight)))
	{
		if (moveLR && !isWorkout)
		{
			bool lol = false;
			if ((checkLeft() || checkRight()) && counts == 900) lol = true;
			if (!onUD && !isLadderTexture && !lol) sprite1.setTexture(texture0);
			for (sf::Sprite& sprite : spritesWorkout)
			{
				if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top) sprite1.setTexture(texture13);
			}
		}
		moveLR = false;
		return;
	}

	int left = static_cast<int>(sprite1.getGlobalBounds().left);
	int mod = left % 30;
	if (mod > 15)
	{
		mod = 30 - mod;
		left += mod;
	}
	else if (mod < 15) left -= mod;

	if (movingRight)
	{
		if (sprite1.getPosition().x >= 32 * 30 - 15 || checkRight())
		{
			if (!onUD && !onHorizontalLadder && !isLadderTexture) sprite1.setTexture(texture0);
			sprite1.setPosition(float(left) + 15, sprite1.getGlobalBounds().top + 15);
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
			if (!onUD && !onHorizontalLadder && !isLadderTexture) sprite1.setTexture(texture0);
			sprite1.setPosition(float(left) + 15, sprite1.getGlobalBounds().top + 15);
			return;
		}
		moveLR = true;
		mainSpeed = -(fabs(mainSpeed));
		sprite1.setScale(-1, 1);
	}

	isWorkout = false;
	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top)
		{
			isWorkout = true;
			animateWorkout();
			sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
			return;
		}
	}

	animateLR();
	sprite1.move(deltaTime.asSeconds()* mainSpeed, 0);
}

bool Game::checkRight()
{
	for (sf::Sprite& block : blocks)
	{
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 31.0f, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 31.0f, sprite1.getGlobalBounds().top + 29.999f))
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
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 1.0f, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 1.0f, sprite1.getGlobalBounds().top + 29.999f))
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


