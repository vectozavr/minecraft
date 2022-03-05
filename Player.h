//
// Created by Иван Ильин on 14.03.2021.
//

#ifndef MINECRAFT_3DZAVR_PLAYER_H
#define MINECRAFT_3DZAVR_PLAYER_H

#include <SFML/Audio/Sound.hpp>
#include <utility>
#include "engine/physics/RigidBody.h"
#include "engine/ResourceManager.h"
#include "Cube.h"
#include "MinecraftConsts.h"

class Player : public RigidBody {
private:
    double _headAngle = 0;

    Cube::Type _selectedBlock = Cube::Type::stone;
public:
    explicit Player(const ObjectNameTag& nameTag,
                    const std::string &filename = "obj/cube.obj",
                    const Vec3D &scale = Vec3D(0.6, 1.8, 0.6));

    // This is for situation when you want to store the position of the head but you dont have attached camera
    void setHeadAngle(double a) { _headAngle = a; }
    [[nodiscard]] double headAngle() const { return _headAngle; };

    [[nodiscard]] Cube::Type selectedBlock() const { return _selectedBlock; }
    void nextBlock();
    void previousBlock();
};


#endif //MINECRAFT_3DZAVR_PLAYER_H
