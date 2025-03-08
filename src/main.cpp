#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <random>

using namespace geode::prelude;

static int maxFrames = 4;
auto chosenGameSprite = Mod::get()->getSettingValue<std::string>("selected-sprite");
auto disableInClassic = Mod::get()->getSettingValue<bool>("disable-inclassic");
auto isCompatDisabled = Mod::get()->getSettingValue<bool>("disable-compat");
auto dynamicToggle = Mod::get()->getSettingValue<bool>("dynamic-toggle");
auto dynamicFrames = Mod::get()->getSettingValue<bool>("dynamic-frames");
auto isModEnabled = Mod::get()->getSettingValue<bool>("enable-sonicmod");
auto enableSounds = Mod::get()->getSettingValue<bool>("enable-sfx");
auto globalSounds = Mod::get()->getSettingValue<bool>("global-sfx");
auto sonicBall = Mod::get()->getSettingValue<bool>("sonic-ball");
auto sonicCube = Mod::get()->getSettingValue<bool>("sonic-cube");
auto jumpInCube = Mod::get()->getSettingValue<bool>("jumpsfx-incube");
auto unleashedSounds = Mod::get()->getSettingValue<bool>("unleashed-sfx");
auto unleashedRankings = Mod::get()->getSettingValue<bool>("unleashed-rankings");
auto doIdleAnim = false;

// Individual SFX settings
auto selectedJumpSound = Mod::get()->getSettingValue<std::string>("jump-sfx");
auto selectedOrbSound = Mod::get()->getSettingValue<std::string>("orb-sfx");
auto selectedDashStartSound = Mod::get()->getSettingValue<std::string>("dash-start-sfx");
auto selectedDashStopSound = Mod::get()->getSettingValue<std::string>("dash-stop-sfx");
auto selectedPadSound = Mod::get()->getSettingValue<std::string>("pad-sfx");

$on_mod(Loaded) {
    listenForSettingChanges("selected-sprite", [](std::string value) {
        chosenGameSprite = value;
    });
    listenForSettingChanges("disable-compat", [](bool value) {
        isCompatDisabled = value;
    });
    listenForSettingChanges("enable-sonicmod", [](bool value) {
        isModEnabled = value;
    });
    listenForSettingChanges("enable-sfx", [](bool value) {
        enableSounds = value;
    });
    listenForSettingChanges("global-sfx", [](bool value) {
        globalSounds = value;
    });
    listenForSettingChanges("dynamic-toggle", [](bool value) {
        dynamicToggle = value;
    });
    listenForSettingChanges("dynamic-frames", [](bool value) {
        dynamicFrames = value;
    });
    // Individual SFX settings
    listenForSettingChanges("jump-sfx", [](std::string value) {
        selectedJumpSound = value;
    });
    listenForSettingChanges("orb-sfx", [](std::string value) {
        selectedOrbSound = value;
    });
    listenForSettingChanges("dash-start-sfx", [](std::string value) {
        selectedDashStartSound = value;
    });
    listenForSettingChanges("dash-stop-sfx", [](std::string value) {
        selectedDashStopSound = value;
    });
    listenForSettingChanges("pad-sfx", [](std::string value) {
        selectedPadSound = value;
    });
    listenForSettingChanges("disable-inclassic", [](bool value) {
        disableInClassic = value;
    });
    // extra settings
    listenForSettingChanges("sonic-ball", [](bool value) {
        sonicBall = value;
    });
    listenForSettingChanges("sonic-cube", [](bool value) {
        sonicCube = value;
    });
    listenForSettingChanges("jumpsfx-incube", [](bool value) {
        jumpInCube = value;
    });
    listenForSettingChanges("unleashed-sfx", [](bool value) {
        unleashedSounds = value;
    });
    listenForSettingChanges("unleashed-rankings", [](bool value) {
        unleashedRankings = value;
    });
}

int genRandomInt(int min, int max) {
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator
    std::uniform_int_distribution<> distr(min, max); // Define the range

    return distr(gen);
}

class $modify(PlayerObject) {
    struct Fields {
        float m_animationTimer = 0.f;
        float m_bumpTimer = 0.f; 
        int m_maxFrames = 4; 
        int m_currentFrame = 1; 
        bool m_flippedX = false; 
        bool m_flippedY = false; 
        bool m_isUsingExtendedFrames = false;
        bool m_isShadow = false; // HE GETS SPECIAL TREATMENT BC HES SO FUCKING COOL
        CCSprite* m_customSprite = nullptr;
        //CCSprite* m_accuracySprite = nullptr;
        //int m_maxAccuracyFrames = 5;
        //int m_currentAccuracyFrame = 1;

