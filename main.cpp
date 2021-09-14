#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

enum State {
    idle,
    runLeft,
    runRight,
    runVert,
    swing
};

enum Turn {
    p1,
    p2
};
struct Scores {
    int score;
    int games;
    int sets;
};

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

class Player {
private:
    float frameDuration = .06f, speed = 2.f, vecLength;
    int player, frame = 0, friction = .2f;
    bool flipSprite = false;
    sf::Clock clock;
    sf::Vector2f playerVec = sf::Vector2f(0.f, 0.f), swingOrigin, racketSize, spriteOrigin, position;
    sf::Sprite sprite;
    sf::RectangleShape racket;
    sf::Keyboard::Key up, right, down, left, swingKey;
    sf::Keyboard::Key keyList[5];
    State state = idle;
    Turn turn;

public:
    Player(Turn _turn, sf::Texture *texture, std::map<std::string, sf::Keyboard::Key> keyDict) {
        // movement will be 0->3 with 0 being up and going in a clockwise motion.
        // swing will be 4!
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

    void update(sf::Vector2f ballPos) {
        if (pressed(swingKey) && state != swing) {
            state = swing;
            frame = 0;

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

            clock.restart();
        }
        movement();
        animate();
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

            if (!isAnyKeyPressed())
                state = idle;

            if (playerVec.y != 0)
                state = runVert;

            if (playerVec.x < 0)
                state = runLeft;

            if (playerVec.x > 0)
                state = runRight;
        }

        // Normalise the speed
        if (playerVec.x != 0.f && playerVec.y != 0.f) {
            vecLength = sqrt(pow(playerVec.x, 2.f) + pow(playerVec.y, 2.f));
            playerVec.x /= vecLength;
            playerVec.y /= vecLength;
        }
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
        if (clock.getElapsedTime().asSeconds() > frameDuration) {
            ++frame;
            clock.restart();
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

    Turn getTurn() { return turn; }

    sf::RectangleShape getRacket() { return racket; }

    State getState() { return state; }

    sf::Sprite *_sprite() { return &sprite; }

    bool isAnyKeyPressed() {
        for (int k = 0; k < 5; ++k) {
            if (sf::Keyboard::isKeyPressed(keyList[k]))
                return true;
        }
        return false;
    }

    // made this because i HATE writing out sf::Keyboard::isKeyPressed(key[n]) every. single. DAMN. TIME.
    bool pressed(sf::Keyboard::Key key) { return sf::Keyboard::isKeyPressed(key); }
};

class Ball : public IBall {
private:
    bool ballIn;
    sf::Sprite sprite, cross;
    sf::Clock clock;
    sf::Vector2f momentum, start, end, anchor, distanceVec;
    Player *player1, *player2;
    sf::RectangleShape activeRacket;
    int rally, crossFrame = 0;
    float speed = .8f, rotate, height, factor = 1, distance = 0.f;
    Turn currentTurn, server = p1;
    State activePlayerState;
    IUmpire *umpire_;
    sf::ConvexShape court;

public:
    int bounces = 0;
    bool active = true;

    Ball(sf::Texture *texture, Player *_player1, Player *_player2, Turn _turn, sf::Texture *crossTexture) {
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
        // std::cout << fx / gx << std::endl;
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
                start = sprite.getPosition();
                momentum = start - anchor;
//            momentum.x *= -1;
                factor = 1;
                bounces = 0;
                speed = .8f;
                if (momentum.x != 0.f or momentum.y != 0.f) {
                    float length = sqrt(pow(momentum.x, 2) + pow(momentum.y, 2));
                    momentum.x /= length;
                    momentum.y /= length;
                }
                end.x = start.x + momentum.x * 40.f;
                end.y = net_y + momentum.y * 30.f;
                distanceVec = end - start;
                cross.setPosition(end);

                distance = sqrt(pow(distanceVec.x, 2) + pow(distanceVec.y, 2));

                crossFrame = 0;
                clock.restart();
                currentTurn = (currentTurn == p1 ? p2 : p1);
            }

            // Animate landing
            if (clock.getElapsedTime().asSeconds() > 0.2) {
                ++crossFrame;
                crossFrame %= 12;
                cross.setTextureRect(sf::IntRect(0, (crossFrame * 10), 10, 10));
            }
            sf::Vector2f travelled = sprite.getPosition() - start;
            height = (momentum.x != 0 or momentum.y != 0) ? ballHeight(travelled) : 0;

            if (height < 0 or std::isnan(height))   // will reset after 1st bounce.
            {
                ballIn = sprite.getGlobalBounds().intersects(court.getGlobalBounds());
                ++bounces;
                if (bounces >= 2) { Notify(false); }
                else if (!ballIn) { Notify(true); }
                start = sprite.getPosition();
                momentum *= .95f;
                speed *= .9f;
                factor *= .4;
                end = start + (distanceVec * .5f);
                distanceVec = end - start;
                cross.setPosition(end);
                height = ballHeight(sprite.getPosition() - start);
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
        player1->setPosition(sf::Vector2f(70.0f, 50.0f));
        player2->setPosition(sf::Vector2f(155.0f, 170.0f));
        if (game) { changeServe(); }
        if (server == p2) {
            this->setPosition(player2->getRacket().getPosition());
        } else if (server == p1) {
            this->setPosition(player1->getRacket().getPosition());
        }
        momentum = sf::Vector2f(0.f, 0.f);
        start = sf::Vector2f(0.f, 0.f);
        end = sf::Vector2f(0.f, 0.f);
        distanceVec = sf::Vector2f(0.f, 0.f);
        currentTurn = server;
        cross.setPosition(-10, -10);
    }

    sf::ConvexShape bounds() { return court; }

    void Attach(IUmpire *pUmpire) override {
        umpire_ = pUmpire;
    }

    void Notify(bool out) override {
        if (out) {
            switch (currentTurn) {
                case p1:
                    currentTurn = p2;
                    break;
                case p2:
                    currentTurn = p1;
                    break;
            }
        }
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
    bool match = false;
    Scores p1, p2;
    int diff = 0, gameDiff = 0;
    Turn gameWinner;

    Umpire(Ball &ball, bool *cutscene) : ball_(ball) {
        _cutscene = cutscene;
        this->ball_.Attach(this);
        p1.score = 3;
        p1.games = 5;
        p1.sets = 0;
        p2.score = 0;
        p2.games = 4;
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
                if (p1.score > p2.score) { p1.games += 1; }
                else { p2.games += 1; }
                game = true;
//                ball_.reset(true);
                p1.score = 0;
                p2.score = 0;
            } else {
                game = false;
//                ball_.reset(false);
            }
        } else {
//            ball_.reset(false);
            game = false;
        }
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
//                p1.games = 0;
//                p2.games = 0;
            }
        }
        if (!*_cutscene) { ball_.reset(game); }

    }
};

