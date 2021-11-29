//
// Created by andri on 29/11/2021.
//

#ifndef TENNISCHUMPSPRACTICEGAME_UMPIREANDBALL_H
#define TENNISCHUMPSPRACTICEGAME_UMPIREANDBALL_H

#include "structsAndEnums.h"
#include "Player.h"

class IUmpire {
public:
    virtual ~IUmpire() = default;;

    virtual void Update(Turn loser) = 0;
};

class IBall {
public:
    virtual ~IBall() {};

    virtual void Attach(IUmpire *umpire) = 0;

//    virtual void Detach(IUmpire *umpire) = 0;
    virtual void Notify(bool out) = 0;
};


class Ball : public IBall {
private:
    bool ballIn, serve = true;
    sf::Sound bounceSfx, hitSfx;
    sf::Sprite sprite, cross;
    sf::Clock clock;
    sf::Vector2f momentum, start, end, anchor, distanceVec;
    Player *player1, *player2;
    sf::RectangleShape activeRacket;
    int rally, crossFrame = 0;
    float rotate, height, factor = 1, distance = 0.f, speed = 2.f;
    Turn currentTurn, server = p1;
    State activePlayerState;
    IUmpire *umpire_;
    sf::ConvexShape court;
    std::vector<sf::SoundBuffer *> bounceSounds;
    std::vector<sf::SoundBuffer *> hitSounds;

public:
    int bounces = 0;
    bool active = true;

    Ball(sf::Texture *texture, Player *_player1, Player *_player2, Turn _turn, sf::Texture *crossTexture,
         std::vector<sf::SoundBuffer *> *_bounceSounds, std::vector<sf::SoundBuffer *> *_hitSounds) {
        int sz =_bounceSounds->size();
        for(int i = 0; i<sz;i++){
            bounceSounds.push_back(_bounceSounds->at(i));
        }

        sz =_hitSounds->size();
        for(int i = 0; i<sz;i++){
            hitSounds.push_back(_hitSounds->at(i));
        }
        srand(55);
        sprite.setTexture(*texture);
        sprite.setOrigin(4, 4);
        cross.setTexture(*crossTexture);
        cross.setTextureRect(sf::IntRect(0, 0, 10, 10));
        cross.setOrigin(5, 5);
        cross.setPosition(-10, -10);
        server = _turn;
        currentTurn = server;
        momentum = sf::Vector2f(0.f, 0.f);
        start = sf::Vector2f(0.f, 0.f);
        end = sf::Vector2f(0.f, 0.f);
        distanceVec = sf::Vector2f(0.f, 0.f);
        player1 = _player1;
        player2 = _player2;
        court.setPointCount(4);
        sf::Vector2f points[] = {
                sf::Vector2f(68, 64),
                sf::Vector2f(171, 64),
                sf::Vector2f(178, 192),
                sf::Vector2f(61, 192)
        };
        for (int i = 0; i < 4; ++i) {
            court.setPoint(i, points[i]);
        }

        court.setFillColor(sf::Color(100, 50, 255, 100));
        court.setOutlineColor(sf::Color(255, 0, 0, 255));
    }

    void setPosition(sf::Vector2f vec) {
        sprite.setPosition(vec);
        start = vec;
        end = vec;
        momentum = sf::Vector2f(0, 0);
    }

    void setTurn(Turn _turn) { currentTurn = _turn; }

    sf::Sprite *_sprite() { return &sprite; }

    sf::Sprite *_cross() { return &cross; }

    sf::Vector2f getPosition() { return sprite.getPosition(); }

    float ballHeight(sf::Vector2f vecTravelled) {
        float d = distance;
        float x = sqrt(pow(vecTravelled.x, 2) + pow(vecTravelled.y, 2)); // distance travelled
        float m = (d / 2);                                               // midpoint
        float fx = x * (d - x) / m;                                // f(x) = x(d-x) / (d/2)
        float gx = (m * (d - m)) / m;                              // g(x) = f(x=m)
        return fx / gx;
    }

