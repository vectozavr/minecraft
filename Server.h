//
// Created by Иван Ильин on 25.05.2021.
//

#ifndef MINECRAFT_3DZAVR_SERVER_H
#define MINECRAFT_3DZAVR_SERVER_H

#include "engine/network/ServerUDP.h"
#include "Player.h"

class Server : public ServerUDP {
private:
    std::map<sf::Uint16, std::shared_ptr<Player>> _players{};

public:
    Server() = default;

    void broadcast() override;

    void processConnect(sf::Uint16 senderId) override;
    void processClientUpdate(sf::Uint16 senderId, sf::Packet& packet) override;
    void processDisconnect(sf::Uint16 senderId) override;

    void processCustomPacket(sf::Packet& packet, sf::Uint16 senderId) override;

    void processStop() override;
};


#endif //MINECRAFT_3DZAVR_SERVER_H
