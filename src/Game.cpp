
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
			break;
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Key::Escape) isRestart = true;
			if (event.key.code == sf::Keyboard::Key::Right) movingRight = true;
			if (event.key.code == sf::Keyboard::Key::Left) movingLeft = true;
			if (event.key.code == sf::Keyboard::Key::Up) movingUp = true;
			if (event.key.code == sf::Keyboard::Key::Down) movingDown = true;
			if (event.key.code == sf::Keyboard::Key::Space)	space = true;
			break;
		case sf::Event::KeyReleased:
			if (event.key.code == sf::Keyboard::Key::Right) movingRight = false;
			if (event.key.code == sf::Keyboard::Key::Left) movingLeft = false;
			if (event.key.code == sf::Keyboard::Key::Up) movingUp = false;
			if (event.key.code == sf::Keyboard::Key::Down) movingDown = false;
			if (event.key.code == sf::Keyboard::Key::Space) space = false;
			break;
		}
	}
}

void Game::update()
{	
	//std::cout << sprite1.getGlobalBounds().left << "\t" << sprite1.getGlobalBounds().top << "\n";
	//window.setFramerateLimit(70);

	if (window.hasFocus()) updateFPS();
	if (!window.hasFocus())
	{
		movingDown = false;
		movingUp = false;
		movingLeft = false;
		movingRight = false;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		deltaTimeClock.restart();
		fps = 45;
	}

	updateDeath();
	if (sprite1.getGlobalBounds().top + 30 <= 0) { isWin = true; return; }
	if (isRestart || isWin) return;

	if (window.hasFocus()) animateDeleted();
	if (!isVictory) updateGold();

	sf::Time deltaTime = deltaTimeClock.restart();
	if (updateFly() && window.hasFocus())
	{
		sprite1.move(0, fabs(mainSpeed) * deltaTime.asSeconds());
		sprite1.setTexture(texture13);
		isFromFly = true;
		return;
	}
	//if (updateHelp(deltaTime)) return;
	updateMoveLR(deltaTime);
	updateMoveUD(deltaTime);
	updateSpace(deltaTime);
	updateFlyTexture();
}

void Game::draw()
{
	window.clear(sf::Color::Black);

	drawLevel();
	if (isWin || isRestart) drawTransition();

	window.display();
}

//////////////////////////////            MAIN FUNCTIONS END            //////////////////
/////////////////////////////////////////////////////////////////////////////////////////

Game::Game()
{
	font.loadFromMemory(font_ttf, font_ttf_len);
	loadTextures();
	initVariables();
	setWindow();
	setIcon();
	setText(level);
	setSprites(level);
	setSprite1();
	setEnemies();
	smoothTextures();
}

void Game::drawLevel()
{
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
}

void Game::updateFPS()
{
	fps++;
	if (framesClock.getElapsedTime().asSeconds() >= 1.0f)
	{
		std::cout << fps << "\n";
		fpsVector.push_back(fps);
		if (fpsVector.size() >= 5)
		{
			float sum = 0;
			for (size_t i = 0; i < fpsVector.size(); i++)
			{
				sum += fpsVector.at(i);
			}
			averageFPS = sum / fpsVector.size();
			fpsVector.clear();
		}
		if (fps >= 60)
		{
			help = 3.0f;
			transitionSpeed = 15;
			reawakenedInterval = 25.0f;
			miniAnimateInterval = 15.0f;
			animationMoveIntervalLR = 32.0f;
			animationMoveIntervalUD = 24.0f;
			animationMoveIntervalWorkout = 40.0f;
		}
		else if (fps >= 40 && fps < 55)
		{
			help = 4.0f;
			transitionSpeed = 20;
			reawakenedInterval = 22.0f;
			miniAnimateInterval = 10.0f;
			animationMoveIntervalLR = 29.0f;
			animationMoveIntervalUD = 21.0f;
			animationMoveIntervalWorkout = 38.0f;
		}
		else if (fps >= 30 && fps < 40)
		{
			help = 5.0f;
			transitionSpeed = 25;
			reawakenedInterval = 21.0f;
			miniAnimateInterval = 8.0f;
			animationMoveIntervalLR = 27.0f;
			animationMoveIntervalUD = 16.0f;
			animationMoveIntervalWorkout = 33.0f;
		}
		else if (fps >= 20 && fps < 30)
		{
			help = 6.0f;
			transitionSpeed = 30;
			reawakenedInterval = 18.0f;
			miniAnimateInterval = 1.0f;
			animationMoveIntervalLR = 32.0f;
			animationMoveIntervalUD = 10.0f;
			animationMoveIntervalWorkout = 36.0f;
		}
		else if (averageFPS > 0 && averageFPS < 20)
			showError(L"Too few frames per second for playing");
		fps = 0;
		framesClock.restart();
	}
}