class UI {
private:
    sf::Sprite board, dash, set, match, p1Score, p2Score, p1Sets, p1Games, p2Sets, p2Games;
    sf::Clock nextFrame, flicker;
    float frame = 0;
    Umpire &umpire_;
    bool show = true;

public:
    UI(Umpire &umpire, sf::Texture *scoreTexture, sf::Texture *setsTexture,
       sf::Texture *gamesTexture, sf::Texture *boardTexture, bool &cutscene) : umpire_(umpire) {
        board.setTexture(*boardTexture);
        board.setPosition(1, 205);

        dash.setTexture(*scoreTexture);
        dash.setTextureRect(sf::IntRect(64, 85, 11, 6));
        dash.setPosition(115, 22);

        set.setTexture(*scoreTexture);
        set.setTextureRect(sf::IntRect(0, 96, 48, 16));
        set.setScale(0, 0);
        set.setOrigin(24, 8);
        set.setPosition(120, 120);

        p1Score.setTexture(*scoreTexture);

        p1Games.setTexture(*gamesTexture);
        p1Games.setTextureRect(sf::IntRect(0, 0, 62, 10));
        p1Games.setPosition(20, 217);

        p1Sets.setTexture(*setsTexture);
        p1Sets.setTextureRect(sf::IntRect(0, 0, 32, 10));
        p1Sets.setPosition(83, 217);

        p2Score.setTexture(*scoreTexture);

        p2Games.setTexture(*gamesTexture);
        p2Games.setTextureRect(sf::IntRect(0, 0, 62, 10));
        p2Games.setPosition(20, 228);

        p2Sets.setTexture(*setsTexture);
        p2Sets.setTextureRect(sf::IntRect(0, 0, 32, 10));
        p2Sets.setPosition(83, 228);
    }

