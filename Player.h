//
// Created by andri on 29/11/2021.
//

#ifndef TENNISCHUMPSPRACTICEGAME_PLAYER_H
#define TENNISCHUMPSPRACTICEGAME_PLAYER_H

#include "structsAndEnums.h"
class Player {
private:
    float frameDuration = .06f, speed = 2.f, vecLength;
    int player, frame = 0, soundIndex = 0;
    bool flipSprite = false;
    sf::Sound sfx;
    std::vector<sf::SoundBuffer *> racketSounds;
    sf::Clock frameClock, soundClock;
    sf::Vector2f playerVec = sf::Vector2f(0.f, 0.f), swingOrigin, racketSize, spriteOrigin, position;
    sf::Sprite sprite;
    sf::RectangleShape racket;
    sf::Keyboard::Key up, right, down, left, swingKey;
    sf::Keyboard::Key keyList[5];
    State state = idle;
    bool *active_;
    Turn turn;

public:
    bool locked = true;

    Player(Turn _turn, sf::Texture *texture, std::map<std::string, sf::Keyboard::Key> keyDict, bool *active,
           std::vector<sf::SoundBuffer *> *_racketSounds) {
        // movement will be 0->3 with 0 being up and going in a clockwise motion.
        // swing will be 4!
        std::vector<int>::size_type sz = _racketSounds->size();
        for (int i = 0; i < sz; i++) {
            racketSounds.push_back(_racketSounds->at(i));
        }
        srand(std::time(0));
        active_ = active;
        playerVec = sf::Vector2f(0.f, 0.f);
        up = keyDict["up"];
        right = keyDict["right"];
        down = keyDict["down"];
        left = keyDict["left"];
        swingKey = keyDict["swing"];
        sf::Keyboard::Key keyList[5] = {up, right, down, left, swingKey};
        turn = _turn;
        sprite.setTexture(*texture);
        if (_turn == p1) {
            player = 0;
            spriteOrigin = sf::Vector2f(16, 16);
            racketSize = sf::Vector2f(26, 15);
            swingOrigin = sf::Vector2f(13, 0);
        } else {
            player = 1;
            spriteOrigin = sf::Vector2f(13, 18);
            racketSize = sf::Vector2f(27, 28);
            swingOrigin = sf::Vector2f(13, 18);
        }
        sprite.setOrigin(spriteOrigin);
        racket = sf::RectangleShape(racketSize);
        racket.setOrigin(swingOrigin);
        racket.setPosition(sprite.getPosition());
        racket.setFillColor(sf::Color(255, 0, 0, 100)); // To visualise the hitbox!
        racket.setOutlineColor(sf::Color(255, 0, 0, 255));

        sprite.setTextureRect(sf::IntRect(0, (player * 32), 32, 32));
    }

    void normalise(sf::Vector2f *vec) {
        if (vec->x != 0.f && vec->y != 0.f) {
            vecLength = sqrt(pow(vec->x, 2.f) + pow(vec->y, 2.f));
            vec->x /= vecLength;
            vec->y /= vecLength;
        }
    }

    void update(sf::Vector2f ballPos) {
        if (*active_) {
            if (pressed(swingKey) && state != swing) {
//                soundIndex = rand()

                state = swing;
                frame = 0;
                if (locked) { locked = false; }
                switch (turn) {
                    case p1:
                        if (ballPos.x > racket.getPosition().x)
                            flipSprite = true;
                        break;

                    default:
                        if (ballPos.x < racket.getPosition().x)
                            flipSprite = true;
                        break;
                }
                if (flipSprite)
                    sprite.scale(-1, 1);

                frameClock.restart();
            }
            if (!locked) {
                movement();
            }
            animate();
        }
    }


    void movement() {
        position = sprite.getPosition();
        playerVec = sf::Vector2f(0.f, 0.f);

        if (state != swing) {

            if (pressed(left))
                playerVec += sf::Vector2f(-1.f, 0.f);

            if (pressed(right))
                playerVec += sf::Vector2f(1.f, 0.f);

            if (pressed(up))
                playerVec += sf::Vector2f(0.f, -1.f);

            if (pressed(down))
                playerVec += sf::Vector2f(0.f, 1.f);

            // Deciding state

            if (playerVec == sf::Vector2f(0.f, 0.f))
                state = idle;

            if (playerVec.y != 0)
                state = runVert;

            if (playerVec.x < 0)
                state = runLeft;

            if (playerVec.x > 0)
                state = runRight;
        }

        // Normalise the speed
        normalise(&playerVec);
        playerVec *= speed;

        //Keep player within bounds!
        switch (turn) {
            case p2:
                if (position.x + playerVec.x < 15.f)
                    playerVec.x = (position.x - 15);

                if (position.x + playerVec.x > 231.f)
                    playerVec.x = 231.f - position.x;

                if (position.y + playerVec.y < 122.f)
                    playerVec.y = position.y - 122.f;

                if (position.y > 240.f)
                    playerVec.y = 240.f - position.y;
                break;

            default:
                if (position.x + playerVec.x < 15.f)
                    playerVec.x = (position.x - 15);

                if (position.x + playerVec.x > 231.f)
                    playerVec.x = 231.f - position.x;

                if (position.y + playerVec.y < 20.f)
                    playerVec.y = position.y - 20.f;

                if (position.y > 108.f)
                    playerVec.y = 108.f - position.y;
                break;
        }
        sprite.move(playerVec);
        racket.move(playerVec);
    }

    void animate() {
        if (frameClock.getElapsedTime().asSeconds() > frameDuration) {
            ++frame;
            frameClock.restart();
        }
        if (frame == 1 && state == swing) {
            sfx.setBuffer(*racketSounds[rand() % racketSounds.size()]);
            sfx.play();
        }
        if (frame > 3 && state == swing) {
            if (flipSprite) {
                flipSprite = false;
                sprite.scale(-1, 1);
            }
            state = idle;
        }
        frame %= 4;
        switch (state) {
            case swing:
                sprite.setTextureRect(sf::IntRect((frame * 32), 128 + (32 * player), 32, 32));
                break;

            case runLeft:
                sprite.setTextureRect(sf::IntRect((frame * 32), 64, 32, 32));
                break;

            case runRight:
                sprite.setTextureRect(sf::IntRect((frame * 32), 96, 32, 32));
                break;

            case runVert:
                sprite.setTextureRect(sf::IntRect((frame * 32), (32 * player), 32, 32));
                break;

            case idle:
                sprite.setTextureRect(sf::IntRect(0, (32 * player), 32, 32));
                break;
        }
    }

    void setPosition(sf::Vector2f pos) {
        sprite.setPosition(pos);
        racket.setPosition(pos);
    }

//    Turn getTurn() { return turn; }

    sf::RectangleShape getRacket() { return racket; }

    State getState() { return state; }

    sf::Sprite *_sprite() { return &sprite; }

    bool isAnyPlayerKeyPressed() {
        for (int k = 0; k < 5; ++k) {
            if (sf::Keyboard::isKeyPressed(keyList[k]))
                return true;
        }
        return false;
    }

    // made this because i HATE writing out sf::Keyboard::isKeyPressed(key[n]) every. single. DAMN. TIME.
    bool pressed(sf::Keyboard::Key key) { return sf::Keyboard::isKeyPressed(key); }


    void reset() {
        state = State::idle;
        frame = 0;
    }
};

#endif //TENNISCHUMPSPRACTICEGAME_PLAYER_H