        // stuff for idle animation thingy
        // its a separate sprite bc im not sure if i can do it with the main sprite lol so i just made a new one for it lol xd lmao haha funny haha xd lmao <- this is a joke btw dont take it seriously <- copilot told me to write this <- copilot
        CCSprite* m_idleAnim = nullptr;
        float m_idleAnimTimer = 0.f;
        int m_maxIdleFrames = 4;
        int m_currentIdleFrame = 1;
    };

    bool init(int p0, int p1, GJBaseGameLayer* p2, cocos2d::CCLayer* p3, bool p4) {
        if (!PlayerObject::init(p0, p1, p2, p3, p4)) return false;

        if (isModEnabled){

            auto fields = m_fields.self();

            // Change frames depending on what sprite u selected
            // Some sprites need to use 8 frames max
            // Some need only 4
            if (!dynamicFrames){
                if (chosenGameSprite == "mania" || chosenGameSprite == "advance2" || chosenGameSprite == "supermania" || chosenGameSprite == "sonic2hd" || chosenGameSprite == "sonic3maniafied" || chosenGameSprite == "sonic1maniafied" || chosenGameSprite == "classicshadowslide" || chosenGameSprite == "modernsonic" || chosenGameSprite == "maniaknuckles" || chosenGameSprite == "mighty" || chosenGameSprite == "ray" || chosenGameSprite == "metal") {
                    fields->m_maxFrames = 8;
                    fields->m_isUsingExtendedFrames = true;
                } else {
                    if (chosenGameSprite == "shadow") {
                        fields->m_maxFrames = 12;
                        fields->m_isUsingExtendedFrames = true;
                        fields->m_isShadow = true; // la creatura ... ha llegado
                    } else {
                        fields->m_maxFrames = 4;
                        fields->m_isUsingExtendedFrames = false;
                    }
                }
            }

            // give birth to sonic (real)
            std::string frameName = fmt::format("{}_sonicRun_01.png"_spr, chosenGameSprite);
            fields->m_customSprite = CCSprite::createWithSpriteFrameName(frameName.c_str());
            if (fields->m_customSprite) {
                fields->m_customSprite->setAnchorPoint({0.5f, 0.5f});
                fields->m_customSprite->setPosition(this->getPosition());
                fields->m_customSprite->setVisible(false);
                fields->m_customSprite->setID("sonic-anim"_spr);
                this->addChild(fields->m_customSprite, 10);
            }

            if (chosenGameSprite == "classic1" && fields->m_customSprite) {
                fields->m_idleAnim = CCSprite::createWithSpriteFrameName("classic1_sonicIdle_01.png"_spr);
            }

            // Hide robot
            m_robotBatchNode->setVisible(false);

        }

        return true;
    }

    void createRobot(int p0) {
        PlayerObject::createRobot(p0);

        if (isModEnabled && !isCompatDisabled){
            m_robotBatchNode->setVisible(false);
            m_robotSprite->setVisible(false);
            
        }
    }

