//
// Created by Иван Ильин on 25.05.2021.
//

#ifndef MINECRAFT_3DZAVR_CLIENT_H
#define MINECRAFT_3DZAVR_CLIENT_H

#include <utility>

#include "engine/network/ClientUDP.h"
#include "Player.h"

class Client : public ClientUDP {
private:
    std::shared_ptr<Player> _player;
    std::map<sf::Uint16, std::shared_ptr<Player>> _players{};

    std::function<void(sf::Uint16)> _spawnPlayerCallBack;
    std::function<void(sf::Uint16)> _removePlayerCallBack;
    std::function<void(const Vec3D& pos, Cube::Type type)> _addCubeCallBack;
    std::function<void(const Vec3D& pos)> _removeCubeCallBack;
public:
    Client(std::shared_ptr<Player>  player) : _player(player) {};

    void updatePacket() override;

    void processInit(sf::Packet& packet) override;
    void processUpdate(sf::Packet& packet) override;
    void processNewClient(sf::Packet& packet) override;
    void processDisconnect(sf::Uint16 targetId) override;

    void processCustomPacket(sf::Packet& packet) override;

    void processDisconnected() override;

    void addCube(const Vec3D& pos, Cube::Type type);
    void removeCube(const Vec3D& pos);

    void addPlayer(sf::Uint16 id, std::shared_ptr<Player> player);

    void setSpawnPlayerCallBack(std::function<void(sf::Uint16)> spawn);
    void setRemovePlayerCallBack(std::function<void(sf::Uint16)> remove);
    void setAddCubeCallBack(std::function<void(const Vec3D& pos, Cube::Type type)> addCube);
    void setRemoveCubeCallBack(std::function<void(const Vec3D& pos)> removeCube);
};


#endif //MINECRAFT_3DZAVR_CLIENT_H
