//
// Created by Иван Ильин on 19.09.2021.
//

#include "PlayerController.h"
#include "engine/utils/Log.h"
#include "engine/animation/Animations.h"
#include "engine/SoundController.h"

PlayerController::PlayerController(std::shared_ptr<Player> player,
                                   std::shared_ptr<Keyboard> keyboard,
                                   std::shared_ptr<Mouse> mouse) : _player(player), _keyboard(keyboard), _mouse(mouse) {
}

void PlayerController::update() {
    bool inRunning_old = _inRunning;
    _inRunning = Keyboard::isKeyPressed(sf::Keyboard::A) || Keyboard::isKeyPressed(sf::Keyboard::D) || Keyboard::isKeyPressed(sf::Keyboard::W) || Keyboard::isKeyPressed(sf::Keyboard::S);

    auto camera = _player->attached(ObjectNameTag("Camera"));
    // in case when the camera is attached we make some animation during running
    if(_inRunning && camera != nullptr) {
        if (!Timeline::isInAnimList(AnimationListTag("camera_hor_oscil"))) {
            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_hor_oscil"), camera, -camera->left() / 3, 0.3, Animation::LoopOut::None, Animation::InterpolationType::Cos);
            Timeline::addAnimation<AWait>(AnimationListTag("camera_hor_oscil"), 0);
            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_hor_oscil"), camera,  camera->left() / 3, 0.3, Animation::LoopOut::None, Animation::InterpolationType::Cos);

            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_vert_oscil"), camera, -Vec3D{0, 1, 0} / 6, 0.15, Animation::LoopOut::None, Animation::InterpolationType::Cos);
            Timeline::addAnimation<AWait>(AnimationListTag("camera_vert_oscil"), 0);
            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_vert_oscil"), camera, Vec3D{0, 1, 0} / 6, 0.15, Animation::LoopOut::None, Animation::InterpolationType::Cos);
            Timeline::addAnimation<AWait>(AnimationListTag("camera_vert_oscil"), 0);
            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_vert_oscil"), camera, -Vec3D{0, 1, 0} / 6, 0.15, Animation::LoopOut::None, Animation::InterpolationType::Cos);
            Timeline::addAnimation<AWait>(AnimationListTag("camera_vert_oscil"), 0);
            Timeline::addAnimation<ATranslate>(AnimationListTag("camera_vert_oscil"), camera, Vec3D{0, 1, 0} / 6, 0.15, Animation::LoopOut::None, Animation::InterpolationType::Cos);

            Timeline::addAnimation<ATranslateToPoint>(AnimationListTag("camera_init"), camera, _player->position() + Vec3D{0, 1.8, 0}, 0.3, Animation::LoopOut::None, Animation::InterpolationType::Cos);
        }

    } else if(inRunning_old && !_inRunning && camera != nullptr) {
        Timeline::deleteAnimationList(AnimationListTag("camera_hor_oscil"));
        Timeline::deleteAnimationList(AnimationListTag("camera_vert_oscil"));
        Timeline::addAnimation<ATranslateToPoint>(AnimationListTag("camera_init"), camera, _player->position() + Vec3D{0, 1.8, 0}, 0.15, Animation::LoopOut::None, Animation::InterpolationType::Cos);
    }

    // Left and right
    if (Keyboard::isKeyPressed(sf::Keyboard::A)) {
        _player->translate(_player->left() * Time::deltaTime() * MinecraftConsts::WALK_SPEED);
    }

    if (Keyboard::isKeyPressed(sf::Keyboard::D)) {
        _player->translate(-_player->left() * Time::deltaTime() * MinecraftConsts::WALK_SPEED);
    }

    // Forward and backward
    if (Keyboard::isKeyPressed(sf::Keyboard::W)) {
        _player->translate(_player->left().cross(Vec3D{0, 1, 0}) * Time::deltaTime() * MinecraftConsts::WALK_SPEED);
    }

    if (Keyboard::isKeyPressed(sf::Keyboard::S)) {
        _player->translate(-_player->left().cross(Vec3D{0, 1, 0}) * Time::deltaTime() * MinecraftConsts::WALK_SPEED);
    }

    if (Keyboard::isKeyPressed(sf::Keyboard::Space) && _player->inCollision()) {
        _player->setVelocity(Vec3D{0, sqrt(2 * -_player->acceleration().y() * MinecraftConsts::JUMP_HEIGHT), 0});
    }

    // Mouse movement
    Vec2D disp = _mouse->getMouseDisplacement();

    _player->rotate(Vec3D{0, -disp.x() * MinecraftConsts::MOUSE_SENSITIVITY, 0});

    double rotationLeft = disp.y() * MinecraftConsts::MOUSE_SENSITIVITY;

    // You can only see in range [-90 : 90] grad
    if (_player->headAngle() + rotationLeft > M_PI / 2) {
        rotationLeft = M_PI / 2 - _player->headAngle();
    }
    if (_player->headAngle() + rotationLeft < -M_PI / 2) {
        rotationLeft = -M_PI / 2 - _player->headAngle();
    }

    _player->setHeadAngle(_player->headAngle() + rotationLeft);
    if(camera != nullptr) {
        auto rate = _player->headAngle() - camera->angleLeftUpLookAt().x();
        camera->rotateLeft(rate);
    }

    if (_keyboard->isKeyTapped(sf::Keyboard::Right)) {
        // change '_selectedBlock'
        _player->nextBlock();
        Log::log("selected block " + std::to_string(_player->selectedBlock()));
        _updateCubeInHandCallBack();
    }
    if (_keyboard->isKeyTapped(sf::Keyboard::Left)) {
        // change '_selectedBlock'
        _player->previousBlock();
        Log::log("selected block " + std::to_string(_player->selectedBlock()));
        _updateCubeInHandCallBack();
    }
    if (_mouse->isButtonTapped(sf::Mouse::Button::Right)) {
        _addCubeCallBack();

        SoundController::loadAndPlay(SoundTag("add"), Cube::soundName(_player->selectedBlock()));
    }
    if (_mouse->isButtonTapped(sf::Mouse::Button::Left)) {
        Cube::Type t = _removeCubeCallBack();
        if (t != Cube::none) {
            SoundController::loadAndPlay(SoundTag("remove"), Cube::soundName(t));
        }
    }

    if (_inRunning && _player->inCollision() && SoundController::getStatus(SoundTag("steps")) != sf::Sound::Status::Playing) {
        int soundNum = round((double) rand() / RAND_MAX * 5) + 1;
        SoundController::loadAndPlay(SoundTag("steps"), "sound/stonestep" + std::to_string(soundNum) + ".ogg");
    }
    if ((_oldVelocity - _player->velocity()).abs() > 30) {
        SoundController::loadAndPlay(SoundTag("fallbig"), "sound/fallbig.ogg");
    }

    _oldVelocity = _player->velocity();
}

void PlayerController::setAddCubeCallBack(std::function<void()> addCube) {
    _addCubeCallBack = std::move(addCube);
}

void PlayerController::setRemoveCubeCallBack(std::function<Cube::Type()> removeCube) {
    _removeCubeCallBack = std::move(removeCube);
}

void PlayerController::setUpdateCubeInHandCallBack(std::function<void()> updateCube) {
    _updateCubeInHandCallBack = std::move(updateCube);
}
