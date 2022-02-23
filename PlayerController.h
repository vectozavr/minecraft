//
// Created by Иван Ильин on 19.09.2021.
//

#ifndef SHOOTER_PLAYERCONTROLLER_H
#define SHOOTER_PLAYERCONTROLLER_H

#include "Player.h"
#include "engine/Keyboard.h"
#include "engine/Mouse.h"

class PlayerController {
private:
    std::shared_ptr<Player> _player;
    std::shared_ptr<Keyboard> _keyboard;
    std::shared_ptr<Mouse> _mouse;

    bool _inRunning = false;

    Vec3D _oldVelocity;

    std::function<void()> _addCubeCallBack;
    std::function<Cube::Type()> _removeCubeCallBack;
    std::function<void()> _updateCubeInHandCallBack;

public:
    PlayerController(std::shared_ptr<Player> player, std::shared_ptr<Keyboard> keyboard, std::shared_ptr<Mouse> mouse);
    void update();

    void setAddCubeCallBack(std::function<void()> addCube);
    void setRemoveCubeCallBack(std::function<Cube::Type()> removeCube);
    void setUpdateCubeInHandCallBack(std::function<void()> updateCube);
};


#endif //SHOOTER_PLAYERCONTROLLER_H
