//Aquarium.cpp
#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::ZaggyFish:
            return "ZaggyFish";
        case AquariumCreatureType::Slowfish:
            return "SlowFish";
        case AquariumCreatureType::BossFish:
            return "BossFish";
        case AquariumCreatureType::PowerUp:
            return "PowerUp";
        default:
            return "UnknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    this->bounce();
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    //checks if speedBoost is active 
    if (m_speedBoostTimer > 0) {
        --m_speedBoostTimer; //subtract one from the timer in every frame 
        if (m_speedBoostTimer == 0) { // Reset speed after boost stops
            m_speed = std::max(1, m_speed - 2);
            ofLogNotice() << "Speed boost ended. Speed reset to " << m_speed;
        }
    }
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }

    //If fish loses a life the power-up will be canceled to make it cleaner 
    if (m_speedBoostTimer > 0) {
            m_speedBoostTimer = 0; // stops the timer
            m_speed = std::max(1, m_speed - 2); // revert speed boost making it go back to normal 
            ofLogNotice() << "Power-Up canceled due to life loss. Speed reset to " << m_speed;
    }

    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); 
    m_dy = (rand() % 3 - 1); 
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}


BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

//Zaggy fish's logic implementation
ZaggyFish::ZaggyFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite) : NPCreature(x, y, speed, sprite) {
    setCollisionRadius(40);
    m_value = 4;
    m_creatureType = AquariumCreatureType::ZaggyFish;
}
//movement implementation
void ZaggyFish::move() {
    m_x += m_dx * m_speed;
    m_y += sin(ofGetElapsedTimef() * 5) * 10; // applies zig-zag movement
    if(m_dx < 0) {
        m_sprite->setFlipped(true);
    } else {
        m_sprite->setFlipped(false);
    }
    bounce();
}
void ZaggyFish::draw() const {
    if(m_sprite) {
        this->m_sprite->draw(this->m_x, this->m_y);
    }
}

//Slow fish's logic implementation
Slowfish::Slowfish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite) : NPCreature(x, y, speed, sprite) {
    setCollisionRadius(50);
    m_value = 6;
    m_creatureType = AquariumCreatureType::Slowfish;
}
//Movement Implementation
void Slowfish::move() {
    m_x += m_dx * (m_speed * 0.25); // Moves slower than the other fish
    m_y += m_dy * (m_speed * 0.25) + sin(ofGetElapsedTimef() * 2) * 2; //small vertical drift 
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}
void Slowfish::draw() const {
    if(m_sprite) {
        this->m_sprite->draw(this->m_x, this->m_y);
    }
}

//New boss logic implementation
BossFish::BossFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
                  : NPCreature(x, y, speed, sprite) {
    this->health = 4;
    this->m_value = 100;
    setCollisionRadius(80);
    coolDownAttack = 2.0f;
    lastAttackTime = 0.0f;
    m_dx = 1; // moves horizontally
    m_dy = 0;
    
    m_creatureType = AquariumCreatureType::BossFish;
}
void BossFish::move() { 
    m_x += m_dx * m_speed; // moves in revers direction when it hit the edges
    m_y += sin(ofGetElapsedTimef() * 2.0f) * 2.0f; // slight vertical sinusoidal moves

    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    // Reverse direction on edges
    if (m_x < 0) {
        m_x = 0;
        m_dx = -m_dx;
    }
    if (m_x + 200 > ofGetWidth()) {
        m_x = ofGetWidth() - 200;
        m_dx = -m_dx;
    }
}
void BossFish::draw() const { //Draws the sprite of Boss fish
    if(m_sprite) {
        this->m_sprite->draw(this->m_x, this->m_y);
    }
    for(auto& circle : m_Attacks_Circles) {
        circle->draw();
    }
}

