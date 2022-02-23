//
// Created by Иван Ильин on 23.02.2022.
//

#ifndef MINECRAFT_MINECRAFTMSGTYPE_H
#define MINECRAFT_MINECRAFTMSGTYPE_H

#include <SFML/Network.hpp>

enum class MinecraftMsgType {
    AddCube,
    RemoveCube,
};

sf::Packet &operator<<(sf::Packet &packet, MinecraftMsgType type);

sf::Packet &operator>>(sf::Packet &packet, MinecraftMsgType &type);



#endif //MINECRAFT_MINECRAFTMSGTYPE_H