bool Game::updateHelp(sf::Time& deltaTime)
{
	if (movingLeft && movingRight || movingUp && movingDown || !movingLeft && !movingRight || !movingUp && !movingDown) return false;

	bool isHelp = false;
	float hlp = help / 2;
	//if (fps >= 30 && fps < 40) hlp += 2.0f;

	sf::RectangleShape rectdown(sf::Vector2f(30.0f, help));
	rectdown.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30 - help);

	sf::RectangleShape rectunder(sf::Vector2f(30.0f, help));
	rectunder.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);

	sf::RectangleShape rectop(sf::Vector2f(30.0f, help));
	rectop.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top);

	for (sf::Sprite& sprite : spritesUD)
	{
		if (movingLeft && sprite1.getPosition().x <= 15 + hlp || movingRight && sprite1.getPosition().x >= 32 * 30 - 15 - hlp) return false;
		for (sf::Sprite& block : blocks)
		{
			if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - 0.01f, sprite1.getGlobalBounds().top + 15.0f) && movingLeft) return false;
			if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.01f, sprite1.getGlobalBounds().top + 15.0f) && movingRight) return false;
		}

		if (rectdown.getGlobalBounds().intersects(sprite.getGlobalBounds()) && movingUp 
			&& rectdown.getGlobalBounds().left >= sprite.getGlobalBounds().left - hlp 
			&& rectdown.getGlobalBounds().left <= sprite.getGlobalBounds().left + hlp)
		{
			for (sf::Sprite& block : blocks)
			{
				if (block.getGlobalBounds().contains(sprite.getGlobalBounds().left + 15.0f, sprite.getGlobalBounds().top - 15.0f)) return false;
			}
			if (rectdown.getGlobalBounds().left != sprite.getGlobalBounds().left)
			{
				sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
			}
			else
			{
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * -fabs(mainSpeed));
				if (rectdown.getGlobalBounds().top >= sprite.getGlobalBounds().top - hlp
					&& rectdown.getGlobalBounds().top <= sprite.getGlobalBounds().top + hlp)
				{
					sprite1.setPosition(sprite.getGlobalBounds().left + 15 + (movingRight ? (hlp + 0.01f) : ( -hlp - 0.01f)), sprite.getGlobalBounds().top - 15);
					return false;	
				}
			}
			isHelp = true;
			break;
		}

		if (rectunder.getGlobalBounds().intersects(sprite.getGlobalBounds()) && movingDown
			&& rectunder.getGlobalBounds().left >= sprite.getGlobalBounds().left - hlp && rectunder.getGlobalBounds().left <= sprite.getGlobalBounds().left + hlp)
		{
			if (rectunder.getGlobalBounds().left != sprite.getGlobalBounds().left)
			{
				sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
			}
			else
			{
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * fabs(mainSpeed));
				if (rectunder.getGlobalBounds().top >= sprite.getGlobalBounds().top + 30 - hlp 
					&& rectunder.getGlobalBounds().top <= sprite.getGlobalBounds().top + 30)
				{
					sprite1.setPosition(sprite.getGlobalBounds().left + 15 + (movingRight ? (hlp + 0.01f) : ( -hlp - 0.01f)), sprite.getGlobalBounds().top + 15);
					return false;
				}
			}
			isHelp = true;
			break;
		}

		if (rectop.getGlobalBounds().intersects(sprite.getGlobalBounds()) && movingDown
			&& rectop.getGlobalBounds().left >= sprite.getGlobalBounds().left - hlp && rectop.getGlobalBounds().left <= sprite.getGlobalBounds().left + hlp)
		{
			for (sf::Sprite& block : blocks)
			{
				if (block.getGlobalBounds().contains(sprite.getGlobalBounds().left + 15.0f, sprite.getGlobalBounds().top + 45.0f)) return false;
			}
			if (rectop.getGlobalBounds().left != sprite.getGlobalBounds().left)
			{
				sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
			}
			else
			{
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * fabs(mainSpeed));
				if (rectop.getGlobalBounds().top >= sprite.getGlobalBounds().top + 30 - hlp && rectop.getGlobalBounds().top < sprite.getGlobalBounds().top + 30)
				{
					sprite1.setPosition(sprite.getGlobalBounds().left + 15 + (movingRight ? (hlp + 0.01f) : ( -hlp - 0.01f)), sprite.getGlobalBounds().top + 45);
					return false;
				}
			}
			isHelp = true;
			break;
		}
	}

	return isHelp;
}

