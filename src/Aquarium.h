#pragma once
#define NOMINMAX 1 // To avoid min/max macro conflict on Windows

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Core.h"


class BossAttackPower;

enum class AquariumCreatureType {
    NPCreature,
    BiggerFish,
    ZaggyFish, //new fish
    Slowfish, //new fish
    BossFish, //New boss fish
    PowerUp
};

string AquariumCreatureTypeToString(AquariumCreatureType t);

class AquariumLevelPopulationNode{
    public:
        AquariumLevelPopulationNode() = default;
        AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population) {
            this->creatureType = creature_type;
            this->population = population;
            this->currentPopulation = 0;
        };
        AquariumCreatureType creatureType;
        int population;
        int currentPopulation;
};

class AquariumLevel : public GameLevel {
    public:
        AquariumLevel(int levelNumber, int targetScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore){};
        void ConsumePopulation(AquariumCreatureType creature, int power);
        bool isCompleted() override;
        void populationReset();
        void levelReset(){m_level_score=0;this->populationReset();}
        virtual std::vector<AquariumCreatureType> Repopulate();

        void setBackGSprite(std::shared_ptr<GameSprite> sprite) { m_background_sprite = sprite; }
        std::shared_ptr<GameSprite> getBackGSprite() const {return m_background_sprite; }
        void drawBackG() {
            if(m_background_sprite) {
                m_background_sprite->draw(0, 0);
            }
        }
    protected:
        std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
        int m_level_score;
        int m_targetScore;
        std::shared_ptr<GameSprite> m_background_sprite; //new background for boss level

};


class PlayerCreature : public Creature {
public:

    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move();
    void draw() const;
    void update();
    void changeSpeed(int speed);
    void setLives(int lives) { m_lives = lives; }
    void setDirection(float dx, float dy);
    float isXDirectionActive() { return m_dx != 0; }
    float isYDirectionActive() {return m_dy != 0; }
    float getDx() { return m_dx; }
    float getDy() { return m_dy; }

    int getScore()const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    int m_speedBoostTimer = 0; // variable for the timer 
    
    void addToScore(int amount, int weight=1) { m_score += amount * weight; }
    void loseLife(int debounce);
    void increasePower(int value) { m_power += value; }
    void reduceDamageDebounce();
    // Returns true if the player is still in damage debounce frames
    bool isDamageDebounce() const { return m_damage_debounce > 0;}

private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1; // mark current power lvl
    int m_damage_debounce = 0; // frames to wait after eating
};

class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() {return this->m_creatureType;}
    void move() override;
    void draw() const override;
    void reverseDirection();
protected:
    AquariumCreatureType m_creatureType;

};

class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

// Inheritance class for the new fish (ZaggyFish)
class ZaggyFish : public NPCreature {
public:
    ZaggyFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

// Inheritance class for the new fish (slowfish)
class Slowfish : public NPCreature {
public:
    Slowfish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};

//Inheritance class for the boss fish
class BossFish : public NPCreature {
    private: 
        int health;
        float coolDownAttack; //cools down in between attacks in seconds
        float lastAttackTime; // time that passes since last attack
        std::vector<std::shared_ptr<BossAttackPower>> m_Attacks_Circles;
        std::shared_ptr<PlayerCreature> m_player; // pass reference of player to store in boss fish class
        bool m_isRemoved = false;
        bool m_hasGivenScore = false;
    public:
        BossFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);

        void SetPlayer(std::shared_ptr<PlayerCreature> player) { m_player = player; }
        std::shared_ptr<PlayerCreature> GetPlayer() const { return m_player; }
        int m_targetScore = 40;  // default value for target score

        void move() override;
        void draw() const override;
        void update(float dt, bool& playerDied); //updates the attack timer and moves the boss
        void shootAttack();

        bool IsRemoved() const { return m_isRemoved; }
        void SetRemoved(bool removed) { m_isRemoved = removed; }
        int getHealth() const { return health; }
        std::vector<std::shared_ptr<BossAttackPower>>& getAttackPower() { return m_Attacks_Circles; }
};

