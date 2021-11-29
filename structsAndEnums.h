//
// Created by andri on 29/11/2021.
//

#ifndef TENNISCHUMPSPRACTICEGAME_STRUCTSANDENUMS_H
#define TENNISCHUMPSPRACTICEGAME_STRUCTSANDENUMS_H
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum State {
    idle,
    runLeft,
    runRight,
    runVert,
    swing,
    swing2
};
enum Scene {
    game, set, match
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

// Function overloading, HOW FUN!!!!
// Load function defined for both texture and sound objects so that I can see which file in particular is not loading.
bool load(sf::Texture *texture, std::string file){
    if(!texture->loadFromFile(file)) {
        std::cout << "COULD NOT LOAD TEXTURE FROM: " << file << std::endl;
        return false;
    }
    return true;
}

bool load(sf::SoundBuffer *sound, std::string file){
    if(!sound->loadFromFile(file)){
        std::cout<<"COULD NOT LOAD SOUND FROM: "<<file<<std::endl;
        return false;
    }
    return true;
}
#endif //TENNISCHUMPSPRACTICEGAME_STRUCTSANDENUMS_H
