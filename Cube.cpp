//
// Created by Иван Ильин on 15.03.2021.
//

#include "Cube.h"
#include <cmath>

sf::Color Cube::cubeColor(Cube::Type t) {
    switch (t) {
        case grass:
            return sf::Color(122, 255, 127);
        case sand:
            return sf::Color(255, 235, 153);
        case glass:
            return sf::Color(255, 255, 255, 50);
        case water:
            return sf::Color(120, 147, 255, 100);
        case stone:
            return sf::Color(148, 148, 148);
        case earth:
            return sf::Color(153, 124, 86);
        case wood:
            return sf::Color(117, 74, 39);
        case snow:
            return sf::Color(255, 255, 255);
        default:
            return sf::Color(255, 245, 194);
    }
}

std::string Cube::soundName(Cube::Type cube) {

    std::string name;
    switch (cube) {
        case Cube::Type::stone:
            name = "stone";
            break;
        case Cube::Type::snow:
            name = "stone";
            break;
        case Cube::Type::earth:
            name = "gravel";
            break;
        case Cube::Type::wood:
            name = "wood";
            break;
        case Cube::Type::grass:
            name = "grass";
            break;
        case Cube::Type::sand:
            name = "sand";
            break;
        case Cube::Type::water:
            name = "glass";
            break;
        case Cube::Type::glass:
            name = "glass";
            break;
        case Cube::Type::none:
            name = "stone";
    }

    int soundNum = round((double) rand() / RAND_MAX * 3) + 1; // random number from 1 to 4

    return "sound/" + name + std::to_string(soundNum) + ".ogg";
}