    void update(float net_y) {
        if (active) {
            switch (currentTurn) {
                case p1:
                    activeRacket = player1->getRacket();
                    anchor = activeRacket.getPosition();
                    anchor.y -= 15.f;
                    activePlayerState = player1->getState();
                    break;

                case p2:
                    activeRacket = player2->getRacket();
                    anchor = activeRacket.getPosition();
                    anchor.y += 15.f;
                    activePlayerState = player2->getState();
                    break;
            }
            if (sprite.getGlobalBounds().intersects(activeRacket.getGlobalBounds()) &&
                activePlayerState == State(swing)) {
                hitSfx.setBuffer(*hitSounds[rand()%hitSounds.size()]);
                hitSfx.play();
                rally++;
                start = sprite.getPosition();
                if (serve) {
                    if (currentTurn == p1)
                        momentum = player2->getRacket().getPosition() - start;
                    else
                        momentum = player1->getRacket().getPosition() - start;
                    serve = false;
                } else
                    momentum = start - anchor;
                factor = 1;
                bounces = 0;
                if (momentum.x != 0.f or momentum.y != 0.f) {
                    float length = sqrt(pow(momentum.x, 2) + pow(momentum.y, 2));
                    momentum.x /= length;
                    momentum.y /= length;
                }
                end = start;

//                end.x = start.x + momentum.x * 40.f;
                if (currentTurn == p1) {
                    while (end.y < net_y + 20.f) { end += momentum; }
                } else {
                    while (end.y > net_y - 20.f) { end += momentum; }
                }
//                end.y = net_y + momentum.y * 30.f;
                distanceVec = end - start;
                cross.setPosition(end);

                distance = sqrt(pow(distanceVec.x, 2) + pow(distanceVec.y, 2));

                crossFrame = 0;
                clock.restart();
                currentTurn = (currentTurn == p1 ? p2 : p1);
//                sf::sleep(sf::Time(sf::milliseconds(200)));
            }

            // Animate landing
            if (clock.getElapsedTime().asSeconds() > 0.2) {
                if (crossFrame < 11) { crossFrame++; }
                cross.setTextureRect(sf::IntRect(0, (crossFrame * 10), 10, 10));
            }
            sf::Vector2f travelled = sprite.getPosition() - start;
            height = (momentum.x != 0 or momentum.y != 0) ? ballHeight(travelled) : 0;

            if (height < 0 or std::isnan(height))   // will reset after 1st bounce.
            {
                ballIn = sprite.getGlobalBounds().intersects(court.getGlobalBounds());
                ++bounces;
                if (serve) { serve = false; }
                if (bounces >= 2) { Notify(false); }
                else if (!ballIn) { Notify(true); }
                momentum *= .8f;
                factor = .6;
                start = sprite.getPosition();
                end = start + (distanceVec * .7f);
                distanceVec = end - start;
                cross.setPosition(end);
                distance = sqrt(pow(distanceVec.x, 2) + pow(distanceVec.y, 2));
                bounceSfx.setBuffer(*bounceSounds[rand()%bounceSounds.size()]);
                bounceSfx.play();
//                height = ballHeight(sprite.getPosition() - start);
            }
            height *= factor;
            sprite.setScale(.9f + height, .9f + height);
            sprite.move(momentum * speed);

        }
    }

    void changeServe() {
        switch (server) {
            case p1:
                server = p2;
                break;
            case p2:
                server = p1;
                break;
        }
    }

    void reset(bool game) {
        rally = 0;
        player1->setPosition(sf::Vector2f(70.0f, 50.0f));
        player1->reset();
        player2->setPosition(sf::Vector2f(155.0f, 170.0f));
        player2->reset();
        if (game) { changeServe(); }
        if (server == p2) {
            player1->locked = false;
            player2->locked = true;
            this->setPosition(player2->getRacket().getPosition());
        } else if (server == p1) {
            player1->locked = true;
            player2->locked = false;
            this->setPosition(player1->getRacket().getPosition());
        }
        momentum = sf::Vector2f(0.f, 0.f);
        start = sf::Vector2f(0.f, 0.f);
        end = sf::Vector2f(0.f, 0.f);
        distanceVec = sf::Vector2f(0.f, 0.f);
        currentTurn = server;
        serve = true;
        cross.setPosition(-10, -10);
    }

    sf::ConvexShape bounds() { return court; }

    void Attach(IUmpire *pUmpire) override {
        umpire_ = pUmpire;
    }

    void Notify(bool out) override {
        umpire_->Update(currentTurn);
    }
};



class Umpire : public IUmpire {
private:
    Ball &ball_;
    sf::String message;
    bool *_cutscene;
public:
    bool game = false;
    Scene scene;
    Scores p1, p2;
    int diff = 0, gameDiff = 0;
    Turn gameWinner;

    Umpire(Ball &ball, bool *cutscene) : ball_(ball) {
        _cutscene = cutscene;
        this->ball_.Attach(this);
        p1.score = 0;
        p1.games = 0;
        p1.sets = 0;
        p2.score = 0;
        p2.games = 0;
        p2.sets = 0;
    }

    void Update(Turn loser) override {
        switch (loser) {
            case Turn::p1:
                p2.score += 1;
                break;
            case Turn::p2:
                p1.score += 1;
                break;
        }
        if (p1.score > p2.score) {
            diff = (p1.score - p2.score);
        } else if (p2.score > p1.score) {
            diff = (p2.score - p1.score);
        } else if (p1.score == p2.score) {
            diff = 0;
        }
        if ((p1.score > 3) or (p2.score > 3)) {
            if (diff >= 2) {
                scene = Scene::game;
                if (p1.score > p2.score) { p1.games += 1; }
                else { p2.games += 1; }
                game = true;
                p1.score = 0;
                p2.score = 0;
            } else { game = false; }

        } else { game = false; }
        // Check if set!

        gameDiff = (p2.games - p1.games);

        if ((p1.games > 5) or (p2.games > 5)) {
            if (gameDiff >= 2 or gameDiff <= -2) {
                if (p1.games > p2.games) {
                    p1.sets += 1;
                    gameWinner = Turn::p1;
                } else {
                    p2.sets += 1;
                    gameWinner = Turn::p2;
                }
                *_cutscene = true;
            }
            scene = Scene::set;
        }

        if (!*_cutscene) { ball_.reset(game); }

    }

    void reset() {
        p1.score = 0;
        p1.games = 0;
        p1.sets = 0;
        p2.score = 0;
        p2.games = 0;
        p2.sets = 0;
    }
};


#endif //TENNISCHUMPSPRACTICEGAME_UMPIREANDBALL_H
