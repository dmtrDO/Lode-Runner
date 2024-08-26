
#include "Enemy.h"

std::vector <sf::Sprite> Enemy::forFly;
std::vector <sf::Sprite> Enemy::spritesWorkout;
std::vector <sf::Sprite> Enemy::spritesUD;
std::vector <sf::Sprite> Enemy::blocks;
std::vector <sf::Sprite> Enemy::holes;

Enemy::Enemy(sf::FloatRect bounds) {
	loadTextures();
	initVariables();
	sprite1.setColor(sf::Color::Magenta);
	sprite1.setOrigin(15.0f, 15.0f);
	sprite1.setPosition(bounds.left + 15.0f, bounds.top + 15.0f);
	generator.seed(static_cast<unsigned int>(std::time(nullptr)));
}

bool Enemy::updateCaught(sf::Time deltaTime, sf::Sprite& goal) {

	sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
	float spriteLeft = spriteBounds.left;
	float spriteTop = spriteBounds.top;

	if (hole.getGlobalBounds().left == -100.0f) {
		sf::FloatRect intersection;
		for (sf::Sprite& sprite : holes) {
			if (spriteBounds.intersects(sprite.getGlobalBounds(), intersection) && intersection.width * intersection.height >= 450) {
				hole = sprite;
				break;
			}
		}
		if (hole.getGlobalBounds().left == -100.0f) {
			isCaught = false;
			return false;
		}
	}

	sf::RectangleShape rect(sf::Vector2f(30.0f, help));
	rect.setPosition(spriteLeft, spriteTop + 30.0f);
	sf::FloatRect intersection;

	if (spriteTop + 30.0f >= hole.getGlobalBounds().top + 30.0f - help &&
		spriteTop + 30.0f <= hole.getGlobalBounds().top + 30.0f &&
		spriteBounds.getPosition() != hole.getGlobalBounds().getPosition() && isCaught == false && isFromFly) {
		isWorkout = false;
		isCaught = true;
		sprite1.setPosition(hole.getGlobalBounds().left + 15.0f, hole.getGlobalBounds().top + 15.0f);
		caughtTimer.restart();
		return true;
	} else if (spriteTop == hole.getGlobalBounds().top && caughtTimer.getElapsedTime().asSeconds() <= 3.0f && isFromFly) {
		return true;
	} else if (caughtTimer.getElapsedTime().asSeconds() > 3.0f && isCaught == true && isFromFly &&
		!(spriteTop + 30.0f >= hole.getGlobalBounds().top && spriteTop + 30.0f <= hole.getGlobalBounds().top + help)) {
		int counter = 0;
		for (sf::Sprite& sprite : blocks) {
			if (sprite.getGlobalBounds().contains(spriteLeft + 1.0f, spriteTop - help) ||
				sprite.getGlobalBounds().contains(spriteLeft + 29.0f, spriteTop - help)) {
				counter++;
				hole.setPosition(-100.0f, -100.0f);
				break;
			}
		}
		if (counter == 0) {
			animateUD();
			isFlyingTexture = false;
			sprite1.move(0, -fabs(mainSpeed) * deltaTime.asSeconds());
		} 
		return true;
	} else if (spriteTop + 30.0f >= hole.getGlobalBounds().top && spriteTop + 30.0f <= hole.getGlobalBounds().top + help &&
				caughtTimer.getElapsedTime().asSeconds() > 3.0f && spriteTop + 30.0f != hole.getGlobalBounds().top && isFromFly) {
		sprite1.setPosition(spriteLeft + 15.0f, hole.getGlobalBounds().top + 15.0f - 30.0f);
		isCaught = false;
		isClimbed = true;
		return true;
	} else if (isClimbed && rect.getGlobalBounds().intersects(hole.getGlobalBounds(), intersection) && intersection.width > help) {

		if (checkLeft() && checkRight() ||
			checkLeft() && spriteLeft == windowWidth * 30.0f - 30.0f ||
			checkRight() && spriteLeft == 0.0f) {
			isClimbed = false;
			return true;
		}

		int counter = 0;
		for (sf::Sprite& sprite : spritesWorkout) {
			if (spriteBounds.intersects(sprite.getGlobalBounds()) && spriteTop == sprite.getGlobalBounds().top) {
				counter++;
				isWorkout = true;
				break;
			}
		}
		
		if (counter != 0)
			animateWorkout();
		else
			animateLR();

		if (changedDirectionCounter == 0) {
			changedDirectionCounter++;
			if (spriteLeft > goal.getGlobalBounds().left) {
				caughtDirection = -1;
				if (checkLeft()) caughtDirection = 1;
			} else {
				caughtDirection = 1;
				if (checkRight() || spriteLeft + 30.0f == windowWidth * 30.0f) caughtDirection = -1;
			}
		}

		sprite1.setScale(caughtDirection, 1);
		sprite1.move(fabs(mainSpeed) * deltaTime.asSeconds() * caughtDirection, 0);

		if ((checkEnemyLeft() && caughtDirection == -1 || checkEnemyRight() && caughtDirection == 1) && intersection.width < 30.0f - help) {
			hole.setPosition(-100.0f, -100.0f);
			isClimbed = false;
			isCaught = false;
			caughtDirection = 1;
			changedDirectionCounter = 0;
			if (isFromFly) {
				isFromFly = false;
				if (isWorkout == false) sprite1.setTexture(texture0);
			}
		}

		return true;

	} else if (isClimbed && rect.getGlobalBounds().intersects(hole.getGlobalBounds(), intersection) && intersection.width <= help && intersection.width > 0) {
		int left = static_cast<int>(rect.getGlobalBounds().left);
		int mod = left % 30;
		if (mod > 15) {
			mod = 30 - mod;
			left += mod;
		} else if (mod < 15) left -= mod;

		sprite1.setPosition((float)left + 15, spriteTop + 15);
		isClimbed = false;
		return true;
	} else {
		hole.setPosition(-100.0f, -100.0f);
		isClimbed = false;
		isCaught = false;
		caughtDirection = 1;
		changedDirectionCounter = 0;
		if (isFromFly) {
			isFromFly = false;
			if (isWorkout == false) sprite1.setTexture(texture0);
		}
	}

	return false;
}

