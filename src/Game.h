
#ifndef GAME
#define GAME

#include "font.h"
#include "images.h"
#include "Enemy.h"

#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Window/Event.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <random>
#include <thread>

namespace fs = std::filesystem;

class Game {
public:
	Game();

	sf::RenderWindow window;

	void handle();
	void update();
	void draw();

private:
	void setWindow();

	void setIcon();

	sf::Clock deltaTimeClock;

	void loadTextures();

	sf::Texture texture0;
	sf::Texture texture1;
	sf::Texture texture2;
	sf::Texture texture3;
	sf::Texture texture4;
	sf::Texture texture5;
	sf::Texture texture10;
	sf::Texture texture11;
	sf::Texture texture12;
	sf::Texture texture13;
	sf::Texture texture14;
	sf::Texture texture15;
	sf::Texture texture16;
	sf::Texture texture17;
	sf::Texture texture18;
	sf::Texture texture19;
	sf::Texture texture20;
	sf::Texture texture21;
	sf::Texture texture22;
	sf::Texture texture23;
	sf::Texture texture24;
	sf::Texture texture25;
	sf::Texture texture26;
	sf::Texture texture27;
	sf::Texture texture28;
	sf::Texture texture29;
	sf::Texture texture30;
	sf::Texture texture31;
	sf::Texture texture32;
	sf::Texture texture33;
	sf::Texture texture34;
	sf::Texture texture35;
	sf::Texture texture36;
	sf::Texture texture37;
	sf::Texture texture38;
	sf::Texture texture39;
	sf::Texture texture40;
	sf::Texture texture41;
	sf::Texture texture42;
	sf::Texture texture43;
	sf::Texture texture44;
	sf::Texture texture45;
	sf::Texture texture46;
	sf::Texture texture47;
	sf::Texture texture48;
	sf::Texture texture49;
	sf::Texture texture50;
	sf::Texture texture51;
	sf::Texture texture52;

	void smoothTextures();

	std::vector <sf::Sprite> levelSprites;

	sf::Sprite sprite1;
	sf::Vector2f mainPosition;
	void setSprite1();

	void setSprites(int level);

	void initVariables();

	float mainSpeed;

	std::vector<sf::Texture> texturesToMoveLR;
	bool movingRight;
	bool movingLeft;
	void updateMoveLR(sf::Time deltaTime);
	sf::Clock clockAnimationLR;
	float animationMoveIntervalLR;
	int frameIndexLR;
	void animateLR();

	std::vector<sf::Texture> texturesToMoveUD;

	bool movingUp;
	bool movingDown;
	void updateMoveUD(sf::Time deltaTime);
	sf::Clock clockAnimationUD;
	float animationMoveIntervalUD;
	int frameIndexUD;
	void animateUD();
	void centrelizing(const sf::Sprite sprite);

	bool moveLR;
	bool ignoreUD;
	bool onUD;

	std::vector<sf::Sprite> spritesUD;
	std::vector<sf::Sprite> blocks;

	bool checkLeft();
	bool checkRight();

	std::vector <sf::Sprite> forFly;

	bool updateFly();
	bool isFromFly;

	std::vector<sf::Texture> texturesForWorkout;

	void animateWorkout();
	sf::Clock clockAnimationWorkout;
	float animationMoveIntervalWorkout;
	int frameIndexWorkout;
	bool isWorkout;

	std::vector<sf::Sprite> spritesWorkout;

	sf::Font font;
	sf::Text text;
	void setText(int level);

	std::vector<sf::Sprite> victoryUD;
	std::vector<sf::Sprite> goldSprites;
	bool isVictory;

	void updateGold();

	bool space;
	sf::Clock spaceTime;
	bool checkSpace(sf::Sprite& sprite);
	void updateSpace();
	std::vector<sf::Sprite> spaceBlocks;
	void removeBlock(sf::Sprite& spaced);
	void backBlock(sf::Sprite& spaced);

	std::vector<sf::Sprite> queueDeleted;
	std::vector<sf::Clock> queueTimer;
	void animateDeleted();

	std::vector<sf::Texture> deletedTextures;
	std::vector<sf::Texture> reawakenedTextures;
	std::vector<bool> isAnimatedDeletes;
	std::vector<bool> isPushedLS;
	std::vector<sf::Clock> miniAnimateDelete;
	float miniAnimateInterval;
	std::vector<sf::Sprite> animatedSprites;
	std::vector<int> counterDeletedTextures;
	float deletedBlockInterval;
	float reawakenedInterval;
	std::vector<sf::Sprite> killedSprites;
	void updateDeath();

	bool updateHelp(sf::Time& deltaTime);
	float help;

	int level;
	int getLevel();
	void setLevel(int level);
	int getNumOfLevels();
	bool isRestart;
	bool isWin;
	void updateLevel(bool isNextLevel);

	sf::Clock flicker;
	float flickTime;

	float fps;
	sf::Clock framesClock;
	void updateFPS();
	void showError(std::wstring finalMessage);
	void showErrorWithLink(std::wstring finalMessage, float step);
	void drawLevel();
	void drawTransition();
	sf::Sprite screenFade;
	sf::Texture textureFade;
	void openLink(const std::string& url);
	bool isDrawnFade;
	unsigned char opacity;
	unsigned char transitionSpeed;

	bool isStart;
	std::vector<Enemy> enemies;
	void setEnemies();
	void setEnemyMove(Enemy& enemy);
	void updateEnemies(sf::Time& deltaTime);
	std::vector<int> randEnemySpawnNums;
	std::vector<int> randLadderEnemySpawn;
	void updateEnemyDeath(Enemy& enemy);
	std::vector<sf::Sprite> holes;
	void updateEnemyPickGold(Enemy& enemy);
	int enemyPercent;
	std::mt19937 generator;
	bool updateOnEnemy(sf::Time deltaTime);


};

#endif

