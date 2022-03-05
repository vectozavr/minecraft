//
// Created by Иван Ильин on 23.02.2022.
//

#ifndef MINECRAFT_MINECRAFTCONSTS_H
#define MINECRAFT_MINECRAFTCONSTS_H

namespace MinecraftConsts {
    const double WORLD_SCALE = 10;
    const double REACH_RANGE = 10;

    const double GRAVITY = 20;
    const double JUMP_HEIGHT = 1.2;
    const double WALK_SPEED = 5;
    const double MOUSE_SENSITIVITY = 1.0 / 1000.0;

    const std::string PLAYER_NAME = "Player";
    const std::string PROJECT_NAME = "Minecraft";

    const std::string CUBE_OBJ = "obj/cube.obj";
    const std::string MAIN_MENU_BACK = "textures/back.png";
    const std::string MAIN_MENU_GUI = "textures/gui.png";

    const std::string HEAD_OBJ = "obj/man/head.obj";
    const std::string BODY_OBJ = "obj/man/body.obj";
    const std::string FOOT_OBJ = "obj/man/foot.obj";
    const std::string MAN_OBJ = "obj/man/man.obj";

    const std::string CLICK_SOUND = "sound/click.ogg";
    const std::string BACK_NOISE = "sound/backNoise.ogg";
    const std::string MUSIC_PATH = "sound/music/";
}

#endif //MINECRAFT_MINECRAFTCONSTS_H
