#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h>
#include <iostream>
#include <cmath>
#include "UmpireAndBall.h"
#include "Player.h"
#include "structsAndEnums.h"
#include "UI.h"







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


bool cutscene = false;
bool active = false;

int main() {
    sf::Texture courtTexture, ballTexture, playerTexture, netTexture, crossTexture, scoreTexture, setsTexture, gamesTexture, boardTexture, titleTexture, subTitleTexture;
    sf::Sprite courtSprite, netSprite, title, subTitle;
    sf::Clock clock, clock2;
    sf::Vector2f ballMomentum(0.f, 0.f);
    sf::SoundBuffer ballBounce1, ballBounce2, bounceOnRacket1, bounceOnRacket2, racketSwoosh1, racketSwoosh2, racketSwoosh3, racketSwoosh4, racketSwoosh5;

    // initialising hash maps of controls for player 1 and 2
    std::map<std::string, sf::Keyboard::Key> p1Keys;
    p1Keys["up"] = sf::Keyboard::W;
    p1Keys["down"] = sf::Keyboard::S;
    p1Keys["left"] = sf::Keyboard::A;
    p1Keys["right"] = sf::Keyboard::D;
    p1Keys["swing"] = sf::Keyboard::Space;
    p1Keys["swing1"] = sf::Keyboard::N;

    std::map<std::string, sf::Keyboard::Key> p2Keys;
    p2Keys["up"] = sf::Keyboard::Up;
    p2Keys["down"] = sf::Keyboard::Down;
    p2Keys["left"] = sf::Keyboard::Left;
    p2Keys["right"] = sf::Keyboard::Right;
    p2Keys["swing"] = sf::Keyboard::Period;
    p2Keys["swing1"] = sf::Keyboard::Numpad2;

    // Loading files
    if (!load(&courtTexture, "src/assets/TCCourt.png") or
        !load(&playerTexture ,"src/assets/32x32_TCPlayers.png") or
        !load(&ballTexture ,"src/assets/TCBall.png") or
        !load(&netTexture ,"src/assets/TCNet.png") or
        !load(&scoreTexture ,"src/assets/TCScores.png") or
        !load(&setsTexture ,"src/assets/sets.png") or
        !load(&gamesTexture ,"src/assets/games.png") or
        !load(&boardTexture ,"src/assets/board.png") or
        !load(&crossTexture ,"src/assets/cross.png") or
        !load(&titleTexture ,"src/assets/TCTitle.png") or
        !load(&subTitleTexture ,"src/assets/pressToStart.png") or
        !load(&ballBounce1 ,"src/oggs/bounce_hard_court_001.ogg") or
        !load(&ballBounce2 ,"src/oggs/bounce_hard_court_002.ogg") or
        !load(&bounceOnRacket1 ,"src/oggs/bounce_on_racket_001.ogg") or
        !load(&bounceOnRacket2 ,"src/oggs/bounce_on_racket_002.ogg") or
        !load(&racketSwoosh1 ,"src/oggs/racket_swoosh_001.ogg") or
        !load(&racketSwoosh2 ,"src/oggs/racket_swoosh_002.ogg") or
        !load(&racketSwoosh3 ,"src/oggs/racket_swoosh_003.ogg") or
        !load(&racketSwoosh4 ,"src/oggs/racket_swoosh_004.ogg") or
        !load(&racketSwoosh5 ,"src/oggs/racket_swoosh_006.ogg")
            ) {
        return -1;
    }
    // clock to make the text that reads 'PRESS ENTER TO START' flicker.
    sf::Clock subTitleFlicker;
    subTitle.setTexture(subTitleTexture);
    subTitle.setPosition(61, 129);

    // preparing vectors for each type of sound

    std::vector<sf::SoundBuffer *> bounceSounds;
    bounceSounds.push_back(&ballBounce1);
    bounceSounds.push_back(&ballBounce2);

    std::vector<sf::SoundBuffer *> hitSounds;
    hitSounds.push_back(&bounceOnRacket1);
    hitSounds.push_back(&bounceOnRacket2);

    std::vector<sf::SoundBuffer *> racketSounds;
    racketSounds.push_back(&racketSwoosh1);
    racketSounds.push_back(&racketSwoosh2);
    racketSounds.push_back(&racketSwoosh3);
    racketSounds.push_back(&racketSwoosh4);
    racketSounds.push_back(&racketSwoosh5);

    std::vector<std::vector<sf::SoundBuffer *> *> sounds;
    sounds.push_back(&bounceSounds);
    sounds.push_back(&hitSounds);
    sounds.push_back(&racketSounds);

    // Setting up title screen.
    title.setTexture(titleTexture);
    title.setPosition(25, 43);
    sf::RectangleShape mainBG(sf::Vector2f(240, 240));
    mainBG.setFillColor(sf::Color(255, 255, 255, 180));

    // disabling antialiasing for textures.
    courtTexture.setSmooth(false);
    playerTexture.setSmooth(false);
    netTexture.setSmooth(false);

    // positioning and creating net texture.
    netSprite.setTexture(netTexture);
    netSprite.setPosition(sf::Vector2f(38.f, 108.f));

    // court Sprite does not need to be repositioned.
    courtSprite.setTexture(courtTexture);

    // Player 1 set up
    Player player1 = Player(p1, &playerTexture, p1Keys, &active, &racketSounds);
    player1.setPosition(sf::Vector2f(70.0f, 50.0f));

    // Player 2 set up
    Player player2 = Player(p2, &playerTexture, p2Keys, &active, &racketSounds);
    player2.setPosition(sf::Vector2f(155.0f, 170.0f));

    // Ball set up
    Ball baliBallerson(&ballTexture, &player1, &player2, Turn(p1), &crossTexture, &bounceSounds, &hitSounds);
    baliBallerson.setPosition(sf::Vector2f(100, 100));

    // Umpire!!
    Umpire *umpire = new Umpire(*&baliBallerson, &cutscene);
    // Window and view Set up
    sf::RenderWindow window(sf::VideoMode(240, 240), "TennisChumpsSFML!");
    UI ui(*umpire, &scoreTexture, &setsTexture, &gamesTexture, &boardTexture, cutscene);


    window.setFramerateLimit(60);
    float netY = netSprite.getPosition().y;

    sf::Event event;
    sf::View view;
    view.setSize(240, 240);
    view.setCenter(view.getSize().x / 2, view.getSize().y / 2);
    view = getLetterboxView(view, 240, 240);
    baliBallerson.reset(false);


    // oncde outside the loop to make the score say 'love - love'
    ui.update();
    //---------------------------------------- Game Loop ----------------------------------------
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
        if (active) {
            if (cutscene) {
                ui.cutScene(umpire->scene, &cutscene);
                if (!cutscene) {
                    baliBallerson.reset(umpire->game);
                    if (umpire->scene == Scene::match) {
                        active = false;
                        umpire->reset();
                    }
                }
//            cutscene = false;
            } else {
                // update position of ball and then see if players are striking the ball
                baliBallerson.update(netY);
                player1.update(baliBallerson.getPosition());
                player2.update(baliBallerson.getPosition());
                ui.update();
            }

        }
        // Render code for players ui etc.
        window.draw(courtSprite);
        window.draw(baliBallerson.bounds());
        window.draw(*baliBallerson._cross());
        window.draw(*player1._sprite());
        window.draw(netSprite);
        window.draw(*player2._sprite());
        window.draw(*baliBallerson._sprite());
        // draw all ui elements last!
        for (auto element: ui.getElements()) {
            window.draw(element);
        }

        // Title screen rendered only if game is done &  on load (so when game is not active);
        if (!active) {
            window.draw(mainBG);
            window.draw(title);
            if (subTitleFlicker.getElapsedTime().asSeconds() > .5) {
                if (subTitle.getScale().x == 0) { subTitle.setScale(1, 1); }
                else { subTitle.setScale(0, 0); }
                subTitleFlicker.restart();
            }
            window.draw(subTitle);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) { active = true; }
        }

        window.display();
//        renderingThread(renderItems);
    }

    return 0;
}
