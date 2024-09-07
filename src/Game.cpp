
#include "Game.h"

Game::Game() {
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

void Game::handle() {
    if (startTimeDelay.getElapsedTime().asSeconds() < 1.0f) return;
    sf::Event event;
    while (window.pollEvent(event)) {
        sf::Keyboard::Key key = event.key.code;
        if (key == sf::Keyboard::Key::Escape && event.type == sf::Event::KeyReleased && isStart == true) {
            handleEscape();
            return;
        }
        switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break; 
            case sf::Event::KeyPressed:
                if (key == sf::Keyboard::Key::F2) isRestart = true;
                if (key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D || key == sf::Keyboard::Key::Numpad6) movingRight = true;
                if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::A || key == sf::Keyboard::Key::Numpad4) movingLeft = true;
                if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W || key == sf::Keyboard::Key::Numpad8) movingUp = true;
                if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S || key == sf::Keyboard::Key::Numpad5) movingDown = true;
                if (key == sf::Keyboard::Key::Space || key == sf::Keyboard::Key::LShift) space = true;
                if (isPause == true) handleText(key);
                break;
            case sf::Event::KeyReleased:
                if (key == sf::Keyboard::Key::Right || key == sf::Keyboard::Key::D || key == sf::Keyboard::Key::Numpad6) movingRight = false;
                if (key == sf::Keyboard::Key::Left || key == sf::Keyboard::Key::A || key == sf::Keyboard::Key::Numpad4) movingLeft = false;
                if (key == sf::Keyboard::Key::Up || key == sf::Keyboard::Key::W || key == sf::Keyboard::Key::Numpad8) movingUp = false;
                if (key == sf::Keyboard::Key::Down || key == sf::Keyboard::Key::S || key == sf::Keyboard::Key::Numpad5) movingDown = false;
                if (key == sf::Keyboard::Key::Space || key == sf::Keyboard::Key::LShift) space = false;
                break;
        }
    }
    if (!isStart && (space || movingLeft || movingRight || movingUp || movingDown)) {
        isStart = true;
        fps = 100;
    }
}

void Game::update() {
    if (window.hasFocus() && isStart == true) updateFPS();
    if (!window.hasFocus()) updateUnfocus();

    updateDeath();

    if (sprite1.getGlobalBounds().top + 30 <= 0) { isWin = true; return; }
    if (isRestart || isWin || isPause) return;

    if (window.hasFocus()) animateDeleted();
    if (!isVictory) updateGold();

    sf::Time deltaTime = deltaTimeClock.restart();

    bool onEnemy = updateOnEnemy(deltaTime);
    if (window.hasFocus()) updateEnemies(deltaTime);
    if (onEnemy) return;

    if (updateFly() && window.hasFocus() && isStart == true) {
        sprite1.move(0, fabs(mainSpeed) * deltaTime.asSeconds());
        sprite1.setTexture(texture13);
        isFromFly = true;
        return;
    }

    if (updateHelp(deltaTime)) return;
    updateMoveLR(deltaTime);
    updateMoveUD(deltaTime);
    updateSpace();
    if (isFromFly) sprite1.setTexture(texture13);
}

void Game::draw() {

    window.clear(sf::Color::Black);

    for (int i = 0; i < levelSprites.size(); i++) {
        window.draw(levelSprites[i]);
    }

    if (isStart == false) {
        float time = flicker.getElapsedTime().asSeconds();
        if (time > 0.0f && time < flickTime) {
            sprite1.setTexture(texture20);
        } else if (time >= flickTime && time < 2.0f * flickTime) {
            sprite1.setTexture(texture0);
        } else flicker.restart();
    } else if (fps < 5 && flicker.getElapsedTime().asSeconds() < flickTime) sprite1.setTexture(texture0);

    window.draw(sprite1);

    for (Enemy& enemy : enemies) {
        window.draw(enemy.sprite1);
    }

    for (sf::Sprite& sprite : killedSprites) {
        window.draw(sprite);
    }

    if (isRed && greenBlueOpacity < 255) {
        sf::Color color;
        color.a = 255;
        color.g = greenBlueOpacity;
        color.b = greenBlueOpacity;
        color.r = 255;
        text.setFillColor(color);
        greenBlueOpacity++;
    } else {
        text.setFillColor(sf::Color::White);
        isRed = false;
        greenBlueOpacity = 0;
    }
    window.draw(text);

    if (isWin || isRestart) drawTransition();

    window.display();
}

void Game::updateUnfocus() {
    movingDown = false;
    movingUp = false;
    movingLeft = false;
    movingRight = false;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    deltaTimeClock.restart();
    fpsArr.clear();
    fps = 100;
}

void Game::handleEscape() {
    if (isPause == false) {
        isPause = true;
        for (Stopwatch& stopwatch : queueTimer) { stopwatch.stop(); }
    } else {
        isPause = false;
        for (Stopwatch& stopwatch : queueTimer) { stopwatch.start(); }
        deltaTimeClock.restart();
        text.setString(std::to_string(level));
    }
}

void Game::handleText(sf::Keyboard::Key key) {
    if (key == sf::Keyboard::BackSpace && text.getString().getSize() != 0) 
        text.setString(text.getString().substring(0, text.getString().getSize() - 1));
    
    if (text.getString().getSize() < 3) {
        if (key == sf::Keyboard::Num0) text.setString(text.getString() + std::to_string(0));
        if (key == sf::Keyboard::Num1) text.setString(text.getString() + std::to_string(1));
        if (key == sf::Keyboard::Num2) text.setString(text.getString() + std::to_string(2));
        if (key == sf::Keyboard::Num3) text.setString(text.getString() + std::to_string(3));
        if (key == sf::Keyboard::Num4) text.setString(text.getString() + std::to_string(4));
        if (key == sf::Keyboard::Num5) text.setString(text.getString() + std::to_string(5));
        if (key == sf::Keyboard::Num6) text.setString(text.getString() + std::to_string(6));
        if (key == sf::Keyboard::Num7) text.setString(text.getString() + std::to_string(7));
        if (key == sf::Keyboard::Num8) text.setString(text.getString() + std::to_string(8));
        if (key == sf::Keyboard::Num9) text.setString(text.getString() + std::to_string(9));
    }

    if (key == sf::Keyboard::Enter) {
        int enteredLevel = std::stoi(text.getString().toAnsiString());
        if (enteredLevel >= 1 && enteredLevel <= getNumOfLevels() && enteredLevel != level) {
            level = enteredLevel;
            isRestart = true;
        } else {
            text.setString(std::to_string(level));
            isRed = true;
        }
    }
    
}

void Game::updateFPS() {
    fps++;
    if (framesClock.getElapsedTime().asSeconds() >= 1.0f) {
        std::cout << fps << "\n";
        framesClock.restart();
        fpsArr.push_back(fps);
        if (fpsArr.size() == 10) {
            int average = 0;
            for (float value : fpsArr) {
                average += value;
            }
            average /= fpsArr.size();
            if (average < 80) showError(L"Too few frames per second for playing");
            fpsArr.clear();
        }
        fps = 0;
    }
}