void Game::updateDeath()
{
	for (sf::Sprite& sprite : spaceBlocks)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()))
		{
			isRestart = true;
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
				isRestart = true;
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
		for (sf::Sprite& sprite : spaceBlocks)
		{
			sf::RectangleShape left(sf::Vector2f(60.0f, 1.0f)), right(sf::Vector2f(60.0f, 1.0f));
			left.setPosition(sprite.getGlobalBounds().left + 15 - 55, sprite.getGlobalBounds().top - 1.0f);
			right.setPosition(sprite.getGlobalBounds().left + 10, sprite.getGlobalBounds().top - 1.0f);
			sf::FloatRect intersection;
			if ((sprite1.getGlobalBounds().intersects(left.getGlobalBounds(), intersection) && intersection.width == 30.0f && vector == 1)
				|| (sprite1.getGlobalBounds().intersects(right.getGlobalBounds(), intersection) && intersection.width == 30.0f && vector == -1))
			{
				if (checkSpace(sprite)) return;
				if (spaceTime.getElapsedTime().asMilliseconds() < 250) return;
				else spaceTime.restart();
				sf::FloatRect area; 
				sf::RectangleShape under(sf::Vector2f(30.0f, 1.0f));
				under.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);
				if (under.getGlobalBounds().intersects(sprite.getGlobalBounds())) 
				{
					float left = vector == 1 ? sprite.getGlobalBounds().left - 30 : sprite.getGlobalBounds().left + 30;
					sprite1.setPosition(left + 15, sprite1.getGlobalBounds().top + 15);
					sprite1.setTexture(texture0);
				}
				removeBlock(sprite);
				return;
			}
		}
	}
}

bool Game::checkSpace(sf::Sprite& sprite)
{
	if (sprite1.getGlobalBounds().top != sprite.getGlobalBounds().top - 30) return true;
	for (sf::Sprite& spr : forFly)
	{
		if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return true;
	}
	for (sf::Sprite& spr : spritesWorkout)
	{
		if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return true;
	}
	return false;
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
			if (sprite.getGlobalBounds().top < 0)
			{
				sf::Image img1 = texture16.copyToImage();
				sf::Image img2 = sprite.getTexture()->copyToImage();
				int counter = 0;
				for (int i = 0; i < 30; i++)
				{
					for (int j = 0; j < 30; j++)
					{
						if (img1.getPixel(i, j) == img2.getPixel(i, j)) counter++;
					}
				}
				if (counter == 900) sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
			}
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
			if (area >= 700)
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

void Game::setSprites(int level)
{
	levelSprites.clear();
	enemies.clear();
	spaceBlocks.clear();
	goldSprites.clear();
	spritesUD.clear();
	blocks.clear();
	forFly.clear();
	spritesWorkout.clear();

	std::wstring path = L"levels/level" + std::to_wstring(level) + L".txt";
	std::ifstream file(path);
	if (!file.is_open()) showError(L"Error file loading: " + path + L"\nCheck levels/.rules.txt");

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
	bool isCorrect1 = true;
	bool isCorrect2 = true;
	while (file.get(ch))
	{
		if (!std::isdigit(ch) && ch != 'i') continue;

		if (counter < 22 * 32 && ch == '9' && isCorrect1 == true) isCorrect1 = false;
		if (counter >= 22 * 32 && ch != '9' && isCorrect2 == true) isCorrect2 = false;

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

		if (counter == vectorOfPositions.size()) showError(L"Error in " + path + L"\nThe field must be 23 x 32\nCheck levels/.rules.txt");

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
	if (counter != 23 * 32) showError(L"Error in " + path + L"\nThe field must be 23 x 32\nCheck levels/.rules.txt");
	if (isCorrect1 == false) showError(L"Error in " + path + L"\nSymbols '9' should be only at the very bottom\nCheck levels/.rules.txt");
	if (isCorrect2 == false) showError(L"Error in " + path + L"\nThe last row should consist only of '9'\nCheck levels/.rules.txt");
}

void Game::setText(int level)
{
	text.setFont(font);
	text.setString(std::to_string(level));
	text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
	text.setPosition(32.0f * 30.0f / 2.0f, 23.0f * 30.0f);
	text.setCharacterSize(24);
	text.setStyle(sf::Text::Bold);
}

void Game::smoothTextures()
{
	for (sf::Texture& texture : texturesToMoveLR)
	{
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : texturesToMoveUD)
	{
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : texturesForWorkout)
	{
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : deletedTextures)
	{
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : reawakenedTextures)
	{
		texture.setSmooth(true);
	}
	texture0.setSmooth(true);
	texture13.setSmooth(true);
	texture14.setSmooth(true);
	texture15.setSmooth(true);
	texture16.setSmooth(true);
	texture17.setSmooth(true);
	texture18.setSmooth(true);
	texture19.setSmooth(true);
}

void Game::setSprite1()
{
	sprite1.setOrigin(15, 15);
	sprite1.setTexture(texture0);
	sprite1.setPosition(mainPosition.x + 15, mainPosition.y + 15);
}

void Game::updateFlyTexture()
{
	for (sf::Sprite& sprite : spritesWorkout)
	{
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top)
		{
			isFromFly = false;
			break;
		}
	}

	int counter = 0;
	for (sf::Sprite& sprite : spritesUD)
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds())) counter++;
	if (isFromFly && counter == 0) sprite1.setTexture(texture13);
	isFromFly = false;
}

