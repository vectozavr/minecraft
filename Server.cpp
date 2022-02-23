//
// Created by Иван Ильин on 25.05.2021.
//

#include "engine/utils/Log.h"
#include "Server.h"
#include "MinecraftMsgType.h"

void Server::broadcast() {
    sf::Packet updatePacket;
    updatePacket << MsgType::ServerUpdate;

    for (auto& player : _players) {
        updatePacket << player.first << player.second->position().x() << player.second->position().y() << player.second->position().z() << player.second->angle().y() << player.second->headAngle();
    }

    for (auto& player : _players) {
        _socket.send(updatePacket, player.first);
    }
}


void Server::processConnect(sf::Uint16 targetId) {
    sf::Packet sendPacket;
    sf::Packet extraPacket;

    extraPacket << MsgType::NewClient << targetId;
    sendPacket << MsgType::Init << targetId;
    _players.insert({ targetId, std::make_shared<Player>(ObjectNameTag("Player_"+std::to_string(targetId))) });
    for (const auto& player : _players)
    {
        sendPacket << player.first << player.second->position().x() << player.second->position().y() << player.second->position().z();
        if (player.first != targetId)
            _socket.sendRely(extraPacket, player.first);
    }
    _socket.sendRely(sendPacket, targetId);
}

void Server::processClientUpdate(sf::Uint16 senderId, sf::Packet& packet) {
    double x, y, z, angleBody, headAngle;

    packet >> x >> y >> z >> angleBody >> headAngle;

    _players.at(senderId)->translateToPoint(Vec3D{ x, y, z });
    _players.at(senderId)->rotateToAngle(Vec3D{0, angleBody, 0});
    _players.at(senderId)->setHeadAngle(headAngle);
}

void Server::processDisconnect(sf::Uint16 senderId) {
    sf::Packet sendPacket;

    sendPacket << MsgType::Disconnect << senderId;
    _players.erase(senderId);
    for (const auto& player : _players)
        _socket.sendRely(sendPacket, player.first);
}


void Server::processCustomPacket(sf::Packet& packet, sf::Uint16 senderId) {
    sf::Packet sendPacket;
    int buff[3];
    sf::Uint16 tmp;
    Cube::Type cubeType;

    MinecraftMsgType type;
    packet >> type;

    switch (type) {
        case MinecraftMsgType::AddCube:
            packet >> tmp >> buff[0] >> buff[1] >> buff[2];
            sendPacket << MsgType::Custom << MinecraftMsgType::AddCube << tmp << buff[0] << buff[1] << buff[2];
            for(auto& _player : _players) {
                if(senderId != _player.first) {
                    _socket.sendRely(sendPacket, _player.first);
                }
            }

            Log::log("Server: AddCube (" + std::to_string(tmp) + ") at [" + std::to_string(buff[0]) + ", " + std::to_string(buff[1]) + ", " + std::to_string(buff[2]) + "]");

            break;
        case MinecraftMsgType::RemoveCube:
            packet >> buff[0] >> buff[1] >> buff[2];
            sendPacket << MsgType::Custom << MinecraftMsgType::RemoveCube << buff[0] << buff[1] << buff[2];
            for(auto& _player : _players) {
                if(senderId != _player.first) {
                    _socket.sendRely(sendPacket, _player.first);
                }
            }
            Log::log("Server: RemoveCube at [" + std::to_string(buff[0]) + ", " + std::to_string(buff[1]) + ", " + std::to_string(buff[2]) + "]");
            break;
    }
}

void Server::processStop() {
    for (auto it = _players.begin(); it != _players.end();)
        _players.erase(it++);
}