bool Game::updateHelp(sf::Time& deltaTime) {
    if (movingLeft && movingRight || movingUp && movingDown || !movingLeft && !movingRight || !movingUp && !movingDown) return false;

    bool isHelp = false;

    float spriteLeft = sprite1.getGlobalBounds().left;
    float spriteTop = sprite1.getGlobalBounds().top;

    sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
    helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
    helpArea.setPosition(spriteLeft + 15, spriteTop + 15);

    sf::RectangleShape rectdown(sf::Vector2f(30.0f, help));
    rectdown.setPosition(spriteLeft, spriteTop + 30 - help);
    sf::FloatRect rectdownBounds = rectdown.getGlobalBounds();
    float rectdownLeft = rectdownBounds.left;
    float rectdownTop = rectdownBounds.top;

    sf::RectangleShape rectunder(sf::Vector2f(30.0f, help));
    rectunder.setPosition(spriteLeft, spriteTop + 30);
    sf::FloatRect rectunderBounds = rectunder.getGlobalBounds();
    float rectunderLeft = rectunderBounds.left;
    float rectunderTop = rectunderBounds.top;

    sf::RectangleShape rectop(sf::Vector2f(30.0f, help));
    rectop.setPosition(spriteLeft, spriteTop);
    sf::FloatRect rectopBounds = rectop.getGlobalBounds();
    float rectopLeft = rectopBounds.left;
    float rectopTop = rectopBounds.top;

    float hlp = help;
    if (fpsArr.empty() == false && fpsArr.back() > 90) hlp = help / 2;

    for (sf::Sprite& sprite : spritesUD) {

        sf::FloatRect sprBounds = sprite.getGlobalBounds();
        float sprLeft = sprBounds.left;
        float sprTop = sprBounds.top;

        if (!sprBounds.intersects(helpArea.getGlobalBounds())) continue;

        if (movingLeft && sprite1.getPosition().x <= 15 + help || movingRight && sprite1.getPosition().x >= windowWidth * 30 - 15 - help) return false;
        for (sf::Sprite& block : blocks) {
            if (block.getGlobalBounds().contains(spriteLeft - 0.01f, spriteTop + 15.0f) && movingLeft) return false;
            if (block.getGlobalBounds().contains(spriteLeft + 30.01f, spriteTop + 15.0f) && movingRight) return false;
        }

        if (rectdownBounds.intersects(sprBounds) && movingUp
            && rectdownLeft >= sprLeft - hlp
            && rectdownLeft <= sprLeft + hlp) {
            for (sf::Sprite& block : blocks) {
                if (block.getGlobalBounds().contains(sprLeft + 15.0f, sprTop - 15.0f)) return false;
            }
            if (rectdownLeft != sprLeft) {
                sprite1.setPosition(sprLeft + 15, spriteTop + 15);
            } else {
                animateUD();
                sprite1.move(0, deltaTime.asSeconds() * -fabs(mainSpeed));

                isFromFly = false;
                if (rectdownTop >= sprTop - hlp
                    && rectdownTop <= sprTop + hlp) {
                    sprite1.setPosition(sprLeft + 15 + (movingRight ? (hlp + 0.01f) : (-hlp - 0.01f)), sprTop - 15);
                    return false;
                }
            }
            isHelp = true;
            break;
        }

        if (rectunderBounds.intersects(sprBounds) && movingDown
            && rectunderLeft >= sprLeft - hlp
            && rectunderLeft <= sprLeft + hlp) {
            if (rectunderLeft != sprLeft) {
                sprite1.setPosition(sprLeft + 15, spriteTop + 15);
            } else {
                animateUD();
                sprite1.move(0, deltaTime.asSeconds() * fabs(mainSpeed));
                isFromFly = false;
                if (rectunderTop >= sprTop + 30 - hlp
                    && rectunderTop <= sprTop + 30) {
                    sprite1.setPosition(sprLeft + 15 + (movingRight ? (hlp + 0.01f) : (-hlp - 0.01f)), sprTop + 15);
                    return false;
                }
            }
            isHelp = true;
            break;
        }

        if (rectopBounds.intersects(sprBounds) && movingDown
            && rectopLeft >= sprLeft - hlp
            && rectopLeft <= sprLeft + hlp) {
            for (sf::Sprite& block : blocks) {
                if (block.getGlobalBounds().contains(sprLeft + 15.0f, sprTop + 45.0f)) return false;
            }
            if (rectopLeft != sprLeft) {
                sprite1.setPosition(sprLeft + 15, spriteTop + 15);
            } else {
                animateUD();
                sprite1.move(0, deltaTime.asSeconds() * fabs(mainSpeed));
                isFromFly = false;
                if (rectopTop >= sprTop + 30 - hlp && rectopTop < sprTop + 30) {
                    sprite1.setPosition(sprLeft + 15 + (movingRight ? (hlp + 0.01f) : (-hlp - 0.01f)), sprTop + 45);
                    return false;
                }
            }
            isHelp = true;
            break;
        }
    }
    return isHelp;
}

void Game::updateDeath() {
    sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
    for (sf::Sprite& sprite : spaceBlocks) {
        if (spriteBounds.intersects(sprite.getGlobalBounds())) {
            isRestart = true;
            return;
        }
    }
    for (Enemy& sprite : enemies) {
        sf::FloatRect intersection;
        if (spriteBounds.intersects(sprite.sprite1.getGlobalBounds(), intersection)) {
            float area = intersection.width * intersection.height;
            if (area >= 350) {
                isRestart = true;
                return;
            }
        }
    }
}

void Game::updateSpace() {
    sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
    helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
    helpArea.setPosition(sprite1.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);

    if (space) {
        int vector = static_cast<int>(sprite1.getScale().x);

        for (sf::Sprite& sprite : spaceBlocks) {
            if (!sprite.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;

            sf::RectangleShape left(sf::Vector2f(60.0f, 1.0f)), right(sf::Vector2f(60.0f, 1.0f));
            left.setPosition(sprite.getGlobalBounds().left + 15 - 55, sprite.getGlobalBounds().top - 1.0f);
            right.setPosition(sprite.getGlobalBounds().left + 10, sprite.getGlobalBounds().top - 1.0f);

            sf::FloatRect intersection;
            if ((sprite1.getGlobalBounds().intersects(left.getGlobalBounds(), intersection) && intersection.width == 30.0f && vector == 1)
                || (sprite1.getGlobalBounds().intersects(right.getGlobalBounds(), intersection) && intersection.width == 30.0f && vector == -1)) {
                if (checkSpace(sprite)) return;
                if (spaceTime.getElapsedTime().asMilliseconds() < 250)
                    return;
                else
                    spaceTime.restart();

                sf::FloatRect area;
                sf::RectangleShape under(sf::Vector2f(30.0f, 1.0f));
                under.setPosition(sprite1.getGlobalBounds().left, sprite1.getGlobalBounds().top + 30);

                if (under.getGlobalBounds().intersects(sprite.getGlobalBounds())) {
                    float left = vector == 1 ? sprite.getGlobalBounds().left - 30 : sprite.getGlobalBounds().left + 30;
                    sprite1.setPosition(left + 15, sprite1.getGlobalBounds().top + 15);
                    bool isWorking = false;
                    for (sf::Sprite& work : spritesWorkout) {
                        if (work.getGlobalBounds().intersects(sprite1.getGlobalBounds())) {
                            isWorking = true;
                            break;
                        }
                    }
                    if (isWorking == false) sprite1.setTexture(texture0);
                }
                holes.push_back(sprite);
                removeBlock(sprite);
                return;
            }
        }
    }
}

bool Game::checkSpace(sf::Sprite& sprite) {
    if (!(sprite1.getGlobalBounds().top + 30.0f >= sprite.getGlobalBounds().top - help &&
        sprite1.getGlobalBounds().top + 30.0f <= sprite.getGlobalBounds().top + help)) return true;
    for (sf::Sprite& spr : forFly) {
        if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return true;
    }
    for (sf::Sprite& spr : spritesWorkout) {
        if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return true;
    }
    for (sf::Sprite& spr : goldSprites) {
        if (sprite.getGlobalBounds().contains(spr.getGlobalBounds().left + 15, spr.getGlobalBounds().top + 45)) return true;
    }
    return false;
}

void Game::removeBlock(sf::Sprite& spaced) {
    float spacedX = spaced.getGlobalBounds().left;
    float spacedY = spaced.getGlobalBounds().top;
    for (sf::Sprite& sprite : forFly) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY - 2000);
    }
    for (sf::Sprite& sprite : blocks) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY - 2000);
    }
    for (sf::Sprite& sprite : levelSprites) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY - 2000);
    }
    for (sf::Sprite& sprite : spaceBlocks) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY - 2000);
    }
    queueDeleted.push_back(spaced);
    Stopwatch timer;
    queueTimer.push_back(timer);
    isAnimatedDeletes.push_back(false);
    isPushedLS.push_back(false);
    animatedSprites.push_back(sf::Sprite());
    miniAnimateDelete.push_back(sf::Clock());
    counterDeletedTextures.push_back(0);
}