bool Game::updateFly()
{
	sf::RectangleShape rect(sf::Vector2f(30.0f, help));
	rect.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);

	bool tempUD = false;
	for (sf::Sprite& sp : spritesUD)
	{
		sf::FloatRect area;
		if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width * area.height >= 450)
		{
			tempUD = true;
			break;
		}
		if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width < help)
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
				tempUD = true;
				break;
			}
		}
	}

	bool tempWork = false;
	for (sf::Sprite& sp : spritesWorkout)
	{

		sf::FloatRect area;
		if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds(), area) && area.width < help)
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
				tempWork = true;
				break;
			}
		}
	}

	sf::FloatRect intersection;
	for (sf::Sprite& sprite : forFly)
	{
		if (tempWork || tempUD) break;
		
		if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.width < help)
		{
			bool work = false;
			for (sf::Sprite& sprt : spritesWorkout)
			{
				sf::FloatRect area;
				if (sprite1.getGlobalBounds().intersects(sprt.getGlobalBounds(), area) && area.width > 30 - help)
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
			&& sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - help)
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
					if (rect.getGlobalBounds().intersects(spr.getGlobalBounds()) && (rect.getGlobalBounds().left != spr.getGlobalBounds().left)) return false;
				}
				sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15.01f);
			}
			sf::FloatRect area;
			if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), area) && area.width < help)
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
					if (ladder.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 16) 
						&& ladder.getGlobalBounds().getPosition() != sprite.getGlobalBounds().getPosition())
					{
						counter++;
						break;
					}
				}
				if (counter == 0)
				{
					for (sf::Sprite& sprite : forFly)
					{
						if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds()))
						{
							sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
							return false;
						}
					}
					return true;
				}
			}
			if (intersection.width < help)
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
		bool onUD = false;
		for (sf::Sprite& sp : spritesUD)
		{
			if (sprite1.getGlobalBounds().intersects(sp.getGlobalBounds()) && sprite1.getGlobalBounds().left == sp.getGlobalBounds().left)
			{
				onUD = true;
				break;
			}
		}
		bool flew = false;
		sf::Image img1 = texture13.copyToImage();
		sf::Image img2 = sprite1.getTexture()->copyToImage();
		int counter = 0;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				if (img1.getPixel(i, j) == img2.getPixel(i, j)) counter++;
			}
		}
		if (tempIgnore && tmp && !onUD && counter != 900) sprite1.setTexture(texture0);
		return;
	}

	bool temp = true;
	sf::Sprite spriteToCenter;

	for (sf::Sprite& sprite : spritesUD)
	{
		if (sprite1.getGlobalBounds().left == sprite.getGlobalBounds().left  && sprite1.getGlobalBounds().top + 30 >= sprite.getGlobalBounds().top
			&& sprite1.getGlobalBounds().top + 30 <= sprite.getGlobalBounds().top + help && movingUp)
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
				if (movingDown && block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 30 + help))
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, block.getGlobalBounds().top - 15);
					return;
				}
				if (movingUp && block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top - help))
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, block.getGlobalBounds().top + 45);
					return;
				}
			}
			temp = false;
			spriteToCenter = sprite;
		}
		if (sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left - 15 && sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left + 15
			&& sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top - 30 && temp == true)
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
	if (sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help || sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left + help)
	{
		mainSpeed = sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help ? fabs(mainSpeed) : -fabs(mainSpeed);
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
	averageFPS = 0;
	screenFade.setTexture(texture20);
	screenFade.setScale(32.0f, 22.0f);
	transitionSpeed = 15;
	isDrawnFade = false;
	isWin = false;
	isRestart = false;
	opacity = 25;
	level = getLevel();
	framesLimit = 200;
	help = 3.0f;
	isFromFly = false;
	space = false;
	isVictory = false;
	isWorkout = false;
	ignoreUD = false;
	moveLR = false;
	movingRight = false;
	movingLeft = false;
	movingUp = false;
	movingDown = false;
	mainSpeed = 130.0f;
	frameIndexLR = 0;
	frameIndexUD = 0;
	frameIndexWorkout = 0;
	deletedBlockInterval = 7.0f;
	reawakenedInterval = 25.0f;
	miniAnimateInterval = 15.0f;
	animationMoveIntervalLR = 32.0f;
	animationMoveIntervalUD = 20.0f;
	animationMoveIntervalWorkout = 40.0f;
	deletedTextures = { texture33, texture34, texture35, texture36, texture37, texture38, texture39, texture20 };
	reawakenedTextures = { texture40, texture41, texture42, texture43, texture44, texture45, texture46, texture47, texture48, texture49, texture50, texture51, texture15 };
	texturesToMoveLR = { texture1, texture2, texture3, texture4, texture5, texture4, texture3, texture2, texture1, texture10, texture11, texture12 };
	texturesToMoveUD = { texture21, texture22, texture23, texture24, texture25, texture26, texture25, texture24, texture23, texture22, texture21, texture27 };
	texturesForWorkout = { texture28, texture29, texture30, texture31, texture32, texture31, texture30, texture29 };

	int num = 20;

	for (sf::Texture& texture : texturesForWorkout) {
		sf::Image image = texture.copyToImage();
		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 30; j++) {
				sf::Color pixelColor = image.getPixel(i, j);
				if (pixelColor.b < num && pixelColor.g < num && pixelColor.r < num) image.setPixel(i, j, sf::Color::Transparent);
			}
		} 
		texture.loadFromImage(image);
	}
	for (sf::Texture& texture : texturesToMoveLR) {
		sf::Image image = texture.copyToImage();
		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 30; j++) {
				sf::Color pixelColor = image.getPixel(i, j);
				if (pixelColor.b < num && pixelColor.g < num && pixelColor.r < num) image.setPixel(i, j, sf::Color::Transparent);
			}
		}
		texture.loadFromImage(image);
	}
	for (sf::Texture& texture : texturesToMoveUD) {
		sf::Image image = texture.copyToImage();
		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 30; j++) {
				sf::Color pixelColor = image.getPixel(i, j);
				if (pixelColor.b < num && pixelColor.g < num && pixelColor.r < num) image.setPixel(i, j, sf::Color::Transparent);
			}
		}
		texture.loadFromImage(image);
	}
	sf::Image image0 = texture0.copyToImage();
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			sf::Color pixelColor = image0.getPixel(i, j);
			if (pixelColor.b < num && pixelColor.g < num && pixelColor.r < num) image0.setPixel(i, j, sf::Color::Transparent);
		}
	}
	texture0.loadFromImage(image0);
	sf::Image image13 = texture13.copyToImage();
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			sf::Color pixelColor = image13.getPixel(i, j);
			if (pixelColor.b < num && pixelColor.g < num && pixelColor.r < num) image13.setPixel(i, j, sf::Color::Transparent);
		}
	}
	texture13.loadFromImage(image13);
}