    void update(float p0) {
        PlayerObject::update(p0);

        auto fields = m_fields.self();
        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        if (dynamicToggle){

            if (isModEnabled && m_robotBatchNode){
                m_robotBatchNode->setVisible(false);
            }

            if(!fields->m_customSprite){

                std::string frameName = fmt::format("{}_sonicRun_01.png"_spr, chosenGameSprite);
                fields->m_customSprite = CCSprite::createWithSpriteFrameName(frameName.c_str());
                fields->m_customSprite->setAnchorPoint({0.5f, 0.5f});
                fields->m_customSprite->setPosition({0, 0});
                fields->m_customSprite->setVisible(false);
                fields->m_customSprite->setID("sonic-anim"_spr);
                this->addChild(fields->m_customSprite, 10);
                
            }

            if (!isModEnabled && fields->m_customSprite){
                    m_robotBatchNode->setVisible(true);
                    m_robotSprite->setVisible(true);
                    m_robotFire->setVisible(true);
                    m_robotBurstParticles->setVisible(true);
                    fields->m_customSprite->setVisible(false);
                    
                }
        }

        if(isModEnabled){

            if (dynamicFrames) {
                if (chosenGameSprite == "mania" || chosenGameSprite == "advance2" || chosenGameSprite == "supermania" || chosenGameSprite == "sonic2hd" || chosenGameSprite == "sonic3maniafied" || chosenGameSprite == "sonic1maniafied" || chosenGameSprite == "classicshadowslide" || chosenGameSprite == "modernsonic" || chosenGameSprite == "maniaknuckles" || chosenGameSprite == "mighty" || chosenGameSprite == "ray" || chosenGameSprite == "metal") {
                    fields->m_maxFrames = 8;
                    fields->m_isUsingExtendedFrames = true;
                } else {
                    if (chosenGameSprite == "shadow") {
                        fields->m_maxFrames = 12;
                        fields->m_isUsingExtendedFrames = true;
                        fields->m_isShadow = true; // la creatura ... ha llegado
                    } else {
                        fields->m_maxFrames = 4;
                        fields->m_isUsingExtendedFrames = false;
                    }
                }
            }

            // Sync rotation
            if (fields->m_customSprite && m_mainLayer) {
                fields->m_customSprite->setRotation(m_mainLayer->getRotation());

                // y flip
                bool mainLayerFlippedY = m_mainLayer->getScaleY() < 0;
                fields->m_customSprite->setFlipY(mainLayerFlippedY);
            }

            // Sonic Ball and Cube feature
            if (sonicBall || (sonicCube && m_isCube)) {
                if (m_isBall || m_isCube) {
                    m_mainLayer->setVisible(false);
                    fields->m_customSprite->setVisible(true);
                    if (m_isCube && sonicCube) {
                        this->setRotation(0);
                    }

                    if (m_isBall && sonicBall) {
                        this->setRotation(0);
                    }
                    
                } else {
                    m_mainLayer->setVisible(true);
                    fields->m_customSprite->setVisible(false);
                }
            }

            // Check if ur a robot or ball
            if ((!m_isRobot && !(m_isBall && sonicBall) && !(m_isCube && sonicCube)) || !fields->m_customSprite) {

                if (sonicBall && !m_isBall) {
                    m_mainLayer->setVisible(true);
                }

                if (sonicCube && !m_isCube) {
                    m_mainLayer->setVisible(true);
                }

                if (fields->m_customSprite) {
                    fields->m_customSprite->setVisible(false);
                }
                return;
            }

            fields->m_customSprite->setVisible(true);
            m_robotFire->setVisible(false);
            m_robotBurstParticles->setVisible(false);

            // bump anim for pads
            if (fields->m_bumpTimer > 0.f) {

                std::string frameName = fmt::format("{}_sonicBumped_01.png"_spr, chosenGameSprite);
                fields->m_customSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));
                fields->m_bumpTimer -= 0.2f;
                fields->m_animationTimer = 0; 
                return;
            }