void Game::animateDeleted() {
    size_t size = queueDeleted.size();
    for (size_t i = 0; i < size; i++) {
        if (!isAnimatedDeletes[i]) {
            if (miniAnimateDelete[i].getElapsedTime().asMilliseconds() >= miniAnimateInterval) {
                animatedSprites[i].setPosition(queueDeleted[i].getGlobalBounds().left, queueDeleted[i].getGlobalBounds().top + 2000);
                for (sf::Sprite& sprite : killedSprites) {
                    if (sprite.getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition()) {
                        sprite.setTexture(deletedTextures[counterDeletedTextures[i]++]);
                        break;
                    }
                }
                if (!isPushedLS[i]) {
                    killedSprites.push_back(animatedSprites[i]);
                    isPushedLS[i] = true;
                }
                miniAnimateDelete[i].restart();
            }
            if (counterDeletedTextures[i] == deletedTextures.size()) {
                for (int k = 0; k < killedSprites.size(); k++) {
                    if (killedSprites[k].getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition()) {
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
        if (queueTimer[i].getElapsedTime() >= deletedBlockInterval * 1000.0f - reawakenedTime) {
            if (miniAnimateDelete[i].getElapsedTime().asMilliseconds() >= reawakenedInterval) {
                for (sf::Sprite& sprite : killedSprites) {  
                    if (sprite.getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition()) {
                        sprite.setTexture(reawakenedTextures[counterDeletedTextures[i]++]);
                        break;
                    }
                }
                if (!isPushedLS[i]) {
                    killedSprites.push_back(animatedSprites[i]);
                    isPushedLS[i] = true;
                }
                miniAnimateDelete[i].restart();
            }
            if (counterDeletedTextures[i] == reawakenedTextures.size()) {
                for (int k = 0; k < killedSprites.size(); k++) {
                    if (killedSprites[k].getGlobalBounds().getPosition() == animatedSprites[i].getGlobalBounds().getPosition()) {
                        killedSprites.erase(killedSprites.begin() + k);
                        break;
                    }
                }
                backBlock(queueDeleted[i]);
                holes.erase(holes.begin());
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

void Game::backBlock(sf::Sprite& spaced) {
    float spacedX = spaced.getGlobalBounds().left;
    float spacedY = spaced.getGlobalBounds().top;
    for (sf::Sprite& sprite : forFly) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY + 2000);
    }
    for (sf::Sprite& sprite : blocks) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY + 2000);
    }
    for (sf::Sprite& sprite : levelSprites) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY + 2000);
    }
    for (sf::Sprite& sprite : spaceBlocks) {
        if (spaced.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition())
            sprite.setPosition(spacedX, spacedY + 2000);
    }
}

void Game::setSprites(int level) {
    levelSprites.clear();
    spaceBlocks.clear();
    goldSprites.clear();
    spritesUD.clear();
    blocks.clear();
    forFly.clear();
    spritesWorkout.clear();
    if (enemies.empty()) {
        for (int i = 0; i < enemyMaxCounter; i++) {
            enemies.push_back(Enemy());
        }
    }

    std::wstring path = L"levels/level" + std::to_wstring(level) + L".txt";
    std::ifstream file(path);
    if (!file.is_open()) showError(L"Error file loading: " + path + L"\nCheck levels/.rules.txt");

    std::vector<sf::Vector2f> vectorOfPositions;
    for (int i = 0; i < windowHeight; i++) {
        for (int j = 0; j < windowWidth; j++) {
            vectorOfPositions.push_back(sf::Vector2f(j * 30.0f, i * 30.0f));
        }
    }
    int counter = 0;
    char ch;
    bool isCorrect1 = true;
    bool isCorrect2 = true;
    while (file.get(ch)) {
        if (!std::isdigit(ch) && ch != 'i') continue;

        if (counter < (windowHeight - 1) * windowWidth && ch == '9' && isCorrect1 == true) isCorrect1 = false;
        if (counter >= (windowHeight - 1) * windowWidth && ch != '9' && isCorrect2 == true) isCorrect2 = false;

        if (ch == '0' && counter < windowWidth) randEnemySpawnNums.push_back(counter);
        if ((ch == '3' || ch == '6') && counter < windowWidth) randLadderEnemySpawn.push_back(counter);

        bool udbool = false;
        bool blocksBool = false;
        bool notFly = false;
        bool workout = false;
        bool victoryUDBool = false;
        bool goldSpritesBool = false;
        bool enemy = false;
        bool spaced = false;
        bool voidBlock = false;

        sf::Sprite sprite;

        switch (ch) {
            case '0':
                voidBlock = true;
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
                if (enemyCounter == enemyMaxCounter) showError(L"Maximum number of enemeis is " + std::to_wstring(enemyMaxCounter));
                enemy = true;
                break;
            case '8':
                mainPosition = vectorOfPositions[counter];
                counter++;
                continue;
            case '9':
                blocksBool = true;
                notFly = true;
                sprite.setTexture(texture19);
                break;
            case 'i':
                sprite.setTexture(texture15);
                break;
        }

        if (counter == vectorOfPositions.size()) showError(L"Error in " + path + L"\nWrong field size\nCheck levels/.rules.txt");

        if (victoryUDBool)
            sprite.setPosition(vectorOfPositions[counter].x, vectorOfPositions[counter].y - 2000);
        else
            sprite.setPosition(vectorOfPositions[counter]);

        if (enemy == true) {
            enemies.at(enemyCounter).sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite.getGlobalBounds().top + 15);
            enemyCounter++;
        } else if (voidBlock == false) levelSprites.push_back(sprite);

        if (spaced) spaceBlocks.push_back(sprite);
        if (goldSpritesBool) goldSprites.push_back(sprite);
        if (udbool || victoryUDBool) spritesUD.push_back(sprite);
        if (blocksBool) blocks.push_back(sprite);
        if (notFly || victoryUDBool) forFly.push_back(sprite);
        if (workout) spritesWorkout.push_back(sprite);

        counter++;
    }
    if (mainPosition.x == -200.0f) showError(L"Error in " + path + L"\nThere must be the main character\nCheck levels/.rules.txt");
    if (counter != windowHeight * windowWidth) showError(L"Error in " + path + L"\nWrong field size\nCheck levels/.rules.txt");
    if (isCorrect1 == false) showError(L"Error in " + path + L"\nSymbols '9' should be only at the very bottom\nCheck levels/.rules.txt");
    if (isCorrect2 == false) showError(L"Error in " + path + L"\nThe last row should consist only of '9'\nCheck levels/.rules.txt");
}

void Game::setText(int level) {
    text.setFont(font);
    text.setString(std::to_string(level));
    text.setOrigin(text.getGlobalBounds().width / 2, text.getGlobalBounds().height / 2);
    text.setPosition(windowWidth * 30.0f / 2.0f, windowHeight * 30.0f);
    text.setCharacterSize(24);
    text.setStyle(sf::Text::Bold);
}

void Game::smoothTextures() {
    for (sf::Texture& texture : texturesToMoveLR) {
        texture.setSmooth(true);
    }
    for (sf::Texture& texture : texturesToMoveUD) {
        texture.setSmooth(true);
    }
    for (sf::Texture& texture : texturesForWorkout) {
        texture.setSmooth(true);
    }
    for (sf::Texture& texture : deletedTextures) {
        texture.setSmooth(true);
    }
    for (sf::Texture& texture : reawakenedTextures) {
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

void Game::setSprite1() {
    sprite1.setOrigin(15, 15);
    sprite1.setTexture(texture0);
    sprite1.setPosition(mainPosition.x + 15, mainPosition.y + 15);
}

bool Game::updateFly() {
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
                    for (sf::Sprite& sprite : forFly) {
                        if (rectBounds.intersects(sprite.getGlobalBounds())) {
                            sprite1.setPosition(spriteLeft + 15, sprite.getGlobalBounds().top - 15);
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

void Game::updateMoveUD(sf::Time deltaTime) {
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
    static bool tempIgnore = false;
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
        if (tempIgnore && tmp && !onUD) sprite1.setTexture(texture0);
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
    animateUD();
}

void Game::centrelizing(const sf::Sprite sprite) {
    if (sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help || sprite1.getGlobalBounds().left >= sprite.getGlobalBounds().left + help) {
        mainSpeed = sprite1.getGlobalBounds().left <= sprite.getGlobalBounds().left - help ? fabs(mainSpeed) : -fabs(mainSpeed);
        sprite1.setScale((float)sprite1.getGlobalBounds().left <= (float)sprite.getGlobalBounds().left - 1.0f ? 1.0f : -1.0f, 1.0f);
        ignoreUD = true;
        return;
    }
    sprite1.setPosition(sprite.getGlobalBounds().left + 15, sprite1.getGlobalBounds().top + 15);
    ignoreUD = false;
}

void Game::animateUD() {
    sf::Time elapsedTime = clockAnimationUD.getElapsedTime();
    if (elapsedTime.asMilliseconds() > animationMoveIntervalUD) {
        frameIndexUD = (frameIndexUD + 1) % static_cast<int>(texturesToMoveUD.size());
        sprite1.setTexture(texturesToMoveUD[frameIndexUD]);
        clockAnimationUD.restart();
    }
}

void Game::initVariables() {
    enemyCounter = 0;
    enemyMaxCounter = 6;
    greenBlueOpacity = 0;
    isRed = false;
    isPause = false;
    windowWidth = 36;
    windowHeight = 20;
    generator.seed(static_cast<unsigned int>(std::time(nullptr)));
    enemyPercent = 2;
    mainPosition = sf::Vector2f(-200.0f, 0);
    flickTime = 0.4f;
    isStart = false;
    screenFade.setTexture(texture20);
    screenFade.setScale(windowWidth, windowHeight - 1);
    transitionSpeed = 2;
    opacity = 2;
    isDrawnFade = false;
    isWin = false;
    isRestart = false;
    level = getLevel();
    help = 1.0f;
    isFromFly = false;
    space = false;
    onUD = false;
    isVictory = false;
    isWorkout = false;
    ignoreUD = false;
    moveLR = false;
    movingRight = false;
    movingLeft = false;
    movingUp = false;
    movingDown = false;
    mainSpeed = 135.0f;
    frameIndexLR = 0;
    frameIndexUD = 0;
    frameIndexWorkout = 0;
    deletedBlockInterval = 7.0f;
    reawakenedInterval = 30.0f;
    miniAnimateInterval = 15.0f;
    animationMoveIntervalLR = 35.0f;
    animationMoveIntervalUD = 30.0f;
    animationMoveIntervalWorkout = 45.0f;
    deletedTextures = { texture33, texture34, texture35, texture36, texture37, texture38, texture39, texture20 };
    reawakenedTextures = { texture40, texture41, texture42, texture43, texture44, texture45, texture46, texture47, texture48, texture49, texture50, texture51, texture15 };
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
}

void Game::setWindow() {
    window.create(sf::VideoMode(windowWidth * 30, windowHeight * 30 + 20), "Lode Runner");
    window.setPosition(sf::Vector2i(-9, 0));
    window.setSize(sf::Vector2u(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height - 88));
    //window.setSize(sf::Vector2u(160 * 4,  4 * 90));
    //window.setPosition(sf::Vector2i(830, 0));
    sf::Mouse::setPosition(sf::Vector2i(0, sf::VideoMode::getDesktopMode().height + 15));
    window.setFramerateLimit(500);
}

void Game::setIcon() {
    sf::Image windowIcon = texture52.copyToImage();
    window.setIcon(windowIcon.getSize().x, windowIcon.getSize().y, windowIcon.getPixelsPtr());
}

void Game::updateMoveLR(sf::Time deltaTime) {
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
                return;
            }
        }
    }
    animateLR();
    sprite1.move(deltaTime.asSeconds() * mainSpeed, 0);
    onUD = false;
    isFromFly = false;
}

bool Game::checkRight() {
    for (sf::Sprite& block : blocks) {
        if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 0.001f)
            || block.getGlobalBounds().contains(sprite1.getGlobalBounds().left + 30.0f + help, sprite1.getGlobalBounds().top + 29.999f)) {
            return true;
        }
    }
    return false;
}

bool Game::checkLeft() {
    for (sf::Sprite& block : blocks) {
        if (block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 0.001f)
            || block.getGlobalBounds().contains(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + 29.999f)) {
            return true;
        }
    }
    return false;
}