void Game::setWindow()
{
	int placeForExpr = 20;
	unsigned int w = 32 * 30, h = 23 * 30;
	sf::VideoMode bestMode;
	for (sf::VideoMode mode : sf::VideoMode::getFullscreenModes())
	{
		if (mode.height >= sf::VideoMode::getDesktopMode().height - 2 * placeForExpr) continue;
		bestMode = mode;
		break;
	}
	double k = static_cast<double>(bestMode.height) / static_cast<double>(h);
	double width = k * static_cast<double>(w);

													/*			 //  for creating and test levels
	winpos = sf::Vector2i(1215, 330);                   /////////////////////////////////////////////////////////////
	winsize = sf::Vector2u(500 * 32 / 23, 500);        /////////////////////////////////////////////////////////////

													*/

	window.create(sf::VideoMode(32 * 30, 23 * 30 + 20), "Lode Runner");
	window.setPosition(sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - width) / 2), 0));
	window.setSize(sf::Vector2u(static_cast<unsigned int>(width), bestMode.height + placeForExpr));
	window.setFramerateLimit(framesLimit);
}

void Game::setIcon()
{
	sf::Image windowIcon = texture52.copyToImage();
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
				if (movingLeft && sprite1.getPosition().x <= 15 + help || movingRight && sprite1.getPosition().x >= 32 * 30 - 15 - help) return;
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
				if (sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - (30 + help) && sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top - (30 - help))
				{
					sprite1.setPosition(sprite1.getGlobalBounds().left + 15, sprite.getGlobalBounds().top - 15);
				}
				return;
			}
			if (movingLeft && sprite1.getPosition().x <= 15 + help || movingRight && sprite1.getPosition().x >= 32 * 30 - 15 - help) return;
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
				if (sprite1.getGlobalBounds().top > sprite.getGlobalBounds().top - help && sprite1.getGlobalBounds().top < sprite.getGlobalBounds().top + help)
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
			bool check = false;
			if ((checkLeft() || checkRight()) && counts == 900) check = true;
			if (!onUD && !isLadderTexture && !check) 
			{
				int counts = 0;
				for (sf::Sprite& sprite : spritesWorkout)
				{
					if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds()) && sprite1.getGlobalBounds().top == sprite.getGlobalBounds().top)
					{
						sf::Image img1 = sprite1.getTexture()->copyToImage();
						sf::Image img2 = texture13.copyToImage();
						int counter = 0;
						for (int i = 0; i < 30; i++)
						{
							for (int j = 0; j < 30; j++)
							{
								if (img1.getPixel(i, j) == img2.getPixel(i, j)) counter++;
							}
						}
						if (counter == 900) counts++;
						break;
					}
				}
				if (counts == 0) sprite1.setTexture(texture0);
			}
		}
		moveLR = false;
		return;
	}


	bool standLR = false;
	for (sf::Texture& texture : texturesToMoveUD)
	{
		sf::Image img1 = sprite1.getTexture()->copyToImage();
		sf::Image img2 = texture.copyToImage();
		int counter = 0;
		bool loop = true;
		for (int i = 0; i < 30; i++)
		{
			if (!loop) break;
			for (int j = 0; j < 30; j++)
			{
				if (img1.getPixel(i, j) == img2.getPixel(i, j)) counter++;
				else
				{
					loop = false;
					break;
				}
			}
		}
		if (counter == 900)
		{
			standLR = true;
			break;
		}
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
		if (sprite1.getPosition().x >= 32 * 30 - 15 - help || checkRight())
		{
			if (!onUD && !onHorizontalLadder && !isLadderTexture) sprite1.setTexture(texture0);
			if (!standLR) sprite1.setScale(1, 1);
			sprite1.setPosition(float(left) + 15, sprite1.getGlobalBounds().top + 15);
			return;
		}
		moveLR = true;
		mainSpeed = fabs(mainSpeed);
		sprite1.setScale(1, 1);
	}

	if (movingLeft)
	{
		if (sprite1.getPosition().x <= 15 + help || checkLeft())
		{
			if (!onUD && !onHorizontalLadder && !isLadderTexture) sprite1.setTexture(texture0);
			if (!standLR) sprite1.setScale(-1, 1);
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
		sf::FloatRect intersection;
		if (sprite1.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) 
			&& sprite1.getGlobalBounds().top >= sprite.getGlobalBounds().top - help
			&& sprite1.getGlobalBounds().top <= sprite.getGlobalBounds().top + help)
		{
			int counter = 0;
			for (sf::Sprite& spr : spritesWorkout)
			{
				if (sprite1.getGlobalBounds().intersects(spr.getGlobalBounds()) && sprite.getGlobalBounds().getPosition() != spr.getGlobalBounds().getPosition())
				{
					counter++;
					break;
				}
			}
			if (!(counter == 0 && intersection.width < help))
			{
				isWorkout = true;
				animateWorkout();
				sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
				return;
			}
		}
	}

	animateLR();
	sprite1.move(deltaTime.asSeconds()* mainSpeed, 0);
}