            // idle anim for platformer
            if (m_isPlatformer && m_platformerXVelocity == 0 && m_isOnGround) {

                std::string frameName = fmt::format("{}_sonicIdle_01.png"_spr, chosenGameSprite);
                fields->m_customSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));
                fields->m_animationTimer = 0;
            } else {
                // set current animation (run or jump)
                std::string frameName;
                float frameDuration;

                if (m_isBall && sonicBall) {
                    frameName = fmt::format("{}_sonicJump_0{}.png"_spr, chosenGameSprite, fields->m_currentFrame);
                    // change frame times if using extended (8) frames
                    if (fields->m_isUsingExtendedFrames){
                        frameDuration = 0.7f;
                    } else {
                        frameDuration = 1.4f;
                    }
                } else if (this->m_isOnGround || this->m_hasGroundParticles) {
                    frameName = fmt::format("{}_sonicRun_0{}.png"_spr, chosenGameSprite, fields->m_currentFrame);
                    // change frame times if using extended (8) frames
                    if (fields->m_isUsingExtendedFrames){
                        if (fields->m_isShadow){
                            frameDuration = 2.2f;
                        } else {
                            frameDuration = 1.9f;
                        }
                    } else {
                        frameDuration = 2.8f;
                    }
                } else {
                    frameName = fmt::format("{}_sonicJump_0{}.png"_spr, chosenGameSprite, fields->m_currentFrame);
                    // change frame times if using extended (8) frames
                    if (fields->m_isUsingExtendedFrames){
                        frameDuration = 0.7f;
                    } else {
                        frameDuration = 1.3f;
                    }
                }

                // update animation frame
                fields->m_animationTimer += p0;

                if (fields->m_animationTimer >= frameDuration && fields->m_bumpTimer <= 0.1f) {
                    fields->m_animationTimer -= frameDuration;

                    // update current frame (cycle through 1-4/8)
                    fields->m_currentFrame = (fields->m_currentFrame % fields->m_maxFrames) + 1;
                    auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str());

                    // Only update if valid
                    if (frame) {
                        fields->m_customSprite->setDisplayFrame(frame);
                    }
                }
            }

            // if i need to add anything else
            // do it here
            // this is INSIDE the if mod is enabled check

        }

        // this is outside the check

    }

    void doReversePlayer(bool p0) {
        PlayerObject::doReversePlayer(p0);

        if (isModEnabled){
            auto fields = m_fields.self();

            if (fields->m_customSprite) {
                if (p0 != fields->m_flippedX) {
                    fields->m_flippedX = p0;
                    fields->m_customSprite->setFlipX(p0); 
                }
            } else {
                geode::log::warn("Sonic sprite not found! Please make sure to restart the level after switching the mod's soft-toggle or use Allow Dynamic Soft-toggle update.");
            }

        }
    }

    void bumpPlayer(float p0, int p1, bool p2, GameObject* p3) {
        PlayerObject::bumpPlayer(p0, p1, p2, p3);

        if (isModEnabled) {
            auto fields = m_fields.self();
            auto fmod = FMODAudioEngine::sharedEngine();
            auto sfxToPlay = fmt::format("{}.ogg"_spr, selectedPadSound);
            bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;
            bool validCube = sonicCube && m_isCube;
            bool doBump = m_isRobot || validCube;

            if (disableInClassic && !m_isPlatformer) {
                sfxToPlay = "none.ogg"_spr;
            }

            if (doBump && fields->m_customSprite) {
                fields->m_bumpTimer = 12.5f; 
            }

            if (enableSounds) {
                if (!globalSounds){
                    if (m_isRobot){
                        fmod->playEffect(sfxToPlay);
                    }
                } else {
                    fmod->playEffect(sfxToPlay);
                }
            }

        }
        
    }

    void playerDestroyed(bool p0) {
        PlayerObject::playerDestroyed(p0);

        auto fields = m_fields.self();
        auto frameName = fmt::format("{}_sonicDeath_01.png"_spr, chosenGameSprite);
        auto deathAnim = CCEaseBackIn::create( CCMoveBy::create(0.75f, {0, -200}) );
        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        bool sonicBallDie = m_isBall && sonicBall;
        bool normalDie = isModEnabled && m_isRobot;
        bool sonicCubeDie = m_isCube && sonicCube;
        bool doDieAnim = sonicBallDie || normalDie || sonicCubeDie;

        if (doDieAnim) {
            fields->m_customSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(frameName.c_str()));
            fields->m_customSprite->runAction(deathAnim);
            if (!doDieAnim) {
                fields->m_customSprite->setVisible(false);
            }
        }

        if (unleashedSounds) {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->playEffect("death_1.ogg"_spr);
        }

    }

    void startDashing(DashRingObject* p0){
        PlayerObject::startDashing(p0);

        auto fmod = FMODAudioEngine::sharedEngine();
        auto sfxToPlayDashStart = fmt::format("{}.ogg"_spr, selectedDashStartSound);

        if (disableInClassic && !m_isPlatformer) {
            sfxToPlayDashStart = "none.ogg"_spr;
        }

        if (isModEnabled && enableSounds){
            if (!globalSounds){
                if (m_isRobot){
                    fmod->playEffect(sfxToPlayDashStart);
                }
            } else {
                fmod->playEffect(sfxToPlayDashStart);
            }
        }

        // Check for unleashedSounds and play a random boost sound
        if (unleashedSounds) {
            int randomBoost = genRandomInt(1, 7);
            auto sfxToPlayBoost = fmt::format("boost_{}.ogg"_spr, randomBoost);

            // Generate another random int to decide whether to play the sound
            int playSound = genRandomInt(1, 10);

            if (playSound > 7) {
                if (!globalSounds) {
                    if (m_isRobot) {
                        fmod->playEffect(sfxToPlayBoost);
                    }
                } else {
                    fmod->playEffect(sfxToPlayBoost);
                }
            }
        }
    }

    void flipGravity(bool p0, bool p1) {
        PlayerObject::flipGravity(p0, p1);

        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        if (sonicCube && m_isCube) {
            float scaleX = m_vehicleSize;
            float scaleY = m_vehicleSize;

            if (m_isUpsideDown && m_isSideways) {
                // Right (rotated 90deg)
                this->setScaleX(scaleX);
                this->setScaleY(scaleY);
            } else if (!m_isUpsideDown && m_isSideways) {
                // Left (rotated -90deg)
                this->setScaleX(-scaleX);
                this->setScaleY(scaleY);
            } else if (m_isUpsideDown && !m_isSideways) {
                // Upside down
                this->setScaleX(scaleX);
                this->setScaleY(-scaleY);
            } else {
                // Normal
                this->setScaleX(scaleX);
                this->setScaleY(scaleY);
            }
        }
    }

    void togglePlayerScale(bool p0, bool p1) {
        PlayerObject::togglePlayerScale(p0, p1);

        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        if (sonicCube && m_isCube) {
            float scaleX = m_vehicleSize;
            float scaleY = m_vehicleSize;

            if (m_isUpsideDown && m_isSideways) {
                // Right (rotated 90deg)
                this->setScaleX(scaleX);
                this->setScaleY(scaleY);
            } else if (!m_isUpsideDown && m_isSideways) {
                // Left (rotated -90deg)
                this->setScaleX(-scaleX);
                this->setScaleY(scaleY);
            } else if (m_isUpsideDown && !m_isSideways) {
                // Upside down
                this->setScaleX(scaleX);
                this->setScaleY(-scaleY);
            } else {
                // Normal
                this->setScaleX(scaleX);
                this->setScaleY(scaleY);
            }
        }
    }

    void stopDashing(){
        PlayerObject::stopDashing();

        if (isModEnabled){
            auto fields = m_fields.self();

            fields->m_customSprite->stopAllActions();
            fields->m_customSprite->setPosition({0,0});

            auto fmod = FMODAudioEngine::sharedEngine();
            auto sfxToPlayDashStop = fmt::format("{}.ogg"_spr, selectedDashStopSound);

            if (disableInClassic && !m_isPlatformer) {
                sfxToPlayDashStop = "none.ogg"_spr;
            }

            if (enableSounds){
                if (!globalSounds){
                    if (m_isRobot){
                        fmod->playEffect(sfxToPlayDashStop);
                    }
                } else {
                    fmod->playEffect(sfxToPlayDashStop);
                }
            }
        }
    }

    void incrementJumps(){
        PlayerObject::incrementJumps();

        auto fmod = FMODAudioEngine::sharedEngine();
        auto sfxToPlayJump = fmt::format("{}.ogg"_spr, selectedJumpSound);
        auto sfxToPlayOrb = fmt::format("{}.ogg"_spr, selectedOrbSound);
        bool m_isCube = !m_isShip && !m_isBird && !m_isBall && !m_isDart && !m_isRobot && !m_isSpider && !m_isSwing;

        if (disableInClassic && !m_isPlatformer) {
            sfxToPlayJump = "none.ogg"_spr;
            sfxToPlayOrb = "none.ogg"_spr;
        }

        if (isModEnabled && enableSounds && PlayLayer::get()){
            if (!m_ringJumpRelated){
                if (m_isRobot){
                    fmod->playEffect(sfxToPlayJump);
                }
                // Sonic Cube jump SFX
                if (sonicCube && m_isCube && jumpInCube) {
                    fmod->playEffect(sfxToPlayJump);
                }

                // Check for unleashedSounds and play a random jump sound
                if (unleashedSounds) {
                    int randomJump = genRandomInt(1, 5);
                    auto sfxToPlayRandomJump = fmt::format("jump_{}.ogg"_spr, randomJump);

                    // Generate another random int to decide whether to play the sound
                    int playSound = genRandomInt(1, 10);

                    if (playSound > 5) {
                        if (m_isRobot || (sonicCube && m_isCube && jumpInCube)) {
                            fmod->playEffect(sfxToPlayRandomJump);
                        }
                    }
                }
            } else {
                if (m_ringJumpRelated){
                    if (m_isRobot){
                        fmod->playEffect(sfxToPlayOrb);
                    } else {
                        fmod->playEffect(sfxToPlayOrb);
                    }
                }
            }
        }
    }

    void setVisible(bool visible) {
        PlayerObject::setVisible(visible);

        if (isModEnabled){
            auto fields = m_fields.self();

            if (fields->m_customSprite) {
                fields->m_customSprite->setVisible(visible);
            }
        }

    }

    void updateTimeMod(float p0, bool p1) {
        PlayerObject::updateTimeMod(p0, p1);

        if (unleashedSounds && PlayLayer::get()) {
            auto fmod = FMODAudioEngine::sharedEngine();
            int randomBoost = genRandomInt(1, 7);
            auto sfxToPlayBoost = fmt::format("boost_{}.ogg"_spr, randomBoost);

            int doPlaySound = genRandomInt(1, 10);

            if (p0 >= 1.2f) {
                fmod->playEffect("modern-fullboost.ogg"_spr);
                if (doPlaySound >= 3) {
                    fmod->playEffect(sfxToPlayBoost);
                }
            }
        }
    }
};