void BossFish::update(float dt, bool& playerDied) {
    playerDied = false; //bool indicating if player died
    move(); //Boss fishe's movement

    // timer for attack spawn
    lastAttackTime += dt > 0 ? dt : 1.0f / 60.0f;
    if (lastAttackTime >= coolDownAttack) {
        shootAttack();
        lastAttackTime = 0.0f;
    }

    // collision between boss and player
    if (m_player) {
        float dx = (m_x + 100) - m_player->getX();
        float dy = (m_y + 100) - m_player->getY();
        float distance = dx * dx + dy * dy;
        float radiusSum = getCollisionRadius() + m_player->getCollisionRadius();

        if (distance < radiusSum * radiusSum) {
            // Only hurt player and boss stays invincible
            m_player->loseLife(3*60);
            m_dx = -m_dx;
            m_dy = -m_dy;
            m_player->setDirection(-m_player->getDx(), -m_player->getDy());
            if (m_player->getLives() <= 0) {
                playerDied = true;
                return;
            }
        }
    }

    //updates the attack circles and checks collision with player
    for(auto it = m_Attacks_Circles.begin(); it != m_Attacks_Circles.end();) {
        auto& circle = *it;
        circle->update();
        //Circle collsion with player
        if(m_player) {
            float dx = circle->getX() - m_player->getX();
            float dy = circle->getY() - m_player->getY();
            float circleDistance = dx * dx + dy * dy;
            float radiusSum = circle->getRadius() + m_player->getCollisionRadius();
            if(circleDistance < radiusSum * radiusSum) {
                m_player->loseLife(3*60);
                it = m_Attacks_Circles.erase(it);
                if (m_player->getLives() <= 0) {
                    playerDied = true;
                    return;
                }
                continue; 
            }
        }
        //removes circle that goes out of the bounds
        if(circle->getX() < 0 || circle->getX() > ofGetWidth() || circle->getY() < 0 || circle->getY() > ofGetHeight()) {
            it = m_Attacks_Circles.erase(it);
        } else {
            ++it;
        }
    }
    //removes boss
    if (m_player && m_player->getScore() >= m_targetScore) {
        m_isRemoved = true;
    }
}
void BossFish::shootAttack() { //shoots a circle from the boss fish mouth located at the right-center of sprite boss fish
    if(!m_player) return;
    //Direction toward the player 
    float centerX = m_x + 100;
    float centerY = m_y + 100;
    float dx = m_player->getX() - centerX;
    float dy = m_player->getY() - centerY;
    float magnitude = sqrt(dx * dx + dy * dy);
    if(magnitude == 0) magnitude = 1;
    dx /= magnitude;
    dy /= magnitude;
    //the speed of the circle attacks
    float speed = 10.0f;
    dx *= speed;
    dy *= speed;
    auto ball = std::make_shared<BossAttackPower>(centerX, centerY, dx, dy, 8.0f, ofColor::violet); 
    m_Attacks_Circles.push_back(ball);

    ofLogNotice() << "Attack circle spawned at: (" << centerX << ", " << centerY << ")";
}

//PowerUp implementation 
PowerUpSpeed::PowerUpSpeed(float x, float y) : Creature(x, y, 0, 0.0f, 0, nullptr) {
    setCollisionRadius(40); 
}
void PowerUpSpeed::move() {}

// draws the power up as a simple red circle
void PowerUpSpeed::draw() const {
    ofSetColor(ofColor::red);
    ofDrawCircle(m_x, m_y, 10);
    ofSetColor(ofColor::white);
}

//boss attack implementation
BossAttackPower::BossAttackPower(float x, float y, float dx, float dy, float radius, ofColor color) : Creature(x, y, 0, radius, 0, nullptr) {
    this->radius = radius;
    this->m_ofColor = color;
    this->m_dx = dx;
    this->m_dy = dy;
}
void BossAttackPower::move() {
    m_x += m_dx;
    m_y += m_dy;

}
void BossAttackPower::update() {
    move();
}
void BossAttackPower::draw() const {
    ofSetColor(m_ofColor);
    ofDrawCircle(m_x, m_y, radius);
    ofSetColor(ofColor::white);
}

// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_zaggy_fish = std::make_shared<GameSprite>("zaggy-fish.png", 80,80);
    this->m_slowfish = std::make_shared<GameSprite>("slowfish.png", 100, 120);
    this->m_boss_fish = std::make_shared<GameSprite>("bossFish.png", 200, 200); //Sprite Boss
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
        case AquariumCreatureType::ZaggyFish:
            return std::make_shared<GameSprite>(*this->m_zaggy_fish);
            
        case AquariumCreatureType::Slowfish:
            return std::make_shared<GameSprite>(*this->m_slowfish);
         case AquariumCreatureType::BossFish:
            return std::make_shared<GameSprite>(*this->m_boss_fish);
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update(bool moveCreatures) {
    bool hasPowerUp = false; // initial condition for to know if player has the powerup
    for (auto& creature : m_creatures) {
        // Only moves the creatures if the flag is true 
        if(moveCreatures && !std::dynamic_pointer_cast<BossFish>(creature)) {
            creature->move();
        }

        //spawn the power-up so it would exist one at a time 
        if(std::dynamic_pointer_cast<PowerUpSpeed>(creature)) {
            hasPowerUp = true;
        }
    }
    static int frameCounter = 0;
    frameCounter++;

    // Occasionally spawn a power-up in every few seconds (aprox every 4 seconds per frame) 
    // and only if the power-Up doesn't exists yet
    if (!hasPowerUp && frameCounter % 240 == 0) {
        this->SpawnCreature(AquariumCreatureType::PowerUp);
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type, std::shared_ptr<PlayerCreature> player) {
    int x = 20 + rand() % (this->getWidth() - 40);
    int y = 20 + rand() % (this->getHeight() - 40);
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::ZaggyFish:
            this->addCreature(std::make_shared<ZaggyFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::ZaggyFish)));
            break;
        case AquariumCreatureType::Slowfish:
            this->addCreature(std::make_shared<Slowfish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::Slowfish)));
            break;
        case AquariumCreatureType::BossFish: {
            // Prevent duplicate bosses
            for (auto& creature : m_creatures) {
            if (std::dynamic_pointer_cast<BossFish>(creature)) return; // boss already exists
            }
            int centerX = this->getWidth() / 2 - 100;
            int centerY = this->getHeight() / 2 - 100;
            auto bossSprite = this->m_sprite_manager->GetSprite(AquariumCreatureType::BossFish);
            auto boss = std::make_shared<BossFish>(centerX, centerY, 2, bossSprite);
            boss->SetPlayer(player);
            m_creatures.push_back(std::static_pointer_cast<Creature>(boss));
            break;   
        }
        case AquariumCreatureType::PowerUp:
            this->addCreature(std::make_shared<PowerUpSpeed>(x, y));
            break;
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel;
    if(selectedLevelIdx >=this->m_aquariumlevels.size()) {
        selectedLevelIdx = this->m_aquariumlevels.size() - 1;
    }
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        if(this->currentLevel < (int)this->m_aquariumlevels.size() - 1) {
            this->currentLevel += 1;
        } // Suppose to spawn boss level
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (npc && checkCollision(player, npc)) {
            return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
        }
    }
    return nullptr;
};
// functin so the npc as a minor reverse direction when collide
void NPCreature::reverseDirection() {
    m_dx = -m_dx;
    m_dy = -m_dy;
}

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    this->m_player->update();

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
            // Handle PowerUp collision
            auto powerUp = std::dynamic_pointer_cast<PowerUpSpeed>(event->creatureB);
            if (powerUp) {
                ofLogNotice() << "Player collected a PowerUpSpeed! Temporary speed boost activated." << std::endl;
                // Temporary speed boost
                this->m_player->changeSpeed(this->m_player->getSpeed() + 2);
                this->m_player->m_speedBoostTimer = 300; // the speed would last 5 seconds
                // Permanent power boost that makes the player stronger
                this->m_player->increasePower(1);
                this->m_aquarium->removeCreature(event->creatureB);
                return;
            }
            if(event->creatureB != nullptr){
                event->print();
                auto npc = std::dynamic_pointer_cast<NPCreature>(event->creatureB);
                if(npc) { // make npc bounce back
                    npc->reverseDirection();
                }
                // Player also bounces away
                this->m_player->setDirection(-this->m_player->getDx(), -this->m_player->getDy());
                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                }
                else{
                    this->m_aquarium->removeCreature(event->creatureB);
                    this->m_player->addToScore(1, event->creatureB->getValue());
                    if (this->m_player->getScore() % 25 == 0){
                        this->m_player->increasePower(1);
                        ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                    }
                    
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }
        }

        // Spawning the boss reliably on the last level
        int lastLevelIndex = m_aquarium->getAquariumLevels().size() - 1; // index of the last level
        // Spawn boss only if we are exactly on the last level and boss hasn't been spawned yet
        if (!m_isBossSpawned && m_aquarium->getCurrentLevelI() == lastLevelIndex) {
            m_aquarium->SpawnCreature(AquariumCreatureType::BossFish, m_player);
            m_isBossSpawned = true;
        }

        //Updating all creatures including the new boss fish for its implementation 
        bool playerDiedByBoss = false;
        for (int i = 0; i < m_aquarium->getCreatureCount(); ++i) {
            auto creature = m_aquarium->getCreatureAt(i);
            if (!creature) continue; // skip null entries
            // Try casting to BossFish
            auto boss = std::dynamic_pointer_cast<BossFish>(creature);
            if (boss) {
                // Ensure the boss has a player pointer
                if (!boss->GetPlayer() && this->m_player) {
                    boss->SetPlayer(this->m_player);
                }
                // Use delta time
                float dt = ofGetLastFrameTime();
                if (dt <= 0) dt = 1.0f / 60.0f; // fallback to 60 FPS
                // Update the boss (movement + attacks)
                bool playerDied = false;
                boss->update(dt, playerDied);
                if (playerDied) {
                    playerDiedByBoss = true;
                }
            }
            else {
                // Regular NPCs move normally
                creature->move();
            }
        }
        // If player died due to boss or boss attack, trigger game over
        if (playerDiedByBoss) {
            this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
            return;
        }
        //removes the boss when level is completed
        for(int i = m_aquarium->getCreatureCount() - 1; i >= 0; --i) {
            auto creature = m_aquarium->getCreatureAt(i);
            auto boss = std::dynamic_pointer_cast<BossFish>(creature);
            if (boss && boss->IsRemoved()) { // boss is dead
                ofLogNotice() << "Removing dead boss from aquarium";
                m_aquarium->removeCreature(std::static_pointer_cast<Creature>(boss));
                m_isBossSpawned = false;
            }
        }
        this->m_aquarium->update(false);
    }

}

void AquariumGameScene::Draw() {
    //current level background 
    if(!m_aquarium->getAquariumLevels().empty()) {
        int i = m_aquarium->getCurrentLevelI() % m_aquarium->getAquariumLevels().size();
        auto currentLevel = m_aquarium->getAquariumLevels()[i];

        if(currentLevel && currentLevel->getBackGSprite()) {
            currentLevel->getBackGSprite()->draw(0, 0);
        }
    }
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

std::vector<AquariumCreatureType> AquariumLevel::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}