    void update() {
        int score_p1 = umpire_.p1.score;
        int score_p2 = umpire_.p2.score;
        if ((score_p1 >= 3) && (score_p2 >= 3)) {
            if (score_p1 > score_p2) {
                p1Score.setTextureRect(sf::IntRect(0, 80, 50, 16));
                p1Score.setPosition(112, 16);
                p1Score.setOrigin(50, 0);
                p2Score.setTextureRect(sf::IntRect(0, 80, 0, 0));
            } else if (score_p1 < score_p2) {
                p2Score.setTextureRect(sf::IntRect(0, 80, 50, 16));
                p1Score.setTextureRect(sf::IntRect(0, 80, 0, 0));
            } else {
                p1Score.setOrigin(0, 0);
                p1Score.setTextureRect(sf::IntRect(0, 64, 79, 16));
                p1Score.setPosition(82, 16);
                p2Score.setTextureRect(sf::IntRect(0, 64, 0, 0));
            }
        } else {
            switch (score_p1) {
                case 0:
                    p1Score.setOrigin(63, 0);
                    p1Score.setTextureRect(sf::IntRect(0, 16 * score_p1, 63, 16));
                    break;
                default:
                    p1Score.setOrigin(31, 0);
                    p1Score.setTextureRect(sf::IntRect(0, 16 * score_p1, 32, 16));
                    break;
            }
            p1Score.setPosition(112, 16);
            switch (score_p2) {
                case 0:
                    p2Score.setTextureRect(sf::IntRect(0, 16 * score_p2, 63, 16));
                    break;
                default:
                    p2Score.setTextureRect(sf::IntRect(0, 16 * score_p2, 32, 16));
                    break;
            }
            p2Score.setPosition(127, 16);
        }

        int games_p1 = umpire_.p1.games;
        int games_p2 = umpire_.p2.games;
        int diffG = games_p2 - games_p1;
        if ((games_p1 > 4) and (games_p2 > 4)) {
            switch (diffG) {
                case -2:
                    p1Games.setTextureRect(sf::IntRect(0, 60, 62, 10));
                    p2Games.setTextureRect(sf::IntRect(0, 40, 62, 10));
//                    TODO - ADD END SET ANIMATION TO BE RUN.
//                      - AND ADD TIMER TO SLOWLY FLASH WHITE ON SCREEN
//                    umpire_.p1.games = 0;
//                    umpire_.p2.games = 0;
                    umpire_.p1.sets += 1;
                    break;
                case -1:
                    p1Games.setTextureRect(sf::IntRect(0, 50, 62, 10));
                    p2Games.setTextureRect(sf::IntRect(0, 40, 62, 10));
                    break;
                case 0:
                    p1Games.setTextureRect(sf::IntRect(0, 50, 62, 10));
                    p2Games.setTextureRect(sf::IntRect(0, 50, 62, 10));
                    break;
                case 1:
                    p1Games.setTextureRect(sf::IntRect(0, 40, 62, 10));
                    p2Games.setTextureRect(sf::IntRect(0, 50, 62, 10));
                    break;
                case 2:
                    p1Games.setTextureRect(sf::IntRect(0, 40, 62, 10));
                    p2Games.setTextureRect(sf::IntRect(0, 60, 62, 10));
//                    TODO - ADD END SET ANIMATION TO BE RUN.
//                      - AND ADD TIMER TO SLOWLY FLASH WHITE ON SCREEN
//                    umpire_.p1.games = 0;
//                    umpire_.p2.games = 0;
                    umpire_.p2.sets += 1;
                    break;
            }
        } else {
            p1Games.setTextureRect(sf::IntRect(0, games_p1 * 10, 62, 10));
            p2Games.setTextureRect(sf::IntRect(0, games_p2 * 10, 62, 10));
        }

        p1Sets.setTextureRect(sf::IntRect(0, 10 * umpire_.p1.sets, 32, 10));
        p2Sets.setTextureRect(sf::IntRect(0, 10 * umpire_.p2.sets, 32, 10));


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
            std::cout << "THE DIFFERENCE IS " << diffG << std::endl << "P1: " << games_p1 << "   P2: " << games_p2;
        }
    }

    std::vector<sf::Sprite> getElements() {
        std::vector<sf::Sprite> elements;
        elements.push_back(p1Score);
        elements.push_back(p2Score);
        elements.push_back(board);
        elements.push_back(p1Games);
        elements.push_back(p2Games);
        elements.push_back(p1Sets);
        elements.push_back(p2Sets);
        elements.push_back(set);
        elements.push_back(dash);
        return elements;
    }

    static float easeInOut(float x) {
        return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
    }

    static float easeOutBounce(float x) {
        const float n1 = 7.5625;
        const float d1 = 2.75;
//        x/= 40;

        if (x < 1 / d1) {
            return n1 * x * x;
        } else if (x < 2 / d1) {
            return n1 * (x -= 1.5 / d1) * x + 0.75;
        } else if (x < 2.5 / d1) {
            return n1 * (x -= 2.25 / d1) * x + 0.9375;
        } else {
            return n1 * (x -= 2.625 / d1) * x + 0.984375;
        }
    }

    void cutScene(bool match, bool *__cutscene__) {
        float progress = frame / 150;
        if (nextFrame.getElapsedTime().asMilliseconds() > 3) {
            if (frame < 150)
                frame += 1;

            set.setScale(easeOutBounce(progress) * 2, easeOutBounce(progress) * 2);
            nextFrame.restart();
        }
        if (flicker.getElapsedTime().asSeconds() > .2) {
            show = !show;
            if (show) {
                if (umpire_.gameWinner == Turn::p1) {
                    p1Games.setTextureRect(sf::IntRect(0, 60, 70, 10));
                } else {
                    p2Games.setTextureRect(sf::IntRect(0, 60, 70, 10));
                }
            } else {
                if (umpire_.gameWinner == Turn::p1) {
                    p1Games.setTextureRect(sf::IntRect(0, 0, 70, 10));
                } else {
                    p2Games.setTextureRect(sf::IntRect(0, 0, 70, 10));
                }
            }

            flicker.restart();
        }
        if (progress == 1) {
            sf::sleep(sf::seconds(1));
            *__cutscene__ = false;
            set.setScale(0, 0);
            umpire_.p1.games = 0;
            umpire_.p2.games = 0;
        }
    }

};

sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight) {

    // Compares the aspect ratio of the window to the aspect ratio of the view,
    // and sets the view's viewport accordingly in order to archieve a letterbox effect.
    // A new view (with a new viewport set) is returned.

    float windowRatio = windowWidth / (float) windowHeight;
    float viewRatio = view.getSize().x / (float) view.getSize().y;
    float sizeX = 1;
    float sizeY = 1;
    float posX = 0;
    float posY = 0;

    bool horizontalSpacing = true;
    if (windowRatio < viewRatio)
        horizontalSpacing = false;

    // If horizontalSpacing is true, the black bars will appear on the left and right side.
    // Otherwise, the black bars will appear on the top and bottom.

    if (horizontalSpacing) {
        sizeX = viewRatio / windowRatio;
        posX = (1 - sizeX) / 2.f;
    } else {
        sizeY = windowRatio / viewRatio;
        posY = (1 - sizeY) / 2.f;
    }

    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));

    return view;
}

struct RenderItems {
    sf::RenderWindow *window;
    UI *ui;
    Player *p1;
    Player *p2;
    sf::Sprite *court;
    sf::Sprite *net;
    Ball *ball;
};

void renderingThread(RenderItems tempy) {
//    std::vector<sf::Sprite> uiElements = ui->getElements();
//    iterator
    sf::RenderWindow *_window = tempy.window;
    UI *_ui = tempy.ui;
    Player *_p1 = tempy.p1;
    Player *_p2 = tempy.p2;
    sf::Sprite *_net = tempy.net;
    sf::Sprite *_court = tempy.court;
    Ball *_ball = tempy.ball;
    while (_window->isOpen()) {

        _window->clear();
        _window->draw(*_court);
        _window->draw(*_p1->_sprite());
        _window->draw(*_net);
        _window->draw(*_p2->_sprite());
        _window->draw(*_ball->_sprite());

        for (auto element: _ui->getElements()) {
            _window->draw(element);
        }
        _window->display();

    }
}

bool cutscene = false;

