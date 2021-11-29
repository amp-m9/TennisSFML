//
// Created by andri on 29/11/2021.
//

#ifndef TENNISCHUMPSPRACTICEGAME_UI_H
#define TENNISCHUMPSPRACTICEGAME_UI_H
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

        match.setTexture(*scoreTexture);
        match.setTextureRect(sf::IntRect(1, 112, 78, 16));
        match.setScale(0, 0);
        match.setOrigin(39, 8);
        match.setPosition(120, 120);

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
                    umpire_.p2.sets += 1;
                    break;
            }
        } else {
            p1Games.setTextureRect(sf::IntRect(0, games_p1 * 10, 62, 10));
            p2Games.setTextureRect(sf::IntRect(0, games_p2 * 10, 62, 10));
        }

        p1Sets.setTextureRect(sf::IntRect(0, 10 * umpire_.p1.sets, 32, 10));
        p2Sets.setTextureRect(sf::IntRect(0, 10 * umpire_.p2.sets, 32, 10));

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
        elements.push_back(match);
        elements.push_back(dash);
        return elements;
    }

    static float easeOutBounce(float x) {
        const float n1 = 7.5625;
        const float d1 = 2.75;

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

    void cutScene(Scene scene, bool *__cutscene__) {
        float progress = frame / 100;
        if (scene == Scene::set) {
            if (nextFrame.getElapsedTime().asSeconds() > 1 / 60) {
                if (frame < 150)
                    frame += 1;
                if (progress < 1) { set.setScale(easeOutBounce(progress) * 3, easeOutBounce(progress) * 3); }
                if (progress >= 1) {
                    set.setScale((progress) * 3, (1 - (progress - 1) * 2) * 3);
                }
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
            if (progress == 1.5) {
                if (umpire_.p1.sets == 2 or umpire_.p2.sets == 2) {
                    umpire_.scene = Scene::match;
                    frame = 0;
                } else {
                    sf::sleep(sf::seconds(1));
                    *__cutscene__ = false;
                }

                set.setScale(0, 0);
                umpire_.p1.games = 0;
                umpire_.p2.games = 0;
            }
        } else if (scene == Scene::game) {
        } else if (scene == Scene::match) {
            if (nextFrame.getElapsedTime().asSeconds() > 1 / 60) {
                if (frame < 150)
                    frame += 1;

                match.setScale(easeOutBounce(progress) * 2, easeOutBounce(progress) * 2);
                nextFrame.restart();
            }
            if (flicker.getElapsedTime().asSeconds() > .2) {
                show = !show;
                if (show) {
                    if (umpire_.gameWinner == Turn::p1) {
                        p1Sets.setScale(1, 1);
                    } else {
                        p2Sets.setScale(1, 1);
                    }
                } else {
                    if (umpire_.gameWinner == Turn::p1) {
                        p1Sets.setScale(0, 0);
                    } else {
                        p2Sets.setScale(0, 0);
                    }
                }
                flicker.restart();
            }
            if (progress == 1) {
                sf::sleep(sf::seconds(1));
                *__cutscene__ = false;
                match.setScale(0, 0);
                p1Sets.setScale(1, 1);
                p2Sets.setScale(1, 1);
            }
        }
    }
};

#endif //TENNISCHUMPSPRACTICEGAME_UI_H