//class for the boss attack Power 
class BossAttackPower : public Creature {
    private:
        ofColor m_ofColor;
        float radius;
        bool m_delete_attack = false;
    public :
        BossAttackPower(float x, float y, float dx, float dy, float radius, ofColor color);
        void move() override;
        void update();
        void draw() const override;

        float getRadius() const { return radius; }
        void DeleteAttack() { m_delete_attack = true; }
        bool isAttackDelete() const { return m_delete_attack; }
};

//class for the speed power-Up
class PowerUpSpeed : public Creature {
public:
    PowerUpSpeed(float x, float y);
    void move() override;
    void draw() const override;
};

class AquariumSpriteManager {
    public:
        AquariumSpriteManager();
        ~AquariumSpriteManager() = default;
        std::shared_ptr<GameSprite>GetSprite(AquariumCreatureType t);
    private:
        std::shared_ptr<GameSprite> m_npc_fish;
        std::shared_ptr<GameSprite> m_big_fish;
        std::shared_ptr<GameSprite> m_zaggy_fish;
        std::shared_ptr<GameSprite> m_slowfish;
        std::shared_ptr<GameSprite> m_boss_fish;
};


class Aquarium{
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update(bool moveCreatures = true);
    void draw() const;
    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }
    void Repopulate();
    void SpawnCreature(AquariumCreatureType type, std::shared_ptr<PlayerCreature> player = nullptr);
    
    std::shared_ptr<Creature> getCreatureAt(int index);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    //getters for the current level and the aquarium levels
    int getCurrentLevelI() const { return currentLevel; }
    const std::vector<std::shared_ptr<AquariumLevel>>& getAquariumLevels() const { return m_aquariumlevels; }

private:
    int m_maxPopulation = 0;
    int m_width;
    int m_height;
    int currentLevel = 0;
    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<Creature>> m_next_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;
};


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


class AquariumGameScene : public GameScene {
    public:
        AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, string name)
        : m_player(std::move(player)) , m_aquarium(std::move(aquarium)), m_name(name){}
        std::shared_ptr<GameEvent> GetLastEvent(){return m_lastEvent;}
        void SetLastEvent(std::shared_ptr<GameEvent> event){this->m_lastEvent = event;}
        std::shared_ptr<PlayerCreature> GetPlayer(){return this->m_player;}
        std::shared_ptr<Aquarium> GetAquarium(){return this->m_aquarium;}
        string GetName()override {return this->m_name;}
        void Update() override;
        void Draw() override;
        bool m_isBossSpawned = false;
    private:
        void paintAquariumHUD();
        std::shared_ptr<PlayerCreature> m_player;
        std::shared_ptr<Aquarium> m_aquarium;
        std::shared_ptr<GameEvent> m_lastEvent;
        string m_name;
        AwaitFrames updateControl{5};
};


class Level_0 : public AquariumLevel  {
    public:
        Level_0(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 10));

        };
};
class Level_1 : public AquariumLevel  {
    public:
        Level_1(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 20));

        };
};
class Level_2 : public AquariumLevel  {
    public:
        Level_2(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 30));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));

        };
};

//New levels 
class Level_3 : public AquariumLevel {
    public:
        Level_3(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore) {
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 35));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZaggyFish, 4));
        };
};

class Level_4 : public AquariumLevel {
    public:
        Level_4(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore) {
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 40));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::ZaggyFish, 4));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::Slowfish, 3));
        };
};

//Boss Level
class Level_Boss : public AquariumLevel {
    public:
        Level_Boss(int levelNumber, int targetScore): AquariumLevel(levelNumber, targetScore) {
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 20));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::Slowfish, 2));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BossFish, 1));

            //sets the boss level background
            auto bossBackG = std::make_shared<GameSprite>("backgroundBoss.png", ofGetWidth(), ofGetHeight());
            this->setBackGSprite(bossBackG);

        }
};