int main() {
    sf::Texture courtTexture, ballTexture, playerTexture, netTexture, crossTexture, scoreTexture, setsTexture, gamesTexture, boardTexture;
    sf::Sprite courtSprite, netSprite, endText;
    sf::Clock clock, clock2;
    sf::Vector2f ballMomentum(0.f, 0.f);

    std::map<std::string, sf::Keyboard::Key> p1Keys;
    p1Keys["up"] = sf::Keyboard::W;
    p1Keys["down"] = sf::Keyboard::S;
    p1Keys["left"] = sf::Keyboard::A;
    p1Keys["right"] = sf::Keyboard::D;
    p1Keys["swing"] = sf::Keyboard::Space;

    std::map<std::string, sf::Keyboard::Key> p2Keys;
    p2Keys["up"] = sf::Keyboard::Up;
    p2Keys["down"] = sf::Keyboard::Down;
    p2Keys["left"] = sf::Keyboard::Left;
    p2Keys["right"] = sf::Keyboard::Right;
    p2Keys["swing"] = sf::Keyboard::Numpad0;

    Turn turn;
    if (!courtTexture.loadFromFile("src/assets/TCCourt.png") or
        !playerTexture.loadFromFile("src/assets/32x32_TCPlayers.png") or
        !ballTexture.loadFromFile("src/assets/TCBall.png") or
        !netTexture.loadFromFile("src/assets/TCNet.png") or
        !scoreTexture.loadFromFile("src/assets/TCScores.png") or
        !setsTexture.loadFromFile("src/assets/sets.png") or
        !gamesTexture.loadFromFile("src/assets/games.png") or
        !boardTexture.loadFromFile("src/assets/board.png") or
        !crossTexture.loadFromFile("src/assets/cross.png")
            ) {
        return -1;
    }
    endText.setTexture(scoreTexture);
    endText.setTextureRect(sf::IntRect(0, 112, 0, 0));

    courtTexture.setSmooth(false);
    playerTexture.setSmooth(false);
    netTexture.setSmooth(false);

    netSprite.setTexture(netTexture);
    netSprite.setPosition(sf::Vector2f(38.f, 108.f));

    courtSprite.setTexture(courtTexture);
    // Player 1 set up
    Player player1 = Player(p1, &playerTexture, p1Keys);
    player1.setPosition(sf::Vector2f(70.0f, 50.0f));

    // Player 2 set up
    Player player2 = Player(p2, &playerTexture, p2Keys);
    player2.setPosition(sf::Vector2f(155.0f, 170.0f));

    // Ball set up
    Ball baliBallerson(&ballTexture, &player1, &player2, Turn(p1), &crossTexture);
    baliBallerson.setPosition(sf::Vector2f(100, 100));

    // Umpire!!
    Umpire *umpire = new Umpire(*&baliBallerson, &cutscene);
    // Window and view Set up
    sf::RenderWindow window(sf::VideoMode(240, 240), "TennisChumpsSFML!");
    UI ui(*umpire, &scoreTexture, &setsTexture, &gamesTexture, &boardTexture, cutscene);


    sf::Vertex line[] = {
            sf::Vertex{sf::Vector2f(netSprite.getPosition().x - 130, netSprite.getPosition().y)},
            sf::Vertex{sf::Vector2f(netSprite.getPosition().x + 230, netSprite.getPosition().y)}
    };
    window.setFramerateLimit(60);
    turn = p1;
    float netY = netSprite.getPosition().y;

    sf::Event event;
    sf::View view;
    view.setSize(240, 240);
    view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
    view = getLetterboxView(view, 240, 240);

    RenderItems renderItems;
    renderItems.window = &window;
    renderItems.ui = &ui;
    renderItems.p1 = &player1;
    renderItems.p2 = &player2;
    renderItems.court = &courtSprite;
    renderItems.net = &netSprite;
    renderItems.ball = &baliBallerson;

    sf::Thread renderer(&renderingThread, renderItems);

//    renderer.launch(); // start the thread (internally calls task.run())
    while (window.isOpen()) {

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                view = getLetterboxView(view, event.size.width, event.size.height);
                window.setView(view);
            }
        }


        window.clear();
        if (cutscene) {
            ui.cutScene(umpire->match, &cutscene);
            if (!cutscene)
                baliBallerson.reset(umpire->game);
//            cutscene = false;
        } else {
            baliBallerson.update(netY);
            player1.update(baliBallerson.getPosition());
            player2.update(baliBallerson.getPosition());
            ui.update();
        }


        window.draw(courtSprite);
        window.draw(baliBallerson.bounds());
        window.draw(*baliBallerson._cross());
        window.draw(*player1._sprite());
        window.draw(netSprite);
        window.draw(*player2._sprite());
        window.draw(*baliBallerson._sprite());
        window.draw(line, 2, sf::Lines);

        for (auto element: ui.getElements()) {
            window.draw(element);
        }

        window.display();
//        renderingThread(renderItems);
    }

    return 0;
}