void Game::animateLR() {
    sf::Time elapsedTime = clockAnimationLR.getElapsedTime();
    if (elapsedTime.asMilliseconds() > animationMoveIntervalLR) {
        frameIndexLR = (frameIndexLR + 1) % static_cast<int>(texturesToMoveLR.size());
        sprite1.setTexture(texturesToMoveLR[frameIndexLR]);
        clockAnimationLR.restart();
    }
}

void Game::animateWorkout() {
    sf::Time elapsedTime = clockAnimationWorkout.getElapsedTime();
    if (elapsedTime.asMilliseconds() > animationMoveIntervalWorkout) {
        frameIndexWorkout = (frameIndexWorkout + 1) % static_cast<int>(texturesForWorkout.size());
        sprite1.setTexture(texturesForWorkout[frameIndexWorkout]);
        clockAnimationWorkout.restart();
    }
}

void Game::loadTextures() {
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

int Game::getLevel() {
    getNumOfLevels();
    std::ifstream ifile("levels/.save.txt");
    char ch;
    std::string num;
    while (ifile.get(ch)) {
        if (!std::isdigit(ch)) {
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

void Game::setLevel(int level) {
    getNumOfLevels();
    std::ofstream ofile("levels/.save.txt", std::ios::trunc | std::ios::out);
    ofile << level;
    ofile.close();
}

int Game::getNumOfLevels() {
    if (!fs::directory_entry("levels").exists())
        showErrorWithLink(L"There must be folder 'levels'\nin the same directory as an executable file\nCheck rules on:\n", 3.0f);
    std::ifstream rules("levels/.rules.txt");
    if (!rules.is_open()) showErrorWithLink(L"Error file loading: levels/.rules.txt\nCheck rules on:\n", 2.0f);
    rules.close();
    std::ifstream isfile("levels/.save.txt");
    if (!isfile.is_open()) showError(L"Error file loading: levels/.save.txt\nCheck levels/.rules.txt");
    isfile.close();

    int files = 0;
    for (const auto& entry : fs::directory_iterator("levels")) {
        std::string directoryName = entry.path().filename().string();
        std::wstring wpath = entry.path().filename().wstring();
        if (directoryName == ".save.txt" || directoryName == ".rules.txt") continue;
        if (directoryName.size() < 10 || directoryName.size() > 12 || entry.is_directory())
            showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
        std::string start = directoryName.substr(0, 5);
        std::string end = directoryName.substr(directoryName.size() - 4, 4);
        int point = 1;
        for (size_t i = 6; i < directoryName.size() - 4; i++) {
            if (directoryName.at(i) != '.') point++;
            else break;
        }
        std::string center = directoryName.substr(5, point);
        if (center.at(0) == '0') showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
        bool isNum = true;
        for (size_t i = 0; i < center.size(); i++) {
            if (!isdigit(center.at(i))) showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
        }
        if (start != "level" || end != ".txt") showError(L"There are an extra directory - 'levels/" + wpath + L"'\nCheck levels/.rules.txt");
        files++;
    }
    int levels = 0;
    for (int i = 1; i <= files; i++) {
        std::wstring path = L"levels/level" + std::to_wstring(i) + L".txt";
        std::ifstream ifile(path);
        if (!ifile.is_open()) showError(L"Error file loading: " + path + L"\nCheck levels/.rules.txt");
        ifile.close();
        levels++;
    }
    return levels;
}

void Game::updateLevel(bool isNextLevel) {
    if (isNextLevel) {
        if (level == getNumOfLevels())
            level = 1;
        else
            level++;
    }
    enemyCounter = 0;
    setSprites(level);
    setSprite1();
    setEnemies();
    setText(level);
    setLevel(level);
    holes.clear();
    queueDeleted.clear();
    queueTimer.clear();
    isAnimatedDeletes.clear();
    isPushedLS.clear();
    miniAnimateDelete.clear();
    animatedSprites.clear();
    counterDeletedTextures.clear();
    killedSprites.clear();
    fpsArr.clear();
    isStart = false;
    isPause = false;
    isRed = false;
    isVictory = false;
    isFromFly = false;
    space = false;
    isVictory = false;
    isWorkout = false;
    ignoreUD = false;
    onUD = false;
    moveLR = false;
    movingRight = false;
    movingLeft = false;
    movingUp = false;
    movingDown = false;
    fps = 0;
    isPause = 0;
    flicker.restart();
}

void Game::drawTransition() {
    sf::Image img = texture20.copyToImage();
    if (opacity <= 255 - transitionSpeed && isDrawnFade == false) {
        opacity += transitionSpeed;
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 30; j++) {
                img.setPixel(i, j, sf::Color(0, 0, 0, opacity));
            }
        }
        textureFade.loadFromImage(img);
        screenFade.setTexture(textureFade);
        window.draw(screenFade);
        if (opacity > 255 - transitionSpeed) {
            if (isRestart) updateLevel(false);
            if (isWin) updateLevel(true);
            isDrawnFade = true;
            opacity = 255;
        }
    }
    if (opacity >= transitionSpeed && isDrawnFade) {
        opacity -= transitionSpeed;
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 30; j++) {
                img.setPixel(i, j, sf::Color(0, 0, 0, opacity));
            }
        }
        textureFade.loadFromImage(img);
        screenFade.setTexture(textureFade);
        window.draw(screenFade);
        if (opacity < transitionSpeed) {
            opacity = transitionSpeed;
            isDrawnFade = false;
            isRestart = false;
            isWin = false;
            deltaTimeClock.restart();
        }
    }
}

