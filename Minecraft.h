//
// Created by Иван Ильин on 03.10.2021.
//

#ifndef MINECRAFT_MINECRAFT_H
#define MINECRAFT_MINECRAFT_H

#include "engine/Engine.h"
#include "Player.h"
#include "PlayerController.h"
#include "engine/gui/Window.h"
#include "Client.h"
#include "Server.h"
#include "Map.h"

class Minecraft : public Engine {
private:
    std::shared_ptr<Player> player = std::make_shared<Player>(ObjectNameTag(MinecraftConsts::PLAYER_NAME));

    std::shared_ptr<PlayerController> playerController = std::make_shared<PlayerController>(player, keyboard, mouse);
    std::shared_ptr<Map> map = std::make_shared<Map>(world);

    Window mainMenu;

    std::shared_ptr<Server> server = std::make_shared<Server>();
    std::shared_ptr<Client> client = std::make_shared<Client>(player);

    bool inGame = false;

    void start() override;
    void update() override;

    void gui() override;

    void play();

    void InitNetwork();

    void spawnPlayer(sf::Uint16 id);
    void removePlayer(sf::Uint16 id);

    void addCube();
    Cube::Type removeCube();

    void updateCubeInHandColor();
public:
    Minecraft() : mainMenu(screen, mouse) {};
};


#endif //MINECRAFT_MINECRAFT_H
