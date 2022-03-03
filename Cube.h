//
// Created by Иван Ильин on 15.03.2021.
//

#ifndef MINECRAFT_3DZAVR_CUBE_H
#define MINECRAFT_3DZAVR_CUBE_H

#include "engine/physics/RigidBody.h"
#include "MinecraftConsts.h"
#include <SFML/Graphics.hpp>

class Cube : public RigidBody {
public:
    enum Type {
        grass,
        sand,
        glass,
        water,
        stone,
        earth,
        wood,
        snow,

        none
    };
private:
    Type type;
public:
    explicit Cube(const Vec3D& pos, Type t) :
            RigidBody(Mesh::Cube(ObjectNameTag(
                    "cube_X_" + std::to_string((int)pos.x()) +
                    "_Y_" + std::to_string((int)pos.y()) +
                    "_Z_" + std::to_string((int)pos.z())), 2)
            ), type(t) {
        setColor(Cube::cubeColor(t));
        translate(pos*2);
        setCollider(true);
    }


    static sf::Color cubeColor(Type t);

    static std::string soundName(Cube::Type cube);
};


#endif //MINECRAFT_3DZAVR_CUBE_H