void Enemy::animateLR() {
	sf::Time elapsedTime = clockAnimationLR.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalLR) {
		frameIndexLR = (frameIndexLR + 1) % static_cast<int>(texturesToMoveLR.size());
		sprite1.setTexture(texturesToMoveLR[frameIndexLR]);
		clockAnimationLR.restart();
	}
}

void Enemy::animateUD() {
	sf::Time elapsedTime = clockAnimationUD.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalUD) {
		frameIndexUD = (frameIndexUD + 1) % static_cast<int>(texturesToMoveUD.size());
		sprite1.setTexture(texturesToMoveUD[frameIndexUD]);
		clockAnimationUD.restart();
	}
}

void Enemy::animateWorkout() {
	sf::Time elapsedTime = clockAnimationWorkout.getElapsedTime();
	if (elapsedTime.asMilliseconds() > animationMoveIntervalWorkout) {
		frameIndexWorkout = (frameIndexWorkout + 1) % static_cast<int>(texturesForWorkout.size());
		sprite1.setTexture(texturesForWorkout[frameIndexWorkout]);
		clockAnimationWorkout.restart();
	}
}

void Enemy::updateMoveLR(sf::Time deltaTime) {
	sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
	float spriteLeft = spriteBounds.left;
	float spriteTop = spriteBounds.top;

	sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
	helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
	helpArea.setPosition(spriteLeft + 15, spriteTop + 15);

	for (const sf::Sprite& sprite : spritesUD) {

		if (!sprite.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;

		sf::FloatRect spriteRect = sprite.getGlobalBounds();
		if (spriteRect.intersects(spriteBounds) && spriteRect.left == spriteLeft) {
			if ((movingLeft || movingRight) && spriteTop + 15 >= spriteRect.top - 15 && spriteTop + 15 <= spriteRect.top) {
				if (movingLeft && sprite1.getPosition().x <= 15 + help || movingRight && sprite1.getPosition().x >= windowWidth * 30 - 15 - help) return;
				for (const sf::Sprite& block : blocks) {
					if ((block.getGlobalBounds().contains(spriteLeft - 0.01f, spriteTop + 15.0f) && movingLeft) ||
						(block.getGlobalBounds().contains(spriteLeft + 30.01f, spriteTop + 15.0f) && movingRight))
						return;
				}
				for (const sf::Sprite& spr : spritesUD) {
					if (spriteRect.contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return;
				}
				mainSpeed = -fabs(mainSpeed);
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				onUD = false;
				isFromFly = false;
				isFlyingTexture = false;
				if (spriteTop >= spriteRect.top - (30 + help) && spriteTop <= spriteRect.top - (30 - help)) {
					sprite1.setPosition(spriteLeft + 15, spriteRect.top - 15);
				}
				return;
			}
			if (movingLeft && sprite1.getPosition().x <= 15 + help || movingRight && sprite1.getPosition().x >= windowWidth * 30 - 15 - help) return;
			if (spriteTop != spriteRect.top && spriteRect.contains(spriteLeft + 15, spriteTop + 15) && (movingRight || movingLeft)) {
				for (const sf::Sprite& block : blocks) {
					if ((block.getGlobalBounds().contains(spriteLeft - 0.01f, spriteTop + 15.0f) && movingLeft) ||
						(block.getGlobalBounds().contains(spriteLeft + 30.01f, spriteTop + 15.0f) && movingRight))
						return;
				}
				mainSpeed = spriteTop + 15 > spriteRect.top + 15 ? -fabs(mainSpeed) : fabs(mainSpeed);
				animateUD();
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				onUD = false;
				isFromFly = false;
				isFlyingTexture = false;

				if (spriteTop > spriteRect.top - help && spriteTop < spriteRect.top + help) {
					sprite1.setPosition(spriteLeft + 15, spriteRect.top + 15);
				}
				return;
			}
		}
	}

	if (((!movingLeft && !movingRight) || (movingLeft && movingRight))) {
		if (moveLR && !isWorkout && !onUD && (int)spriteTop % 30 == 0) sprite1.setTexture(texture0);
		moveLR = false;
		return;
	}

	int left = static_cast<int>(spriteLeft);
	int mod = left % 30;
	if (mod > 15) {
		mod = 30 - mod;
		left += mod;
	} else if (mod < 15) left -= mod;

	if (movingRight) {
		if (sprite1.getPosition().x >= windowWidth * 30 - 15 - help || checkRight()) {

			int workCounter = 0;
			for (sf::Sprite& sprite : spritesWorkout) {
				if (!spriteBounds.intersects(sprite.getGlobalBounds())) workCounter++;
			}
			if (workCounter == spritesWorkout.size()) isWorkout = false;

			if ((checkRight() || spriteLeft >= windowHeight * windowWidth - 30) && isWorkout == false && onUD == false) sprite1.setTexture(texture0);
			sprite1.setPosition(float(left) + 15, spriteTop + 15);
			return;
		}
		moveLR = true;
		mainSpeed = fabs(mainSpeed);
		sprite1.setScale(1, 1);
	}

	if (movingLeft) {
		if (sprite1.getPosition().x <= 15 + help || checkLeft()) {

			int workCounter = 0;
			for (sf::Sprite& sprite : spritesWorkout) {
				if (!spriteBounds.intersects(sprite.getGlobalBounds())) workCounter++;
			}
			if (workCounter == spritesWorkout.size()) isWorkout = false;

			if ((checkLeft() || spriteLeft <= help) && isWorkout == false && onUD == false) sprite1.setTexture(texture0);
			sprite1.setPosition(float(left) + 15, spriteTop + 15);
			return;
		}
		moveLR = true;
		mainSpeed = -fabs(mainSpeed);
		sprite1.setScale(-1, 1);
	}

	isWorkout = false;
	for (const sf::Sprite& sprite : spritesWorkout) {
		if (!sprite.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;
		sf::FloatRect sprBounds = sprite.getGlobalBounds();
		sf::FloatRect intersection;
		if (spriteBounds.intersects(sprBounds, intersection)
			&& spriteTop >= sprBounds.top - help
			&& spriteTop <= sprBounds.top + help) {
			int counter = 0;
			for (const sf::Sprite& spr : spritesWorkout) {
				if (spriteBounds.intersects(spr.getGlobalBounds()) && sprBounds.getPosition() != spr.getGlobalBounds().getPosition()) {
					counter++;
					break;
				}
			}
			if (!(counter == 0 && intersection.width < help)) {
				isWorkout = true;
				animateWorkout();
				sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
				onUD = false;
				isFromFly = false;
				isFlyingTexture = false;
				return;
			}
		}
	}
	animateLR();
	sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
	onUD = false;
	isFromFly = false;
	isFlyingTexture = false;
}

void Enemy::updateMoveUD(sf::Time deltaTime) {
	sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
	float spriteLeft = spriteBounds.left;
	float spriteTop = spriteBounds.top;

	sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
	helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
	helpArea.setPosition(spriteLeft + 15, spriteTop + 15);

	bool tmpLR = true;
	if (spriteLeft == 0 && movingLeft || spriteLeft == windowWidth * 30 - 30 && movingRight) tmpLR = false;
	for (sf::Sprite& block : blocks) {
		sf::FloatRect blockBounds = block.getGlobalBounds();
		if (blockBounds.contains(spriteLeft - 0.01f, spriteTop + 15.0f) && movingLeft) { tmpLR = false; break; }
		if (blockBounds.contains(spriteLeft + 30.01f, spriteTop + 15.0f) && movingRight) { tmpLR = false; break; }
	}
	if ((movingRight || movingLeft) && tmpLR) return;
	if ((!movingUp && !movingDown) || (movingUp && movingDown)) {
		bool tmp = true;
		for (sf::Sprite& sprite : spritesWorkout) {
			if (spriteBounds.intersects(sprite.getGlobalBounds())) {
				tmp = false;
				break;
			}
		}
		bool onUD = false;
		for (sf::Sprite& sp : spritesUD) {
			if (spriteBounds.intersects(sp.getGlobalBounds()) && spriteLeft == sp.getGlobalBounds().left) {
				onUD = true;
				break;
			}
		}
		if (tempIgnore && tmp && !onUD) {
			sprite1.setTexture(texture0);
			isFlyingTexture = false;
		}
		return;
	}
	bool temp = true;
	sf::Sprite spriteToCenter;

	for (sf::Sprite& sprite : spritesUD) {
		if (!sprite.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;

		sf::FloatRect sprBounds = sprite.getGlobalBounds();
		float sprLeft = sprBounds.left;
		float sprTop = sprBounds.top;

		if (spriteLeft == sprLeft && spriteTop + 30 >= sprTop && spriteTop + 30 <= sprTop + help && movingUp) {
			bool valid = true;
			for (sf::Sprite& sp : spritesUD) {
				sf::FloatRect intersection;
				if (spriteBounds.intersects(sp.getGlobalBounds(), intersection)) {
					if (intersection.width * intersection.height > 750) {
						valid = false;
						break;
					}
				}
			}
			if (valid && spriteTop != sprTop - 30) {
				sprite1.setPosition(spriteLeft + 15, sprTop - 15);
				onUD = true;
				return;
			}
		}

		if (spriteBounds.intersects(sprBounds) && spriteLeft >= sprLeft - 15 && spriteLeft <= sprLeft + 15) {
			for (sf::Sprite& block : blocks) {
				if (movingDown && block.getGlobalBounds().contains(spriteLeft + 15, spriteTop + 30 + help)) {
					sprite1.setPosition(spriteLeft + 15, block.getGlobalBounds().top - 15);
					onUD = true;
					return;
				}
				if (movingUp && block.getGlobalBounds().contains(spriteLeft + 15, spriteTop - help)) {
					sprite1.setPosition(spriteLeft + 15, block.getGlobalBounds().top + 45);
					onUD = true;
					return;
				}
			}
			temp = false;
			spriteToCenter = sprite;
		}

		if (spriteLeft >= sprLeft - 15 && spriteLeft <= sprLeft + 15 && spriteTop == sprTop - 30 && temp == true) {
			spriteToCenter = sprite;
			if (movingDown) {
				mainSpeed = fabs(mainSpeed);
				centrelizing(spriteToCenter);
				if (ignoreUD) {
					sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
					onUD = false;
					isFromFly = false;
					bool intersects = false;
					for (sf::Sprite& sprite : spritesWorkout) {
						if (spriteBounds.intersects(sprite.getGlobalBounds())) {
							intersects = true;
							break;
						}
					}
					intersects ? animateWorkout() : animateLR();
					tempIgnore = true;
					return;
				}
				sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
				isFromFly = false;
				onUD = false;

				animateUD();
			}
			return;
		}
	}
	if (temp) return;
	if (movingUp) mainSpeed = -fabs(mainSpeed);
	if (movingDown) mainSpeed = fabs(mainSpeed);
	centrelizing(spriteToCenter);
	if (ignoreUD) {
		tempIgnore = true;
		sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
		isFromFly = false;
		isFlyingTexture = false;
		onUD = false;
		bool intersects = false;
		for (sf::Sprite& sprite : spritesWorkout) {
			if (spriteBounds.intersects(sprite.getGlobalBounds())) {
				intersects = true;
				break;
			}
		}
		intersects ? animateWorkout() : animateLR();
		return;
	}
	tempIgnore = false;
	sprite1.move(0, deltaTime.asSeconds() * mainSpeed);
	onUD = false;
	isFromFly = false;
	isFlyingTexture = false;
	animateUD();
}

bool Enemy::updateFly() {
	sf::RectangleShape rect(sf::Vector2f(30.0f, help));
	rect.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);
	sf::FloatRect rectBounds = rect.getGlobalBounds();
	float rectLeft = rectBounds.left;
	float rectTop = rectBounds.top;

	sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
	float spriteLeft = spriteBounds.left;
	float spriteTop = spriteBounds.top;

	sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
	helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
	helpArea.setPosition(spriteLeft + 15, spriteTop + 15);
	sf::FloatRect helpBounds = helpArea.getGlobalBounds();

	bool tempUD = false;
	for (sf::Sprite& sp : spritesUD) {

		sf::FloatRect spBounds = sp.getGlobalBounds();

		if (!spBounds.intersects(helpBounds)) continue;

		sf::FloatRect area;
		if (spriteBounds.intersects(spBounds, area) && area.width * area.height >= 450) {
			tempUD = true;
			break;
		}
		if (spriteBounds.intersects(spBounds, area) && area.width < help) {
			int count = 0;
			for (sf::Sprite& s : spritesUD) {
				if (spriteBounds.intersects(s.getGlobalBounds()) && spBounds.getPosition() != s.getGlobalBounds().getPosition()) {
					count++;
					break;
				}
			}
			for (sf::Sprite& spri : spritesWorkout) {
				sf::FloatRect intersec;
				if (spriteBounds.intersects(spri.getGlobalBounds(), intersec) && intersec.width * intersec.height >= 450) {
					count++;
					break;
				}
			}
			if (count != 0) {
				tempUD = true;
				break;
			}
		}
	}

	bool tempWork = false;
	for (sf::Sprite& sp : spritesWorkout) {

		sf::FloatRect spBounds = sp.getGlobalBounds();

		if (!spBounds.intersects(helpBounds)) continue;

		sf::FloatRect area;
		if (spriteBounds.intersects(spBounds, area) && area.width < help) {
			int count = 0;
			for (sf::Sprite& s : spritesWorkout) {
				if (spriteBounds.intersects(s.getGlobalBounds()) && spBounds.getPosition() != s.getGlobalBounds().getPosition()) {
					count++;
					break;
				}
			}
			if (count != 0) {
				tempWork = true;
				break;
			}
		}
	}

	sf::FloatRect intersection;
	for (sf::Sprite& sprite : forFly) {

		sf::FloatRect sprBounds = sprite.getGlobalBounds();

		if (!sprBounds.intersects(helpBounds)) continue;

		if (tempWork || tempUD) break;

		if (rectBounds.intersects(sprBounds, intersection) && intersection.width < help) {
			bool work = false;
			for (sf::Sprite& sprt : spritesWorkout) {
				sf::FloatRect area;
				if (spriteBounds.intersects(sprt.getGlobalBounds(), area) && area.width > 30 - help && sprt.getGlobalBounds().top == spriteTop) {
					work = true;
					break;
				}
			}
			if (work) return false;
			int counts = 0;
			for (sf::Sprite& spr : forFly) {
				if (rectBounds.intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprBounds.getPosition()) {
					counts++;
					break;
				}
			}
			if (counts == 0 && rectTop == sprBounds.top) {
				int left = static_cast<int>(rectLeft);
				int mod = left % 30;
				if (mod > 15) {
					mod = 30 - mod;
					left += mod;
				} else if (mod < 15) left -= mod;

				sprite1.setPosition((float)left + 15, spriteTop + 15);
				return true;
			}
		}
	}

	for (sf::Sprite& sprite : spritesWorkout) {

		sf::FloatRect sprBounds = sprite.getGlobalBounds();

		if (!sprBounds.intersects(helpBounds)) continue;

		sf::FloatRect intersection;
		if (spriteBounds.intersects(sprBounds, intersection)
			&& spriteTop <= sprBounds.top
			&& spriteTop >= sprBounds.top - help) {
			if (spriteTop != sprBounds.top)
				sprite1.setPosition(spriteLeft + 15, sprBounds.top + 15);
			if (movingDown) {
				if (movingUp) return false;
				for (sf::Sprite spr : forFly) {
					sf::FloatRect area;
					if (rectBounds.intersects(spr.getGlobalBounds(), area) && area.width < 2 * help) {
						int counter = 0;
						for (sf::Sprite& spri : forFly) {
							if (rectBounds.intersects(spri.getGlobalBounds()) && spri.getGlobalBounds().getPosition() != spr.getGlobalBounds().getPosition()) {
								counter++;
								break;
							}
						}
						if (counter != 0) break;
						int left = static_cast<int>(rectLeft);
						int mod = left % 30;
						if (mod > 15) {
							mod = 30 - mod;
							left += mod;
						} else if (mod < 15) left -= mod;
						sprite1.setPosition((float)left + 15, spriteTop + 15 + help + 0.01f);
						return true;
					}
				}

				int floorCounter = 0;
				sf::FloatRect floorArea;
				for (sf::Sprite& spr : forFly) {
					if (!rectBounds.intersects(spr.getGlobalBounds())
						|| (rectBounds.intersects(spr.getGlobalBounds(), floorArea) && floorArea.width < 2 * help)) floorCounter++;
				}
				if (floorCounter == forFly.size()) {
					int ladCounter = 0;
					for (sf::Sprite& spr : spritesUD) {
						sf::FloatRect intersectionArea;
						if (spriteBounds.intersects(spr.getGlobalBounds(), intersectionArea) && intersectionArea.width < 2 * help
							&& spriteTop == spr.getGlobalBounds().top) ladCounter++;
					}
					if (ladCounter == 1) {
						int left = static_cast<int>(rectLeft);
						int mod = left % 30;
						if (mod > 15) {
							mod = 30 - mod;
							left += mod;
						} else if (mod < 15) left -= mod;
						sprite1.setPosition((float)left + 15, spriteTop + 15 + help + 0.01f);
					}
				}

				for (sf::Sprite& spr : blocks) {
					if (rectBounds.intersects(spr.getGlobalBounds())) return false;
				}
				for (sf::Sprite& spr : spritesUD) {
					if (spriteBounds.intersects(spr.getGlobalBounds()) && spriteBounds.intersects(sprBounds) &&
						sprBounds.top == spr.getGlobalBounds().top && spriteTop == sprBounds.top) return false;
					if (rectBounds.intersects(spr.getGlobalBounds()) && (rectLeft != spr.getGlobalBounds().left)) return false;
				}
				sprite1.setPosition(spriteLeft + 15, spriteTop + 15.0f + help + 0.01f);
			}
			sf::FloatRect area;
			if (spriteBounds.intersects(sprBounds, area) && area.width < help && movingDown == false) {
				int counts = 0;
				for (sf::Sprite& spr : spritesWorkout) {
					if (spriteBounds.intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprBounds.getPosition()) {
						counts++;
						break;
					}
				}
				for (sf::Sprite& spr : forFly) {
					sf::FloatRect rct;
					if (rectBounds.intersects(spr.getGlobalBounds()) || spriteBounds.intersects(spr.getGlobalBounds())) {
						counts++;
						break;
					}
				}
				if (counts == 0) {
					float left = spriteLeft > sprBounds.left ? sprBounds.left + 30 : sprBounds.left - 30;
					sprite1.setPosition(left + 15, spriteTop + 15);
					return true;
				}
			}
			return false;
		}
	}

	for (sf::Sprite& sprite : spritesUD) {

		sf::FloatRect sprBounds = sprite.getGlobalBounds();
		float sprLeft = sprBounds.left;
		float sprTop = sprBounds.top;

		if (!sprBounds.intersects(helpBounds)) continue;

		sf::FloatRect intersection;
		if (spriteBounds.intersects(sprBounds, intersection)) {
			if (intersection.width * intersection.height <= 450
				&& spriteLeft == sprLeft
				&& spriteTop > sprTop) {
				int counter = 0;
				for (sf::Sprite& ladder : spritesUD) {
					if (ladder.getGlobalBounds().contains(spriteLeft + 15, spriteTop + 15 + help)
						&& ladder.getGlobalBounds().getPosition() != sprBounds.getPosition()) {
						counter++;
						break;
					}
				}
				if (counter == 0) {
					for (sf::Sprite& sp : forFly) {
						if (rectBounds.intersects(sp.getGlobalBounds())) {
							sprite1.setPosition(spriteLeft + 15, sp.getGlobalBounds().top - 15);
							return false;
						}
					}
					return true;
				}
			}
			if (intersection.width < help) {
				int counts = 0;
				for (sf::Sprite& spr : spritesUD) {
					if (spriteBounds.intersects(spr.getGlobalBounds()) && spr.getGlobalBounds().getPosition() != sprBounds.getPosition()) {
						counts++;
						break;
					}
				}
				for (sf::Sprite& spr : forFly) {
					if (rectBounds.intersects(spr.getGlobalBounds())) {
						counts++;
						break;
					}
				}
				if (counts == 0) {
					float left = spriteLeft > sprLeft ? sprLeft + 30 : sprLeft - 30;
					sprite1.setPosition(left + 15, spriteTop + 15);
					return true;
				}
			}
			return false;
		}
	}

	int tmpCounter = 0;
	for (sf::Sprite& sprite : forFly) {
		if (!rectBounds.intersects(sprite.getGlobalBounds())) tmpCounter++;
	}
	if (tmpCounter == forFly.size()) {

		int md = (int)spriteLeft % 30;
		if (md >= 15) md = 30 - md;
		if (md <= help) {
			int left = static_cast<int>(rectLeft);
			int mod = left % 30;
			if (mod > 15) {
				mod = 30 - mod;
				left += mod;
			} else if (mod < 15) left -= mod;
			sprite1.setPosition(left + 15.0f, spriteTop + 15.0f);
		}

		return true;
	}

	for (sf::Sprite& sprite : forFly) {
		if (rectBounds.intersects(sprite.getGlobalBounds())) {
			sprite1.setPosition(spriteLeft + 15, sprite.getGlobalBounds().top - 15);
			return false;
		}
	}

	return false;
}

void Enemy::initVariables() {
	tempIgnore = false;
	windowWidth = 36;
	windowHeight = 20;
	direction = 1;
	isFlyingTexture = false;
	pickedGoldTime = 0;
	onGold = false;
	isPickedGold = false;
	changedDirectionCounter = 0;
	caughtDirection = 1;
	hole.setPosition(-100.0f, -100.0f);
	isCaught = false;
	isClimbed = false;
	isFromFly = false;
	space = false;
	onUD = false;
	isWorkout = false;
	ignoreUD = false;
	moveLR = false;
	frameIndexLR = 0;
	frameIndexUD = 0;
	frameIndexWorkout = 0;
	help = 2.0f;
	movingDown = false;
	movingLeft = false;
	movingRight = false;
	movingUp = false;
	mainSpeed = 70.0f;
	animationMoveIntervalLR = 45.0f;
	animationMoveIntervalUD = 28.0f;
	animationMoveIntervalWorkout = 42.0f;
}

void Enemy::loadTextures() {
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

	texturesToMoveLR = { texture1, texture2, texture3, texture4, texture5, texture4, texture3, texture2, texture1, texture10, texture11, texture12 };
	texturesToMoveUD = { texture21, texture22, texture23, texture24, texture25, texture26, texture25, texture24, texture23, texture22, texture21, texture27 };
	texturesForWorkout = { texture28, texture29, texture30, texture31, texture32, texture31, texture30, texture29 };

	int num = 30;

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

	for (sf::Texture& texture : texturesToMoveLR) {
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : texturesToMoveUD) {
		texture.setSmooth(true);
	}
	for (sf::Texture& texture : texturesForWorkout) {
		texture.setSmooth(true);
	}
	texture0.setSmooth(true);
	texture13.setSmooth(true);
}

bool Enemy::checkRight() const {
	for (sf::Sprite& block : blocks) {
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 29.999f)) {
			return true;
		}
	}
	return false;
}

bool Enemy::checkLeft() const {
	for (sf::Sprite& block : blocks) {
		if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 0.001f)
			|| block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 29.999f)) {
			return true;
		}
	}
	return false;
}