bool Game::checkRight()
{
	for (sf::Sprite& block : blocks)
	{
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 29.999f))
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
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 29.999f))
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
	texture0.loadFromMemory(__0_png, __0_png_len);
	texture1.loadFromMemory(__1_png, __1_png_len);
	texture2.loadFromMemory(__2_png, __2_png_len);
	texture3.loadFromMemory(__3_png, __3_png_len);
	texture4.loadFromMemory(__4_png, __4_png_len);
	texture5.loadFromMemory(__5_png, __5_png_len);
	texture10.loadFromMemory(__10_png, __10_png_len);
	texture11.loadFromMemory(__11_png, __11_png_len);
	texture12.loadFromMemory(__12_png, __12_png_len);
	texture13.loadFromMemory(__13_png, __13_png_len);
	texture14.loadFromMemory(__14_png, __14_png_len);
	texture15.loadFromMemory(__15_png, __15_png_len);
	texture16.loadFromMemory(__16_png, __16_png_len);
	texture17.loadFromMemory(__17_png, __17_png_len);
	texture18.loadFromMemory(__18_png, __18_png_len);
	texture19.loadFromMemory(__19_png, __19_png_len);
	texture20.loadFromMemory(__20_png, __20_png_len);
	texture21.loadFromMemory(__21_png, __21_png_len);
	texture22.loadFromMemory(__22_png, __22_png_len);
	texture23.loadFromMemory(__23_png, __23_png_len);
	texture24.loadFromMemory(__24_png, __24_png_len);
	texture25.loadFromMemory(__25_png, __25_png_len);
	texture26.loadFromMemory(__26_png, __26_png_len);
	texture27.loadFromMemory(__27_png, __27_png_len);
	texture28.loadFromMemory(__28_png, __28_png_len);
	texture29.loadFromMemory(__29_png, __29_png_len);
	texture30.loadFromMemory(__30_png, __30_png_len);
	texture31.loadFromMemory(__31_png, __31_png_len);
	texture32.loadFromMemory(__32_png, __32_png_len);
	texture33.loadFromMemory(__33_png, __33_png_len);
	texture34.loadFromMemory(__34_png, __34_png_len);
	texture35.loadFromMemory(__35_png, __35_png_len);
	texture36.loadFromMemory(__36_png, __36_png_len);
	texture37.loadFromMemory(__37_png, __37_png_len);
	texture38.loadFromMemory(__38_png, __38_png_len);
	texture39.loadFromMemory(__39_png, __39_png_len);
	texture40.loadFromMemory(__40_png, __40_png_len);
	texture41.loadFromMemory(__41_png, __41_png_len);
	texture42.loadFromMemory(__42_png, __42_png_len);
	texture43.loadFromMemory(__43_png, __43_png_len);
	texture44.loadFromMemory(__44_png, __44_png_len);
	texture45.loadFromMemory(__45_png, __45_png_len);
	texture46.loadFromMemory(__46_png, __46_png_len);
	texture47.loadFromMemory(__47_png, __47_png_len);
	texture48.loadFromMemory(__48_png, __48_png_len);
	texture49.loadFromMemory(__49_png, __49_png_len);
	texture50.loadFromMemory(__50_png, __50_png_len);
	texture51.loadFromMemory(__51_png, __51_png_len);
	texture52.loadFromMemory(__52_png, __52_png_len);
}

