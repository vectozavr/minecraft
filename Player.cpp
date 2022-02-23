//
// Created by Иван Ильин on 14.03.2021.
//

#include "Player.h"
#include "engine/utils/Log.h"

Player::Player(const ObjectNameTag& nameTag, const std::string &filename, const Vec3D &scale) :
RigidBody(ObjectNameTag(nameTag), filename, scale) {
    setAcceleration(Vec3D{0, -MinecraftConsts::GRAVITY, 0});
    setCollision(true);
    setVisible(false);
    setColor({240, 168, 168});
}

void Player::nextBlock() {
    _selectedBlock = static_cast<Cube::Type>(((int) _selectedBlock + 1) % (int) Cube::Type::none);
}

void Player::previousBlock() {
    if ((int) _selectedBlock > 0) {
        _selectedBlock = static_cast<Cube::Type>(((int) _selectedBlock - 1) % (int) Cube::Type::none);
        Log::log("selected block " + std::to_string(_selectedBlock));
    } else {
        _selectedBlock = static_cast<Cube::Type>((int) Cube::Type::none - 1);
    }
}