bool Enemy::checkEnemyLeft() const {
	sf::RectangleShape rect(sf::Vector2f(help, 30.0f));
	rect.setPosition(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top);
	for (sf::Sprite& block : blocks) {
		if (rect.getGlobalBounds().intersects(block.getGlobalBounds())) {
			return true;
		}
	}
	return false;
}

bool Enemy::checkEnemyRight() const {
	sf::RectangleShape rect(sf::Vector2f(help, 30.0f));
	rect.setPosition(sprite1.getGlobalBounds().left + 30.0f, sprite1.getGlobalBounds().top);
	for (sf::Sprite& block : blocks) {
		if (rect.getGlobalBounds().intersects(block.getGlobalBounds())) {
			return true;
		}
	}
	return false;
}

void Enemy::centrelizing(const sf::Sprite sprite) {
	if (sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help || sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left + help) {
		mainSpeed = sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help ? fabs(mainSpeed) : -fabs(mainSpeed);
		sprite1.setScale((float)sprite1.getGlobalBounds().left <= (float)sprite.getGlobalBounds().left - 1.0f ? 1.0f : -1.0f, 1.0f);
		ignoreUD = true;
		return;
	}
	sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
	ignoreUD = false;
}

void Enemy::initMoves() {
	movingLeft = false;
	movingRight = false;
	movingUp = false;
	movingDown = false;
}