void Game::setEnemies()
{
	for (sf::Sprite& enemy : enemies)
	{
		enemy.setTexture(texture13);
		enemy.setColor(sf::Color::Magenta);
	}
}

int Game::getLevel()
{
	getNumOfLevels();
	std::ifstream ifile("levels/.save.txt");
	char ch;
	std::string num;
	while (ifile.get(ch))
	{
		if (!std::isdigit(ch))
		{
			wchar_t wc = std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(ch);
			std::wstring wstr;
			wstr += wc;
			showError(L"Error file loading: levels/.save.txt\nUnexpected symbol - '" + wstr + L"'\nCheck levels/.rules.txt");
		}
		num += ch;
	}
	ifile.close();
	return std::stoi(num);
}

void Game::setLevel(int level)
{
	getNumOfLevels();
	std::ofstream ofile("levels/.save.txt", std::ios::trunc | std::ios::out);
	ofile << level;
	ofile.close();
}

int Game::getNumOfLevels()
{
	if (!fs::directory_entry("levels").exists()) 
		showErrorWithLink(L"There must be folder 'levels'\nin the same directory as an executable file\nCheck rules on:\n", 3.0f);
	std::ifstream rules("levels/.rules.txt");
	if (!rules.is_open()) showErrorWithLink(L"Error file loading: levels/.rules.txt\nCheck rules on:\n", 2.0f);
	rules.close();
	std::ifstream isfile("levels/.save.txt");
	if (!isfile.is_open()) showError(L"Error file loading: levels/.save.txt\nCheck levels/.rules.txt");
	isfile.close();

    int files = 0;
	for (const auto& entry : fs::directory_iterator("levels"))
	{
		std::string directoryName = entry.path().filename().string();
		std::wstring wpath = entry.path().filename().wstring();
		if (directoryName == ".save.txt" || directoryName == ".rules.txt") continue;
		if (directoryName.size() < 10 || directoryName.size() > 12 || entry.is_directory()) 
			showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
		std::string start = directoryName.substr(0, 5);
		std::string end = directoryName.substr(directoryName.size() - 4, 4);
		int point = 1;
		for (size_t i = 6; i < directoryName.size() - 4; i++)
		{
			if (directoryName.at(i) != '.') point++;
			else break;
		}
		std::string center = directoryName.substr(5, point);
		if (center.at(0) == '0') showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
		bool isNum = true;
		for (size_t i = 0; i < center.size(); i++)
		{
			if (!isdigit(center.at(i))) showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
		}
		if (start != "level" || end != ".txt") showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
		files++;
	}
	int levels = 0;
	for (int i = 1; i <= files; i++)
	{
		std::wstring path = L"levels/level" + std::to_wstring(i) + L".txt";
		std::ifstream ifile(path);
		if (!ifile.is_open()) showError(L"Error file loading: " + path + L"\nCheck levels/.rules.txt");
		ifile.close();
		levels++;
	}
	return levels;
}

void Game::updateLevel(bool isNextLevel)
{
	if (isNextLevel)
	{
		if (level == getNumOfLevels())
			level = 1;
		else
			level++;
	}
	setSprites(level);
	setSprite1();
	setEnemies();
	setText(level);
	setLevel(level);
	queueDeleted.clear();
	queueTimer.clear();
	isAnimatedDeletes.clear();
	isPushedLS.clear();
	miniAnimateDelete.clear();
	animatedSprites.clear();
	counterDeletedTextures.clear();
	killedSprites.clear();
	isVictory = false;
	isFromFly = false;
	space = false;
	isVictory = false;
	isWorkout = false;
	ignoreUD = false;
	moveLR = false;
	movingRight = false;
	movingLeft = false;
	movingUp = false;
	movingDown = false;
}

