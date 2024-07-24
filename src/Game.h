
#include "SFML/Graphics.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>

class Game
{
public:
	Game(std::string path, int level, sf::Vector2i windowPosition, sf::Vector2u windowSize);

	sf::RenderWindow window;

	void handle();
	void update();
	void draw();

	bool getOpen() const;
	bool isIgnoreLevel() const;
	sf::Vector2i getWinpos() const;

private:

	bool ignoreNextLevel;

	bool open;

	std::string pathToLevel;

	void setWindow(sf::Vector2i windowsPosition, sf::Vector2u windowSize);
	///// 23 x 32 blocks
   ///// 1 block = 30 x 30 pixels

	sf::Image windowIcon;
	void setIcon();

	sf::Clock deltaTimeClock;

	void loadTextures();

	sf::Texture texture0;
	sf::Texture texture1;
	sf::Texture texture2;
	sf::Texture texture3;
	sf::Texture texture4;
	sf::Texture texture5;
	sf::Texture texture6;
	sf::Texture texture7;
	sf::Texture texture8;
	sf::Texture texture9;
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

	void smoothTextures();

	std::vector <sf::Sprite> levelSprites;

	sf::Sprite sprite1;
	sf::Vector2f mainPosition;
	void setSprite1();

	void setSprites();

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

	std::vector<sf::Sprite> spritesUD;
	std::vector<sf::Sprite> blocks;

	bool checkLeft();
	bool checkRight();

	std::vector <sf::Sprite> forFly;

	bool updateFly();

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

	void updateGold();
	bool isVictory;

	bool space;
	void updateSpace(sf::Time deltaTime);
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

	std::vector<sf::Sprite> enemies;
	void setEnemies();

	void updateDeath();
	bool end;
	void restart();

	sf::Vector2i winpos;
};


