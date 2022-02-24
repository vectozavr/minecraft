//
// Created by Иван Ильин on 06.02.2021.
//

#include "Minecraft.h"

using namespace std;


int main() {
    Minecraft game;

    // Optimal for standard monitors:
    //game.create(1280, 720);
    //game.create(1920, 1080, MinecraftConsts::PROJECT_NAME, true, Consts::BACKGROUND_COLOR, sf::Style::Fullscreen);

    // Optimal for MacBook Pro 16 display:
    game.create(2048, 1152);
    //game.create(3840, 2160);

    return 0;
}