void Game::drawTransition()
{
	sf::Image img = texture20.copyToImage();
	if (opacity <= 255 - transitionSpeed && isDrawnFade == false)
	{
		opacity += transitionSpeed;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				img.setPixel(i, j, sf::Color(0, 0, 0, opacity));
			}
		}
		textureFade.loadFromImage(img);
		screenFade.setTexture(textureFade);
		window.draw(screenFade);
		if (opacity > 255 - transitionSpeed)
		{
			if (isRestart) updateLevel(false);
			if (isWin) updateLevel(true);
			isDrawnFade = true;
			opacity = 255;
		}
	}
	if (opacity >= transitionSpeed && isDrawnFade)
	{
		opacity -= transitionSpeed;
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				img.setPixel(i, j, sf::Color(0, 0, 0, opacity));
			}
		}
		textureFade.loadFromImage(img);
		screenFade.setTexture(textureFade);
		window.draw(screenFade);
		if (opacity < transitionSpeed)
		{
			opacity = transitionSpeed;
			isDrawnFade = false;
			isRestart = false;
			isWin = false;
			deltaTimeClock.restart();
		}
	}
}

void Game::showError(std::wstring finalMessage)
{
	window.close();
	sf::RenderWindow errWindow(sf::VideoMode::getFullscreenModes().back(), "Error");
	errWindow.setPosition(sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - errWindow.getSize().x) / 2), 0));
	while (errWindow.isOpen())
	{
		sf::Event event;
		while (errWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				errWindow.close();
				window.close();
			}
		}
		sf::Text finalText(finalMessage, font, 24);
		finalText.setFillColor(sf::Color::Magenta);
		finalText.setPosition((errWindow.getSize().x - finalText.getGlobalBounds().width) / 2,
								(errWindow.getSize().y - finalText.getGlobalBounds().height) / 2);
		errWindow.clear(sf::Color::Black);
		errWindow.draw(finalText);
		errWindow.display();
	}
	std::exit(1);
}

void Game::showErrorWithLink(std::wstring finalMessage, float step)
{
	window.close();
	sf::RenderWindow errWindow(sf::VideoMode::getFullscreenModes().back(), "Error", sf::Style::Close | sf::Style::Titlebar);
	errWindow.setPosition(sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - errWindow.getSize().x) / 2), 0));
	std::string linkString = "https://github.com/dmtrDO/Lode-Runner";
	sf::Text linkage(linkString, font);
	sf::Text finalText;
	bool isClicked = false;
	sf::Vector2f mousePosition;
	bool onText = false;
	sf::Cursor cursor;
	finalText.setFont(font);

	while (errWindow.isOpen())
	{
		isClicked = false;
		sf::Event event;
		while (errWindow.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				errWindow.close();
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				isClicked = true;
			}
			if (event.type == sf::Event::MouseMoved)
			{
				mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(errWindow));
			}
		}
		finalText.setString(finalMessage);
		finalText.setCharacterSize(24);
		finalText.setFillColor(sf::Color::Magenta);
		finalText.setPosition((errWindow.getSize().x - linkage.getGlobalBounds().width) / 2, (errWindow.getSize().y - finalText.getGlobalBounds().height) / 2);
		linkage.setPosition((errWindow.getSize().x - linkage.getGlobalBounds().width) / 2 + 2,
							(errWindow.getSize().y - finalText.getGlobalBounds().height) / 2 + step * linkage.getGlobalBounds().height - 3.5);
		linkage.setFillColor(sf::Color::Magenta);
		linkage.setCharacterSize(24);
		linkage.setStyle(sf::Text::Style::Regular);
		if (linkage.getGlobalBounds().contains(mousePosition))
		{
			cursor.loadFromSystem(sf::Cursor::Hand);
			errWindow.setMouseCursor(cursor);
			linkage.setFillColor(sf::Color::Red);
			linkage.setStyle(sf::Text::Style::Underlined);
			if (isClicked) 
			{
				openLink("https://github.com/dmtrDO/Lode-Runner");
				errWindow.close();
			}
		}
		else
		{
			cursor.loadFromSystem(sf::Cursor::Arrow);
			errWindow.setMouseCursor(cursor);
		}
		errWindow.clear(sf::Color::Black);
		errWindow.draw(finalText);
		errWindow.draw(linkage);
		errWindow.display();
	}
	std::exit(1);
}

void Game::openLink(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
	system(("start " + url).c_str());
#elif defined(__APPLE__) || defined(__MACH__)
	system(("open " + url).c_str());
#elif defined(__linux__)
	system(("xdg-open " + url).c_str());
#endif
}