void Game::showError(std::wstring finalMessage) {
    window.close();
    sf::RenderWindow errWindow(sf::VideoMode::getFullscreenModes().back(), "Error");
    errWindow.setPosition(sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - errWindow.getSize().x) / 2), 0));
    errWindow.setFramerateLimit(100);
    while (errWindow.isOpen()) {
        sf::Event event;
        while (errWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
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

void Game::showErrorWithLink(std::wstring finalMessage, float step) {
    window.close();
    sf::RenderWindow errWindow(sf::VideoMode::getFullscreenModes().back(), "Error", sf::Style::Close | sf::Style::Titlebar);
    errWindow.setPosition(sf::Vector2i(static_cast<int>((sf::VideoMode::getDesktopMode().width - errWindow.getSize().x) / 2), 0));
    errWindow.setFramerateLimit(100);
    std::string linkString = "https://github.com/dmtrDO/Lode-Runner";
    sf::Text linkage(linkString, font);
    sf::Text finalText;
    bool isClicked = false;
    sf::Vector2f mousePosition;
    bool onText = false;
    sf::Cursor cursor;
    finalText.setFont(font);
    while (errWindow.isOpen()) {
        isClicked = false;
        sf::Event event;
        while (errWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                errWindow.close();
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                isClicked = true;
            }
            if (event.type == sf::Event::MouseMoved) {
                mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(errWindow));
            }
        }
        finalText.setString(finalMessage);
        finalText.setCharacterSize(24);
        finalText.setFillColor(sf::Color::Magenta);
        finalText.setPosition((errWindow.getSize().x - linkage.getGlobalBounds().width) / 2, (errWindow.getSize().y - finalText.getGlobalBounds().height) / 2);
        linkage.setPosition((errWindow.getSize().x - linkage.getGlobalBounds().width) / 2.0f + 2,
            (errWindow.getSize().y - finalText.getGlobalBounds().height) / 2.0f + step * linkage.getGlobalBounds().height - 3.5f);
        linkage.setFillColor(sf::Color::Magenta);
        linkage.setCharacterSize(24);
        linkage.setStyle(sf::Text::Style::Regular);
        if (linkage.getGlobalBounds().contains(mousePosition)) {
            cursor.loadFromSystem(sf::Cursor::Hand);
            errWindow.setMouseCursor(cursor);
            linkage.setFillColor(sf::Color::Red);
            linkage.setStyle(sf::Text::Style::Underlined);
            if (isClicked) {
                openLink("https://github.com/dmtrDO/Lode-Runner");
                errWindow.close();
            }
        } else {
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

void Game::setEnemies() {
    for (int i = 0; i < enemyCounter; i++) {
        enemies.at(i).sprite1.setTexture(texture13);
    }
    for (; enemyCounter < enemyMaxCounter; enemyCounter++) {
        enemies.at(enemyCounter).sprite1.setPosition(-100, 0);
    }
    Enemy::forFly = forFly;
    Enemy::spritesUD = spritesUD;
    Enemy::blocks = blocks;
    Enemy::spritesWorkout = spritesWorkout;
}

void Game::updateEnemyDeath(Enemy& enemy) {
    int counter = 0;
    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    for (sf::Sprite& sprite : blocks) {
        if (enemyBounds.intersects(sprite.getGlobalBounds())) {
            enemy.isCaught = false;
            enemy.isClimbed = false;
            enemy.caughtDirection = 1;
            enemy.changedDirectionCounter = 0;
            enemy.isDirectionChanged = false;
            enemy.direction = 1;
            enemy.isLadderDirectionChanged = false;
            enemy.ladderDirection = 1;
            std::uniform_int_distribution<int> distribution;
            if (randEnemySpawnNums.empty() == false)
                enemy.sprite1.setPosition(randEnemySpawnNums.at(distribution(generator) % randEnemySpawnNums.size()) * 30.0f + 15.0f, 15.0f - help);
            else if (randLadderEnemySpawn.empty() == false)
                enemy.sprite1.setPosition(randLadderEnemySpawn.at(distribution(generator) % randLadderEnemySpawn.size()) * 30.0f + 15.0f, 15.0f - help);
            else
                enemy.sprite1.setPosition(-30.0f, 0.0f);
            return;
        }
        counter++;
    }
}

void Game::updateGold() {
    if (goldSprites.size() == 0) {
        for (sf::Sprite& sprite : levelSprites) {
            if (sprite.getGlobalBounds().top < 0) {
                bool temp = false;
                for (sf::Sprite& spr : spaceBlocks) {
                    if (spr.getGlobalBounds().getPosition() == sprite.getGlobalBounds().getPosition()) {
                        temp = true;
                        break;
                    }
                }
                if (temp) continue;
                sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
            }
        }
        for (sf::Sprite& sprite : forFly) {
            for (sf::Sprite& sp : spritesUD) {
                if (sprite.getGlobalBounds().getPosition() == sp.getGlobalBounds().getPosition() && sprite.getGlobalBounds().top < 0)
                    sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
            }
        }
        for (sf::Sprite& sprite : spritesUD) {
            if (sprite.getGlobalBounds().top < 0) sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top + 2000);
        }
        isVictory = true;
        return;
    }

    for (int i = 0; i < goldSprites.size(); i++) {
        sf::FloatRect intersection;
        if (sprite1.getGlobalBounds().intersects(goldSprites[i].getGlobalBounds(), intersection)) {
            float area = intersection.width * intersection.height;
            if (area >= 700) {
                for (int j = 0; j < levelSprites.size(); j++) {
                    if (levelSprites[j].getGlobalBounds().top == goldSprites[i].getGlobalBounds().top && levelSprites[j].getGlobalBounds().left == goldSprites[i].getGlobalBounds().left) {
                        levelSprites[j].setTexture(texture20);
                    }
                }
                goldSprites.erase(goldSprites.begin() + i);
            }
        }
    }
}

void Game::updateEnemyPickGold(Enemy& enemy) {

    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    float enemyLeft = enemyBounds.left;
    float enemyTop = enemyBounds.top;

    sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
    helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
    helpArea.setPosition(enemyLeft + 15, enemyTop + 15);

    if (enemy.pickedGoldTime == 0) {
        for (sf::Sprite& gold : goldSprites) {

            if (!gold.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;

            sf::FloatRect intersection;
            if (enemyBounds.intersects(gold.getGlobalBounds(), intersection) && intersection.width * intersection.height >= 600 &&
                enemy.isPickedGold == false && enemy.onGold == false) {
                enemy.onGold = true;
                std::uniform_int_distribution<int> distribution;
                int fortunate = distribution(generator) % enemyPercent;
                if (fortunate == 0 && enemy.pickUpInterval.getElapsedTime().asSeconds() > 2.0f) {
                    for (sf::Sprite& sprite : levelSprites) {
                        if (sprite.getGlobalBounds().getPosition() == gold.getGlobalBounds().getPosition()) {
                            sprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top - 2000.0f);
                            enemy.goldSprite.setPosition(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top);
                            break;
                        }
                    }
                    gold.setPosition(gold.getGlobalBounds().left, gold.getGlobalBounds().top - 2000.0f);
                    enemy.pickedGoldTime = distribution(generator) % 40 + 5;
                    enemy.pickedGoldTimer.restart();
                }
            } else {
                int counter = 0;
                sf::FloatRect area;
                for (sf::Sprite& gold : goldSprites) {
                    if (enemyBounds.intersects(gold.getGlobalBounds(), area) && area.width * area.height >= 600) counter++;
                }
                if (counter == 0) enemy.onGold = false;
            }
        }
    } else {

        for (sf::Sprite& sprite : holes) {
            if (enemyBounds.getPosition() == sprite.getGlobalBounds().getPosition() && enemy.isCaught == true) {
                int left = (int)sprite.getGlobalBounds().left;
                int top = (int)sprite.getGlobalBounds().top - 30.0f;
                bool checkUp = false;
                for (sf::Sprite& spr : forFly) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                for (sf::Sprite& spr : goldSprites) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                for (sf::Sprite& spr : holes) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                for (sf::Sprite& spr : spritesWorkout) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                for (sf::Sprite& spr : spritesUD) {
                    //if (sf::Vector2(spr.getGlobalBounds().left, spr.getGlobalBounds().top + 2000.0f) == sf::Vector2f(left, top)) { checkUp = true; break; }
                    if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; }
                }
                if (checkUp == false) {
                    for (sf::Sprite& gold : levelSprites) {
                        if (gold.getGlobalBounds().getPosition() == enemy.goldSprite.getGlobalBounds().getPosition()) gold.setPosition(left, top);
                    }
                    for (sf::Sprite& gold : goldSprites) {
                        if (gold.getGlobalBounds().getPosition() == enemy.goldSprite.getGlobalBounds().getPosition()) gold.setPosition(left, top);
                    }
                    enemy.pickedGoldTime = 0;
                    enemy.onGold = false;
                    enemy.pickUpInterval.restart();
                }
            }
        }

        sf::RectangleShape rect(sf::Vector2f(30.0f, help));
        rect.setPosition(enemyLeft, enemyTop + 30.0f);
        if (enemy.pickedGoldTimer.getElapsedTime().asSeconds() >= enemy.pickedGoldTime) {
            sf::FloatRect area;
            for (sf::Sprite& sprite : forFly) {

                if (!sprite.getGlobalBounds().intersects(helpArea.getGlobalBounds())) continue;

                if (rect.getGlobalBounds().intersects(sprite.getGlobalBounds(), area) && area.width > 15) {
                    int left = (int)sprite.getGlobalBounds().left;
                    int top = (int)sprite.getGlobalBounds().top - 30.0f;
                    bool checkUp = false;
                    for (sf::Sprite& spr : forFly) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                    for (sf::Sprite& spr : goldSprites) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                    for (sf::Sprite& spr : holes) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                    for (sf::Sprite& spr : spritesWorkout) { if (spr.getGlobalBounds().getPosition() == sf::Vector2f(left, top)) { checkUp = true; break; } }
                    for (sf::Sprite& spr : spritesUD) {
                        if (sf::Vector2(spr.getGlobalBounds().left, spr.getGlobalBounds().top + 2000.0f) == sf::Vector2f(left, top)) { checkUp = true; break; }
                    }
                    if (checkUp == false) {
                        for (sf::Sprite& gold : levelSprites) {
                            if (gold.getGlobalBounds().getPosition() == enemy.goldSprite.getGlobalBounds().getPosition()) gold.setPosition(left, top);
                        }
                        for (sf::Sprite& gold : goldSprites) {
                            if (gold.getGlobalBounds().getPosition() == enemy.goldSprite.getGlobalBounds().getPosition()) gold.setPosition(left, top);
                        }
                        enemy.pickedGoldTime = 0;
                        enemy.onGold = false;
                        enemy.pickUpInterval.restart();
                    }
                }
            }
        }
    }
}

bool Game::updateEnemiesCollisions(Enemy& enemy, sf::Time deltaTime) {

    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    float enemyLeft = enemyBounds.left;
    float enemyTop = enemyBounds.top;

    sf::RectangleShape lSide(sf::Vector2f(help, 30.0f - 2 * help)), rSide(sf::Vector2f(help, 30.0f - 2 * help));
    lSide.setPosition(enemyLeft - help, enemyTop + help);
    rSide.setPosition(enemyLeft + 30.0f, enemyTop + help);

    sf::RectangleShape uSide(sf::Vector2f(30.0f - 2 * help, help)), dSide(sf::Vector2f(30.0f - 2 * help, help));
    uSide.setPosition(enemyLeft + help, enemyTop - help);
    dSide.setPosition(enemyLeft + help, enemyTop + 30.0f);

    sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
    helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
    helpArea.setPosition(enemyLeft + 15, enemyTop + 15);

    for (Enemy& sprite : enemies) {
        sf::FloatRect sprBounds = sprite.sprite1.getGlobalBounds();
        if (lSide.getGlobalBounds().intersects(sprBounds) && enemy.movingLeft) enemy.movingLeft = false;
        if (rSide.getGlobalBounds().intersects(sprBounds) && enemy.movingRight) enemy.movingRight = false;
        if (uSide.getGlobalBounds().intersects(sprBounds) && enemy.movingUp) enemy.movingUp = false;
        if (dSide.getGlobalBounds().intersects(sprBounds) && enemy.movingDown) enemy.movingDown = false;
    }

    for (Enemy& sprite : enemies) {

        if (!helpArea.getGlobalBounds().intersects(sprite.sprite1.getGlobalBounds())) continue;

        sf::FloatRect sprBounds = sprite.sprite1.getGlobalBounds();

        if (dSide.getGlobalBounds().intersects(sprBounds) && sprite.isCaught == false && enemy.isFlyingTexture == true) {
            enemy.sprite1.setPosition(enemyLeft + 15.0f, sprBounds.top - 30.0f + 15.0f);
            enemy.isFromFly = true;
            enemy.isFlyingTexture = true;
            return true;
        }

        if (dSide.getGlobalBounds().intersects(sprBounds) && sprite.isCaught == true ||
            dSide.getGlobalBounds().intersects(sprBounds) && sprite.isCaught == false && enemy.isFlyingTexture == false) {
            if (enemyTop != sprBounds.top - 30.0f)
                enemy.sprite1.setPosition(enemyLeft + 15.0f, sprBounds.top - 30.0f + 15.0f);
            if (enemy.updateCaught(deltaTime, sprite1)) return true;
            enemy.updateMoveLR(deltaTime);
            enemy.updateMoveUD(deltaTime);
            if (enemy.isFromFly) enemy.sprite1.setTexture(texture13);
            return true;
        }
        if (uSide.getGlobalBounds().intersects(sprBounds) && enemy.isCaught) return true;
        if ((lSide.getGlobalBounds().intersects(sprBounds) && enemy.caughtDirection == -1 ||
            rSide.getGlobalBounds().intersects(sprBounds) && enemy.caughtDirection == 1)
            && enemy.isClimbed) {
            enemy.isClimbed = false;
            return true;
        }
    }

    return false;
}

void Game::updateEnemies(sf::Time& deltaTime) {
    Enemy::forFly = forFly;
    Enemy::spritesUD = spritesUD;
    Enemy::blocks = blocks;
    Enemy::holes = holes;
    Enemy::spritesWorkout = spritesWorkout;

    if (isStart == false) return;

    for (Enemy& enemy : enemies) {
        enemy.initMoves();
        setEnemyMove(enemy);
        //enemy.movingLeft = movingLeft; enemy.movingRight = movingRight; enemy.movingUp = movingUp; enemy.movingDown = movingDown;
        updateEnemyDeath(enemy);
        updateEnemyPickGold(enemy);
        if (updateEnemiesCollisions(enemy, deltaTime) == true || enemy.updateCaught(deltaTime, sprite1)) continue;
        if (enemy.updateFly()) {
            enemy.sprite1.move(0, fabs(enemy.mainSpeed) * deltaTime.asSeconds());
            enemy.sprite1.setTexture(texture13);
            enemy.isFromFly = true;
            enemy.isFlyingTexture = true;
            continue;
        }
        enemy.updateMoveLR(deltaTime);
        enemy.updateMoveUD(deltaTime);
        if (enemy.isFromFly) enemy.sprite1.setTexture(texture13);
    }
}

bool Game::updateOnEnemy(sf::Time deltaTime) {

    sf::FloatRect spriteBounds = sprite1.getGlobalBounds();
    float spriteLeft = spriteBounds.left;
    float spriteTop = spriteBounds.top;

    sf::RectangleShape helpArea(sf::Vector2f(90.0f, 90.0f));
    helpArea.setOrigin(helpArea.getLocalBounds().width / 2, helpArea.getLocalBounds().height / 2);
    helpArea.setPosition(spriteLeft + 15, spriteTop + 15);

    sf::RectangleShape under(sf::Vector2f(30.0f - 2 * help, help));
    under.setPosition(sprite1.getGlobalBounds().left + help, sprite1.getGlobalBounds().top + 30.0f);

    for (Enemy& enemy : enemies) {

        if (!helpArea.getGlobalBounds().intersects(enemy.sprite1.getGlobalBounds())) continue;

        sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
        float enemyLeft = enemyBounds.left;
        float enemyTop = enemyBounds.top;

        if (enemyLeft != spriteLeft && isFromFly) return false;

        sf::RectangleShape etop(sf::Vector2f(30.0f, 2 * help));
        etop.setPosition(enemyLeft, enemyTop);

        if (under.getGlobalBounds().intersects(etop.getGlobalBounds()) && enemy.isFlyingTexture) {

            if (spriteTop != enemyTop - 30.0f && isFromFly == true)
                sprite1.setPosition(spriteLeft + 15.0f, enemyTop + 15.0f - 30.0f);

            updateMoveUD(deltaTime);
            updateSpace();

            if ((int)spriteTop % 30 == 0 && enemy.isCaught == true) {
                updateMoveLR(deltaTime);
                if (isFromFly) sprite1.setTexture(texture13);
                return true;
            }

            bool isEnemyOnPlatform = false;
            sf::RectangleShape enemyUnder(sf::Vector2f(30.0f - 2 * help, help));
            enemyUnder.setPosition(enemyLeft + help, enemyTop + 30.0f);
            sf::FloatRect area;
            for (sf::Sprite& sprite : forFly) {
                if (sprite.getGlobalBounds().intersects(enemyUnder.getGlobalBounds(), area) && area.height == help) {
                    isEnemyOnPlatform = true;
                    break;
                }
            }
            for (Enemy& enemy : enemies) {
                if (enemy.sprite1.getGlobalBounds().intersects(enemyUnder.getGlobalBounds(), area) && area.height == help) {
                    isEnemyOnPlatform = true;
                    break;
                }
            }

            if (isEnemyOnPlatform == false) {
                sf::RectangleShape lSide(sf::Vector2f(help, 30.0f - 2 * help)), rSide(sf::Vector2f(help, 30.0f - 2 * help));
                lSide.setPosition(spriteLeft - help, spriteTop + help);
                rSide.setPosition(spriteLeft + 30.0f, spriteTop + help);
                sf::FloatRect intersection;

                for (sf::Sprite& sprite : spritesUD) {
                    if (lSide.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.height >= 30.0f - 2 * help && movingLeft) {
                        sprite1.setPosition(sprite.getGlobalBounds().left + 30.0f - help - 0.1f + 15.0f, sprite.getGlobalBounds().top + 15.0f);
                        return false;
                    }
                    if (rSide.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.height >= 30.0f - 2 * help && movingRight) {
                        sprite1.setPosition(sprite.getGlobalBounds().left - 30.0f + help + 0.1f + 15.0f, sprite.getGlobalBounds().top + 15.0f);
                        return false;
                    }
                }

                for (sf::Sprite& sprite : spritesWorkout) {
                    if (lSide.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.height >= 30.0f - 2 * help && movingLeft) {
                        sprite1.setPosition(sprite.getGlobalBounds().left + 30.0f - help - 0.1f + 15.0f, sprite.getGlobalBounds().top + 15.0f);
                        return false;
                    }
                    if (rSide.getGlobalBounds().intersects(sprite.getGlobalBounds(), intersection) && intersection.height >= 30.0f - 2 * help && movingRight) {
                        sprite1.setPosition(sprite.getGlobalBounds().left - 30.0f + help + 0.1f + 15.0f, sprite.getGlobalBounds().top + 15.0f);
                        return false;
                    }
                }

                sf::RectangleShape rRect(sf::Vector2f(help, help)), lRect(sf::Vector2f(help, help));
                lRect.setPosition(spriteLeft - help, spriteTop + 30.0f);
                rRect.setPosition(spriteLeft + 30.0f, spriteTop + 30.0f);

                sf::FloatRect area;
                for (sf::Sprite& sprite : forFly) {
                    if (sprite.getGlobalBounds().intersects(rRect.getGlobalBounds(), area) && movingRight &&
                        float((int)spriteTop % 30 == 0) && checkR() == false) {
                        sprite1.setPosition(spriteLeft + 15.0f, (int)spriteTop + 15.0f);
                        updateMoveLR(deltaTime);
                        return true;
                    }
                    if (sprite.getGlobalBounds().intersects(lRect.getGlobalBounds(), area) && movingLeft &&
                        float((int)spriteTop % 30 == 0) && checkL() == false) {
                        sprite1.setPosition(spriteLeft + 15.0f, (int)spriteTop + 15.0f);
                        updateMoveLR(deltaTime);
                        return true;
                    }
                }

            } else {

                if ((movingLeft && !checkLeft() || movingRight && !checkRight()) && spriteLeft == enemyLeft && (int)spriteTop % 30 == 0) {
                    if (movingLeft) sprite1.setPosition(spriteLeft + 15.0f - help - 0.1f, spriteTop + 15.0f);
                    if (movingRight) sprite1.setPosition(spriteLeft + 15.0f + help + 0.1f, spriteTop + 15.0f);
                }

                if ((int)spriteTop % 30 == 0) updateMoveLR(deltaTime);
                if (isFromFly) sprite1.setTexture(texture13);

            }

            return true;
        }
    }
    return false;
}

bool Game::checkL() {
    sf::RectangleShape lSide(sf::Vector2f(help, 30.0f - 2 * help));
    lSide.setPosition(sprite1.getGlobalBounds().left - help, sprite1.getGlobalBounds().top + help);
    for (sf::Sprite& block : blocks) {
        if (lSide.getGlobalBounds().intersects(block.getGlobalBounds())) return true;
    }
    return false;
}

bool Game::checkR() {
    sf::RectangleShape rSide(sf::Vector2f(help, 30.0f - 2 * help));
    rSide.setPosition(sprite1.getGlobalBounds().left + 30.0f, sprite1.getGlobalBounds().top + help);
    for (sf::Sprite& block : blocks) {
        if (rSide.getGlobalBounds().intersects(block.getGlobalBounds())) return true;
    }
    return false;
}

bool Game::isEnemyFlying(sf::Sprite& enemy) {
    sf::FloatRect enemyBounds = enemy.getGlobalBounds();
    sf::RectangleShape under(sf::Vector2f(30.0f, help));
    under.setPosition(enemyBounds.left, enemyBounds.top + 30.0f);

    for (sf::Sprite& ladder : spritesUD) {
        if (enemyBounds.intersects(ladder.getGlobalBounds())) return false;
    }
    for (sf::Sprite& work : spritesWorkout) {
        if (enemyBounds.intersects(work.getGlobalBounds()) && enemyBounds.top == work.getGlobalBounds().top) return false;
    }
    for (sf::Sprite& platform : forFly) {
        sf::FloatRect platformBounds = platform.getGlobalBounds();
        if (under.getGlobalBounds().intersects(platformBounds) && !enemyBounds.intersects(platformBounds)) return false;
    }
    for (Enemy& esprite : enemies) {
        if (esprite.isCaught && esprite.isFlyingTexture && under.getGlobalBounds().intersects(esprite.sprite1.getGlobalBounds())) return false;
    }

    return true;
}

bool Game::isEnableEnemyRight(Enemy& enemy) {
    if (isEnemyFlying(enemy.sprite1) == true) return false;

    sf::RectangleShape rSide(sf::Vector2f(help, 30.0f - 2 * help));
    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();

    if (enemyBounds.left >= 30.0f * windowWidth - 30.0f) return false;

    rSide.setPosition(enemyBounds.left + 30.0f, enemyBounds.top + help);

    for (sf::Sprite& block : blocks) {
        if (rSide.getGlobalBounds().intersects(block.getGlobalBounds())) return false;
    }
    for (Enemy& esprite : enemies) {
        if (rSide.getGlobalBounds().intersects(esprite.sprite1.getGlobalBounds())) return false;
    }

    return true;
}

bool Game::isEnableEnemyLeft(Enemy& enemy) {
    if (isEnemyFlying(enemy.sprite1) == true) return false;

    sf::RectangleShape lSide(sf::Vector2f(help, 30.0f - 2 * help));
    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();

    if (enemyBounds.left <= 0) return false;

    lSide.setPosition(enemyBounds.left - help, enemyBounds.top + help);

    for (sf::Sprite& block : blocks) {
        if (lSide.getGlobalBounds().intersects(block.getGlobalBounds())) return false;
    }
    for (Enemy& esprite : enemies) {
        if (lSide.getGlobalBounds().intersects(esprite.sprite1.getGlobalBounds())) return false;
    }

    return true;
}

bool Game::isEnableEnemyUp(Enemy& enemy) {
    if (isEnemyFlying(enemy.sprite1) == true) return false;

    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    if (enemyBounds.top <= 0) return false;
    sf::RectangleShape uSide(sf::Vector2f(30.0f - 2 * help, help));
    uSide.setPosition(enemyBounds.left + help, enemyBounds.top - help);

    int counter = 0;
    sf::FloatRect intersection;
    for (sf::Sprite& ladder : spritesUD) {
        if (enemyBounds.intersects(ladder.getGlobalBounds(), intersection) && intersection.width > 15.0f) {
            counter++;
            break;
        }
    }
    if (counter == 0) return false;
    for (sf::Sprite& block : blocks) {
        if (uSide.getGlobalBounds().intersects(block.getGlobalBounds())) return false;
    }
    for (Enemy& esprite : enemies) {
        if (uSide.getGlobalBounds().intersects(esprite.sprite1.getGlobalBounds())) return false;
    }

    return true;
}

bool Game::isEnableEnemyDown(Enemy& enemy) {
    if (isEnemyFlying(enemy.sprite1) == true) return false;

    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    sf::RectangleShape dSide(sf::Vector2f(30.0f - 2 * help, help));
    dSide.setPosition(enemyBounds.left + help, enemyBounds.top + 30.0f);

    bool isWorking = false;
    for (sf::Sprite& work : spritesWorkout) {
        if (enemyBounds.intersects(work.getGlobalBounds()) && work.getGlobalBounds().top == enemyBounds.top) {
            isWorking = true;
            break;
        }
    }

    for (sf::Sprite& block : blocks) {
        if (dSide.getGlobalBounds().intersects(block.getGlobalBounds())) return false;
    }
    for (Enemy& esprite : enemies) {
        if (dSide.getGlobalBounds().intersects(esprite.sprite1.getGlobalBounds())) return false;
    }

    if (isWorking) return true;

    sf::FloatRect intersection;
    for (sf::Sprite& ladder : spritesUD) {
        if (dSide.getGlobalBounds().intersects(ladder.getGlobalBounds(), intersection) && intersection.width > 15.0f) return true;
    }

    int counter = 0;
    for (sf::Sprite& ladder : spritesUD) {
        if (enemyBounds.intersects(ladder.getGlobalBounds(), intersection) && intersection.width > 15.0f) {
            counter++;
            break;
        }
    }
    if (counter == 0) return false;

    return true;
}

void Game::setEnemyMove(Enemy& enemy) {

    sf::FloatRect enemyBounds = enemy.sprite1.getGlobalBounds();
    sf::FloatRect spriteBounds = sprite1.getGlobalBounds();

    if (enemy.isCaught || enemy.isClimbed || isEnemyFlying(enemy.sprite1) || (enemyBounds.left == spriteBounds.left && enemy.isFlyingTexture)) {
        enemy.isDirectionChanged = false;
        enemy.direction = 0;
        return;
    }

    if (spriteBounds.top > enemyBounds.top + help && isEnableEnemyDown(enemy)) {
        enemy.isDirectionChanged = false;
        enemy.direction = 0;
        enemy.movingDown = true;
        enemy.ladderDirection = -1;
        return;

    } else if (spriteBounds.top < enemyBounds.top - help && isEnableEnemyUp(enemy)) {
        enemy.isDirectionChanged = false;
        enemy.direction = 0;
        enemy.movingUp = true;
        enemy.ladderDirection = 1;
        return;

    } else if (spriteBounds.left > enemyBounds.left + help && isEnableEnemyRight(enemy) && enemy.isDirectionChanged == false) {
        enemy.movingRight = true;
        enemy.direction = 1;
        return;

    } else if (spriteBounds.left < enemyBounds.left - help && isEnableEnemyLeft(enemy) && enemy.isDirectionChanged == false) {
        enemy.movingLeft = true;
        enemy.direction = -1;
        return;

    } else {



        if (enemy.isDirectionChanged == false) {
            if (enemy.direction == 1) {
                enemy.direction = -1;
                enemy.movingLeft = true;
                enemy.isDirectionChanged = true;
            } else if (enemy.direction == -1) {
                enemy.direction = 1;
                enemy.movingRight = true;
                enemy.isDirectionChanged = true;
            }
        } else {
            if (enemy.direction == 1) {
                enemy.movingRight = true;
                if (isEnableEnemyRight(enemy) == false) enemy.isDirectionChanged = false;
            } else if (enemy.direction == -1) {
                enemy.movingLeft = true;
                if (isEnableEnemyLeft(enemy) == false) enemy.isDirectionChanged = false;
            }
        }

    }



}