class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (unleashedSounds) {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->playEffect("herewe.ogg"_spr);
        }
        
        return true;
    }

    void startGame() {
        PlayLayer::startGame();

        if (unleashedSounds) {
            auto fmod = FMODAudioEngine::sharedEngine();
            fmod->playEffect("go.ogg"_spr);
        }
    }

    void levelComplete() {
        PlayLayer::levelComplete();
        auto fmod = FMODAudioEngine::sharedEngine();

        if (unleashedSounds){
            fmod->playEffect("goal_ring.ogg"_spr);
        }

        if(unleashedRankings){

            int attempts = this->m_attempts;

            int randomBest = genRandomInt(1, 6);
            int randomGood = genRandomInt(1, 5);
            int randomBad = genRandomInt(1, 5);

            if (attempts <= 3) {
                fmod->playEffect(fmt::format("complete_best_{}.ogg"_spr, randomBest));
            } else if (attempts > 3 && attempts <= 10) {
                fmod->playEffect(fmt::format("complete_good_{}.ogg"_spr, randomGood));
            } else {
                fmod->playEffect(fmt::format("complete_bad_{}.ogg"_spr, randomBad));
            }

        }
    }
};

class $modify(EndLevelLayer) {
    void customSetup() {
        EndLevelLayer::customSetup();

        if (unleashedRankings) {
            int attempts = m_playLayer->m_attempts;
            auto fmod = FMODAudioEngine::sharedEngine();

            auto rankSprite = CCSprite::createWithSpriteFrameName("rank_placeholder.png"_spr);
            rankSprite->setOpacity(0);
            rankSprite->setZOrder(10);
            rankSprite->setPosition({50, 50});
            rankSprite->setID("rank-sprite"_spr);
            rankSprite->setScale(9.0f);
            this->addChild(rankSprite);

            geode::log::debug("attempts: {}", attempts);

            bool rankS = attempts == 1;
            bool rankA = attempts == 2 || attempts == 3;
            bool rankB = attempts >= 4 && attempts <= 6;
            bool rankC = attempts >= 7 && attempts <= 10;
            bool rankD = attempts >= 11 && attempts <= 15;
            bool rankE = attempts > 16;

            if (rankS) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_s.png"_spr));
                fmod->playEffect("rankS.ogg"_spr);
            } else if (rankA) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_a.png"_spr));
                fmod->playEffect("rankA.ogg"_spr);
            } else if (rankB) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_b.png"_spr));
                fmod->playEffect("rankC.ogg"_spr);
            } else if (rankC) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_c.png"_spr));
                fmod->playEffect("rankC.ogg"_spr);
            } else if (rankD) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_d.png"_spr));
                fmod->playEffect("rankC.ogg"_spr);
            } else if (rankE) {
                rankSprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("rank_e.png"_spr));
                fmod->playEffect("rankE.ogg"_spr);
            }
        }
        
    }

    void onHideLayer(CCObject* sender) {
        EndLevelLayer::onHideLayer(sender);
        geode::log::debug("when escondes el endlevel layer");
    }

    void showLayer(bool p0) {
        EndLevelLayer::showLayer(p0);

        if (unleashedRankings) {
            auto ranking = this->getChildByID("rank-sprite"_spr);

            // animations
            auto fadeIn = CCFadeIn::create(0.3f);
            auto scaleDown = CCScaleTo::create(0.3, 1.75f);

            ranking->runAction(fadeIn);
            ranking->runAction(scaleDown);
        }
    }

};