
#ifndef ENEMY
#define ENEMY

#include <iostream>
#include <random>
#include "images.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

class Enemy {

public:

	Enemy();
	void initVariables();

	sf::Sprite sprite1;

	int direction;
	bool isDirectionChanged;
	int ladderDirection;
	bool isLadderDirectionChanged;
	sf::Clock directionClock;
	float currPosX;
	int framesX;
	bool isLadderException;

	bool movingLeft;
	bool movingRight;
	bool movingDown;
	bool movingUp;
	float mainSpeed;
	
	void initMoves();

	bool isFlyingTexture;
	bool isFromFly;
	bool updateFly();
	bool updateCaught(sf::Time deltaTime, sf::Sprite& goal);
	bool isCaught;
	bool isClimbed;
	int caughtDirection;
	int changedDirectionCounter;

	bool isPickedGold;
	bool onGold;
	sf::Clock pickedGoldTimer;
	sf::Clock pickUpInterval;
	int pickedGoldTime;
	std::mt19937 generator;
	std::uniform_int_distribution<int> distribution;
	sf::Sprite goldSprite;

	void updateMoveLR(sf::Time deltaTime);
	void updateMoveUD(sf::Time deltaTime);

	static std::vector <sf::Sprite> forFly;
	static std::vector <sf::Sprite> spritesWorkout;
	static std::vector <sf::Sprite> spritesUD;
	static std::vector <sf::Sprite> blocks;
	static std::vector <sf::Sprite> holes;

private:

	int windowWidth;
	int windowHeight;

	sf::Clock caughtTimer;
	sf::Sprite hole;

	void centrelizing(const sf::Sprite sprite);

	bool checkLeft() const;
	bool checkRight() const;
	bool checkEnemyLeft() const;
	bool checkEnemyRight() const;

	sf::Clock clockAnimationLR;
	sf::Clock clockAnimationUD;
	sf::Clock clockAnimationWorkout;

	void animateUD();
	void animateLR();
	void animateWorkout();

	bool space;
	bool onUD;
	bool isWorkout;
	bool ignoreUD;
	bool moveLR;
	bool tempIgnore;
	int frameIndexLR;
	int frameIndexUD;
	int frameIndexWorkout;
	float help;
	float animationMoveIntervalLR;
	float animationMoveIntervalUD;
	float animationMoveIntervalWorkout;

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
	std::vector<sf::Texture> texturesToMoveLR;
	std::vector<sf::Texture> texturesToMoveUD;
	std::vector<sf::Texture> texturesForWorkout;
};

#endif


