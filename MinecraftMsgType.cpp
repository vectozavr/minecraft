//
// Created by Иван Ильин on 23.02.2022.
//

#include "MinecraftMsgType.h"

sf::Packet &operator<<(sf::Packet &packet, MinecraftMsgType type) {
    return packet << (sf::Uint16) type;
}

sf::Packet &operator>>(sf::Packet &packet, MinecraftMsgType &type) {
    sf::Uint16 temp;
    packet >> temp;
    type = (MinecraftMsgType) temp;
    return packet;
}