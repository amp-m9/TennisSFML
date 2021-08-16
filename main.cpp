#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

float frameDuration = 0.06f;
float speed = 2.f;
enum State
{
    idle,
    runLeft,
    runRight,
    runVert,
    swing
};

enum Turn
{
    p1,
    p2
};

bool isAnyKeyPressed1()
{
    sf::Keyboard::Key keys[]{sf::Keyboard::Up, sf::Keyboard::Down,
                             sf::Keyboard::Left, sf::Keyboard::Right,
                             sf::Keyboard::Numpad0};

    for (int k = 0; k < 5; ++k)
    {
        if (sf::Keyboard::isKeyPressed(keys[k]))
        {
            return true;
        }
    }
    return false;
}

bool isAnyKeyPressed2()
{
    sf::Keyboard::Key keys[]{sf::Keyboard::W, sf::Keyboard::A,
                             sf::Keyboard::S, sf::Keyboard::D,
                             sf::Keyboard::Space};

    for (int k = 0; k < 5; ++k)
    {
        if (sf::Keyboard::isKeyPressed(keys[k]))
        {
            return true;
        }
    }
    return false;
}

void p2Movement(sf::Sprite *sprite, State *state, sf::Clock *clock,
                int *frame, sf::RectangleShape *racket, sf::Vector2f ballPos, bool *flipped)
{
    sf::Vector2f position = sprite->getPosition();
    sf::Vector2f move = sf::Vector2f(0.f, 0.f);
    int row = 1;
    float length;

    // Setting state and vector based on action
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && *state != swing)
    {
        sprite->setRotation(0.f);
        *state = swing;
        *frame = 0;
        sprite->setTextureRect(sf::IntRect(0, 160, 32, 32));
        clock->restart();
        if (ballPos.x < racket->getPosition().x)
        {
            sprite->scale(-1, 1);
            *flipped = true;
        }
    }

    if (*state == swing)
    {
        if (clock->getElapsedTime().asSeconds() > frameDuration)
        {
            *frame += 1;

            if (*frame > 3)
            {
                *frame = 0;
                *state = idle;
                if (*flipped)
                {
                    sprite->scale(-1, 1);
                    *flipped = false;
                }
            }
            else
            {
                sprite->setTextureRect(sf::IntRect((*frame * 32), 160, 32, 32));
                clock->restart();
            }
        }
    }
    else
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            move += sf::Vector2f(-1.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            move += sf::Vector2f(1.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            move += sf::Vector2f(0.f, -1.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            move += sf::Vector2f(0.f, 1.f);

        // Setting idle state and animation.
        if (!isAnyKeyPressed2() && *state != swing)
        {
            *frame = 0;
            *state = idle;
            row = 1;
        }

        if (move.y != 0)
        {
            *state = runVert;
            row = 1;
        }

        if (move.x < 0)
        {
            *state = runRight;
            row = 2;
        }

        if (move.x > 0)
        {
            *state = runLeft;
            row = 3;
        }

        // Animation
        if (clock->getElapsedTime().asSeconds() > frameDuration)
        {
            *frame %= 4;
            if (*state = idle)
                *frame = 0;
            sprite->setTextureRect(sf::IntRect((*frame * 32), (row * 32), 32, 32));
            *frame += 1;
            clock->restart();
        }
    }
    // Movement

    // Normalise the speed
    if (move.x != 0.f && move.y != 0.f)
    {
        length = sqrt(pow(move.x, 2.f) + pow(move.y, 2.f));
        move.x /= length;
        move.y /= length;
    }
    move *= speed;
    if (position.x + move.x < 15.f)
        move.x = (position.x - 15);

    if (position.x + move.x > 231.f)
        move.x = 231.f - position.x;

    if (position.y + move.y < 115.f)
        move.y = position.y - 115.f;

    if (position.y > 240.f)
        move.y = 240.f - position.y;

    racket->move(move);
    sprite->move(move);
}

void p1Movement(sf::Sprite *sprite, State *state, sf::Clock *clock,
                int *frame, sf::RectangleShape *racket, sf::Vector2f ballPos, bool *flipped)
{
    sf::Vector2f position = sprite->getPosition();
    sf::Vector2f move = sf::Vector2f(0.f, 0.f);
    int row = 0;
    float length;

    // Setting state and vector based on action
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad0) && *state != swing)
    {
        sprite->setRotation(0.f);
        *state = swing;
        *frame = 0;
        sprite->setTextureRect(sf::IntRect(0, 128, 32, 32));
        clock->restart();
        if (ballPos.x > racket->getPosition().x)
        {
            sprite->scale(-1, 1);
            *flipped = true;
        }
    }

    if (*state == swing)
    {
        if (clock->getElapsedTime().asSeconds() > frameDuration)
        {
            *frame += 1;

            if (*frame > 3)
            {
                *frame = 0;
                *state = idle;
                if (*flipped)
                {
                    sprite->scale(-1, 1);
                    *flipped = false;
                }
            }
            else
            {
                sprite->setTextureRect(sf::IntRect((*frame * 32), 128, 32, 32));
                clock->restart();
            }
        }
    }
    else
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            move += sf::Vector2f(-1.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            move += sf::Vector2f(1.f, 0.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            move += sf::Vector2f(0.f, -1.f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            move += sf::Vector2f(0.f, 1.f);

        // Setting idle state and animation.
        if (!isAnyKeyPressed1() && *state != swing)
        {
            *frame = 0;
            *state = idle;
            row = 0;
        }

        if (move.y != 0)
        {
            *state = runVert;
            row = 0;
        }

        if (move.x < 0)
        {
            *state = runRight;
            row = 2;
        }

        if (move.x > 0)
        {
            *state = runLeft;
            row = 3;
        }

        // Animation
        if (clock->getElapsedTime().asSeconds() > frameDuration)
        {
            *frame %= 4;
            if (*state = idle)
                *frame = 0;
            sprite->setTextureRect(sf::IntRect((*frame * 32), (row * 32), 32, 32));
            *frame += 1;
            clock->restart();
        }
    }
    // Movement
    if (move.x != 0.f && move.y != 0.f)
    {
        length = sqrt(pow(move.x, 2.f) + pow(move.y, 2.f));
        move.x /= length;
        move.y /= length;
    }
    move *= speed;

    if (position.x + move.x < 15.f)
        move.x = (position.x - 15);

    if (position.x + move.x > 231.f)
        move.x = 231.f - position.x;

    if (position.y + move.y < 20.f)
        move.y = position.y - 20.f;

    if (position.y > 108.f)
        move.y = 108.f - position.y;
    racket->move(move);
    sprite->move(move);
}

void ballCalc(sf::Sprite *ball, State state, sf::RectangleShape *racket, sf::Vector2f *ballVec, Turn *turn, Turn player)
{
    sf::Vector2f BallVec = *ballVec;
    float rotationRate;
    float increase = .4f;
    float speed = .1f;
    ball->rotate(20.f);
    if (ball->getGlobalBounds().intersects(racket->getGlobalBounds()) && (state == swing) && *turn == player)
    {
        sf::Vector2f anchor = racket->getPosition();

        if (*turn == p2)
        {
            // BallVec.y += increase; // send back ball with more momentum
            *turn = p1; // add more mmomentum to the ball
            anchor.y += 15.f;
        }
        else if (*turn == p1)
        {
            *turn = p2;
            anchor.y -= 15.f;
        }

        BallVec = ball->getPosition() - anchor;
        BallVec.x *= -1;

        // Normalising the speed
        if (BallVec.x != 0.f or BallVec.y != 0.f)
        {
            float length = sqrt(pow(BallVec.x, 2.f) + pow(BallVec.y, 2.f));
            BallVec.x /= length;
            BallVec.y /= length;
        }

        *ballVec = BallVec;
    }
    ball->move(*ballVec * speed);
}

int main()
{
    sf::Texture courtTexture, ballTexture, playerTexture, netTexture;
    sf::Sprite courtSprite, ballSprite, p1Sprite, p2Sprite, netSprite;
    sf::Clock clock, clock2;
    sf::RectangleShape ball(sf::Vector2f(8, 8)), p1FrontHand(sf::Vector2f(26, 15)), p2FrontHand(sf::Vector2f(27, 28));
    sf::Vector2f ballMomentum(0.f, 0.f);
    sf::Vertex line[2];
    State p1State, p2State;
    Turn turn, p1Turn = p1, p2Turn = p2;
    int p1Frames = 0;
    int p2Frames = 0;
    bool p1Flipped = false;
    bool p2Flipped = true;
    if (!courtTexture.loadFromFile("src/assets/TCCourt.png") or
        !playerTexture.loadFromFile("src/assets/32x32_TCPlayers.png") or
        !ballTexture.loadFromFile("src/assets/TCBall.png") or
        !netTexture.loadFromFile("src/assets/TCNet.png"))
    {
        return -1;
    }
    else
    {
        courtTexture.setSmooth(false);
        playerTexture.setSmooth(false);
        netTexture.setSmooth(false);

        netSprite.setTexture(netTexture);
        netSprite.setPosition(sf::Vector2f(38.f, 108.f));

        courtSprite.setTexture(courtTexture);

        // Player 1 set up
        p1Sprite.setTexture(playerTexture);
        p1Sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
        p1Sprite.setPosition(sf::Vector2f(100.0f, 100.0f));
        p1Sprite.setOrigin(sf::Vector2f(16.f, 16.f));

        p1FrontHand.setOrigin(sf::Vector2f(13, 0));
        p1FrontHand.setPosition(p1Sprite.getPosition());
        p1FrontHand.setFillColor(sf::Color(255, 0, 0, 100));

        p1State = idle;

        // Player 2 set up
        p2Sprite.setTexture(playerTexture);
        p2Sprite.setTextureRect(sf::IntRect(0, 32, 32, 32));
        p2Sprite.setPosition(sf::Vector2f(200.0f, 200.0f));
        p2Sprite.setOrigin(sf::Vector2f(16.f, 16.f));

        p2FrontHand.setOrigin(sf::Vector2f(13, 18));
        p2FrontHand.setPosition(p2Sprite.getPosition() + sf::Vector2f(0.f, 4.f));
        p2FrontHand.setFillColor(sf::Color(255, 0, 0, 100));

        p2State = idle;

        // Ball Set-up
        ballSprite.setTexture(ballTexture);
        ballSprite.setOrigin(4, 4);

        ball.setOutlineThickness(0.2f);
        ball.setOutlineColor(sf::Color(250, 150, 100));
        ball.setOrigin(4, 4);

        ballSprite.setPosition(120, 120);
    }

    sf::RenderWindow window(sf::VideoMode(240, 240), "SFML works!");
    window.setFramerateLimit(60);
    turn = p1;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        // Movement of players and ball
        p1Movement(&p1Sprite, &p1State, &clock, &p1Frames, &p1FrontHand, ballSprite.getPosition(), &p1Flipped);
        p2Movement(&p2Sprite, &p2State, &clock2, &p2Frames, &p2FrontHand, ballSprite.getPosition(), &p2Flipped);
        ballCalc(&ballSprite, p1State, &p1FrontHand, &ballMomentum, &turn, p1Turn);
        ballCalc(&ballSprite, p2State, &p2FrontHand, &ballMomentum, &turn, p2Turn);
        // Lines to show shot directionals
        // sf::Vertex linep1[2] = {
        //     sf::Vertex(p1FrontHand.getPosition(), sf::Color(255, 0, 255, 100)),
        //     sf::Vertex((p1FrontHand.getPosition() + sf::Vector2f(0.f, 50.f)), sf::Color(255, 0, 255, 100))};

        sf::Vertex linep1_1[2] = {
            sf::Vertex(p1FrontHand.getPosition() - sf::Vector2f(5, 0), sf::Color::Cyan),
            sf::Vertex((p1FrontHand.getPosition() + sf::Vector2f(-5.f, 50.f)), sf::Color::Cyan)};

        sf::Vertex linep1_2[2] = {
            sf::Vertex(p1FrontHand.getPosition() + sf::Vector2f(5, 0), sf::Color::Yellow),
            sf::Vertex((p1FrontHand.getPosition() + sf::Vector2f(5.f, 50.f)), sf::Color::Yellow)};

        sf::Vertex linep2[2] = {
            sf::Vertex(p2FrontHand.getPosition(), sf::Color(0, 0, 255, 100)),
            sf::Vertex((p2FrontHand.getPosition() - sf::Vector2f(0.f, 50.f)), sf::Color(0, 0, 255, 100))};

        // Displaying graphics
        window.draw(courtSprite);

        // Draw P1
        window.draw(p1Sprite);
        window.draw(p1FrontHand);
        // window.draw(linep1, 2, sf::Lines);
        window.draw(linep1_1, 2, sf::Lines);
        window.draw(linep1_2, 2, sf::Lines);

        // Draw P2
        window.draw(p2Sprite);
        window.draw(p2FrontHand);
        window.draw(linep2, 2, sf::Lines);

        window.draw(netSprite);
        window.draw(ballSprite);
        window.display();
    }

    return 0;
}
