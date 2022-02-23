//
// Created by Иван Ильин on 25.05.2021.
//

#include <utility>
#include <SFML/Network/Packet.hpp>

#include "Client.h"
#include "engine/utils/Log.h"
#include "MinecraftMsgType.h"
#include "engine/animation/Animations.h"

void Client::updatePacket() {
    sf::Packet packet;
    packet << MsgType::ClientUpdate << _player->position().x() << _player->position().y() << _player->position().z() << _player->angle().y() << _player->headAngle();
    _socket.send(packet, _socket.serverId());
}

void Client::processInit(sf::Packet& packet) {
    sf::Uint16 targetId;
    double x, y, z;

    while (packet >> targetId >> x >> y >> z)
    {
        if(targetId != _socket.ownId()) {
            if(_spawnPlayerCallBack != nullptr)
                _spawnPlayerCallBack(targetId);

            _players[targetId]->translateToPoint(Vec3D{x, y, z});
        }
    }
}

void Client::processUpdate(sf::Packet& packet) {
    sf::Uint16 targetId;
    double x, y, z, angleBody, headAngle;

    while (packet >> targetId >> x >> y >> z >> angleBody >> headAngle)
    {
        if (_players.count(targetId)) {
            if (targetId != _socket.ownId()) {
                std::string name = "Player_" + std::to_string(targetId);

                Vec3D newPosition = Vec3D{x, y, z};

                bool isAnimate = (_players[targetId]->position() - newPosition).sqrAbs() > 0.2;

                _players[targetId]->translateToPoint(newPosition);
                _players[targetId]->rotateToAngle(Vec3D{0, angleBody, 0});

                auto head = _players[targetId]->attached(ObjectNameTag(name + "_head"));

                if(head != nullptr) {
                    head->rotateLeft(headAngle - _players[targetId]->headAngle());
                }
                _players[targetId]->setHeadAngle(headAngle);

                auto foot1 = _players[targetId]->attached(ObjectNameTag(name + "_foot_1"));
                auto foot2 = _players[targetId]->attached(ObjectNameTag(name + "_foot_2"));

                if (isAnimate) {
                    if (foot1 != nullptr && foot2 != nullptr &&
                        !Timeline::isInAnimList(AnimationListTag(name + "_foot1_rotation"))) {
                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot1_rotation"),
                                                            foot1, 0.6, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);
                        Timeline::addAnimation<AWait>(AnimationListTag(name + "_foot1_rotation"), 0);
                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot1_rotation"),
                                                            foot1, -1.2, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);
                        Timeline::addAnimation<AWait>(AnimationListTag(name + "_foot1_rotation"), 0);
                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot1_rotation"),
                                                            foot1, 0.6, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);

                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot2_rotation"),
                                                            foot2, -0.6, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);
                        Timeline::addAnimation<AWait>(AnimationListTag(name + "_foot2_rotation"), 0);
                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot2_rotation"),
                                                            foot2, 1.2, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);
                        Timeline::addAnimation<AWait>(AnimationListTag(name + "_foot2_rotation"), 0);
                        Timeline::addAnimation<ARotateLeft>(AnimationListTag(name + "_foot2_rotation"),
                                                            foot2, -0.6, 0.2, Animation::LoopOut::None,
                                                            Animation::InterpolationType::Linear);
                    }
                }
            }
        }
    }
}

void Client::processNewClient(sf::Packet& packet) {
    sf::Uint16 targetId;
    packet >> targetId;

    if(_spawnPlayerCallBack != nullptr)
        _spawnPlayerCallBack(targetId);
}

void Client::processDisconnect(sf::Uint16 targetId) {
    if (targetId != _socket.ownId() && _players.count(targetId)) {
        if(_removePlayerCallBack != nullptr)
            _removePlayerCallBack(targetId);
        _players.erase(targetId);
    }
}


void Client::processCustomPacket(sf::Packet& packet) {
    Cube::Type cubeType;
    int buff[3];

    MinecraftMsgType type;
    packet >> type;

    switch (type) {
        case MinecraftMsgType::AddCube:
            sf::Uint16 tmp;
            packet >> tmp >> buff[0] >> buff[1] >> buff[2];
            cubeType = Cube::Type(tmp);
            if(_addCubeCallBack != nullptr) {
                _addCubeCallBack(Vec3D(buff[0], buff[1], buff[2]), cubeType);
            }
            Log::log("Client: AddCube (" + std::to_string(tmp) + ") at [" + std::to_string(buff[0]) + ", " + std::to_string(buff[1]) + ", " + std::to_string(buff[2]) + "]");

            break;
        case MinecraftMsgType::RemoveCube:
            packet >> buff[0] >> buff[1] >> buff[2];
            if(_removeCubeCallBack != nullptr) {
                _removeCubeCallBack(Vec3D(buff[0], buff[1], buff[2]));
            }
            Log::log("Client: RemoveCube at [" + std::to_string(buff[0]) + ", " + std::to_string(buff[1]) + ", " + std::to_string(buff[2]) + "]");

            break;
    }
}

void Client::processDisconnected() {
    for (auto it = _players.begin(); it != _players.end();) {
        processDisconnect(it++->first);
    }
}

void Client::addCube(const Vec3D& pos, Cube::Type type) {
    sf::Packet packet;

    packet << MsgType::Custom << MinecraftMsgType::AddCube << (sf::Uint16)type << (int)pos.x() << (int)pos.y() << (int)pos.z();
    _socket.send(packet, _socket.serverId());

    Log::log("Client: AddCube (" + std::to_string(type) + ") at [" + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ", " + std::to_string(pos.z()) + "]");
}

void Client::removeCube(const Vec3D& pos) {
    sf::Packet packet;
    packet << MsgType::Custom << MinecraftMsgType::RemoveCube << (int)pos.x() << (int)pos.y() << (int)pos.z();
    _socket.send(packet, _socket.serverId());

    Log::log("Client: RemoveCube at [" + std::to_string(pos.x()) + ", " + std::to_string(pos.y()) + ", " + std::to_string(pos.z()) + "]");
}

void Client::setSpawnPlayerCallBack(std::function<void(sf::Uint16)> spawn) {
    _spawnPlayerCallBack = std::move(spawn);
}

void Client::setRemovePlayerCallBack(std::function<void(sf::Uint16)> remove) {
    _removePlayerCallBack = std::move(remove);
}

void Client::setAddCubeCallBack(std::function<void(const Vec3D&, Cube::Type)> addCube) {
    _addCubeCallBack = std::move(addCube);
}

void Client::setRemoveCubeCallBack(std::function<void(const Vec3D&)> removeCube) {
    _removeCubeCallBack = std::move(removeCube);
}

void Client::addPlayer(sf::Uint16 id, std::shared_ptr<Player> player) {
    _players.insert({ id, player });
}



