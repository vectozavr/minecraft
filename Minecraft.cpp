//
// Created by Иван Ильин on 03.10.2021.
//

#include <fstream>
#include "Minecraft.h"
#include "engine/animation/Animations.h"
#include "engine/ResourceManager.h"
#include "engine/SoundController.h"

using namespace std;

// Read server/client settings and start both.
// If client doesn't connect to the localhost - server doesn't start.
void Minecraft::InitNetwork()
{
    std::string clientIp;
    sf::Uint16 clientPort;
    sf::Uint16 serverPort;
    std::ifstream connectFile("connect.txt", std::ifstream::in);

    // If failed to read client settings
    if (!connectFile.is_open() || !(connectFile >> clientIp >> clientPort) || sf::IpAddress(clientIp) == sf::IpAddress::None)
    {
        connectFile.close();
        // Create file and write default settings
        clientIp = "127.0.0.1";
        clientPort = 54000;
        std::ofstream temp("connect.txt", std::ofstream::out);
        temp << clientIp << std::endl << clientPort;
        temp.close();
    }
    connectFile.close();

    // If failed to read server settings
    connectFile.open("server.txt", std::ifstream::in);
    if (!connectFile.is_open() || !(connectFile >> serverPort))
    {
        connectFile.close();
        // Create file and write default settings
        serverPort = 54000;
        std::ofstream temp("server.txt", std::ofstream::out);
        temp << serverPort;
        temp.close();
    }
    connectFile.close();

    if (clientIp == sf::IpAddress::LocalHost) {
        server->start(serverPort);
    }

    client->connect(clientIp, clientPort);

    client->setSpawnPlayerCallBack([this](sf::Uint16 id){ spawnPlayer(id); });
    client->setRemovePlayerCallBack([this](sf::Uint16 id){ removePlayer(id); });

    client->setAddCubeCallBack([this](const Vec3D& pos, Cube::Type type){ map->addCube(pos, type); });
    client->setRemoveCubeCallBack([this](const Vec3D& pos){ map->removeCube(pos); } );
}

void Minecraft::start() {
    // This code executed once in the beginning:
    screen->setMouseCursorVisible(true);
    setUpdateWorld(false);

    map->loadMap("maps/map_test");

    playerController->setAddCubeCallBack([this](){addCube();});
    playerController->setRemoveCubeCallBack([this](){ return removeCube();});
    playerController->setUpdateCubeInHandCallBack([this](){updateCubeInHandColor();});

    camera->translateToPoint(player->position() + Vec3D{0, 1.8, 0});
    player->attach(camera);
    player->translateToPoint(Vec3D{0, 2, 0}*MinecraftConsts::WORLD_SCALE);

    world->loadBody(ObjectNameTag("cube_in_hand"), MinecraftConsts::CUBE_OBJ);
    auto cube_in_hand = world->body(ObjectNameTag("cube_in_hand"));
    cube_in_hand->setCollider(false);
    cube_in_hand->translateToPoint(player->position() + Vec3D{-3, -1, 1.4});
    cube_in_hand->rotate(Vec3D{0, M_PI/10, 0});
    camera->attach(cube_in_hand);

    cube_in_hand->setColor(Cube::cubeColor(player->selectedBlock()));

    world->addBody(player);

    // connecting to the server
    InitNetwork();
    // Waiting for connect and updating server if it's same window
    while (client->isWorking() && !client->connected())
    {
        client->update();
        server->update();
        Time::update();
    }
    // If connect fail - return to menu
    if (!client->isWorking())
    {
        inGame = false;
        server->stop();
    }

    // windows init:
    mainMenu.setTitle("Main menu");
    mainMenu.setBackgroundTexture(MinecraftConsts::MAIN_MENU_BACK, 1.1, 1.1, screen->width(), screen->height());

    mainMenu.addButton(screen->width()/2, 200, 200, 20, [this] () {
        this->play();
        SoundController::loadAndPlay(SoundTag("click"), MinecraftConsts::CLICK_SOUND);
    }, "Server: " + client->serverIp().toString(), 5, 5, MinecraftConsts::MAIN_MENU_GUI, {0, 66}, {0, 86}, {0, 46}, Consts::MEDIUM_FONT, {255, 255, 255});
    mainMenu.addButton(screen->width()/2, 350, 200, 20, [this] () {
        this->player->translateToPoint(Vec3D{0, 6, 0});
        this->player->setVelocity({});
        this->play();
        SoundController::loadAndPlay(SoundTag("click"), MinecraftConsts::CLICK_SOUND);
    }, "Respawn", 5, 5, MinecraftConsts::MAIN_MENU_GUI, {0, 66}, {0, 86}, {0, 46}, Consts::MEDIUM_FONT, {255, 255, 255});
    mainMenu.addButton(screen->width()/2, 500, 200, 20, [this] () {
        this->map->saveMap("maps/map_test");
        SoundController::loadAndPlay(SoundTag("click"), MinecraftConsts::CLICK_SOUND);
    }, "Save map", 5, 5, MinecraftConsts::MAIN_MENU_GUI, {0, 66}, {0, 86}, {0, 46}, Consts::MEDIUM_FONT, {255, 255, 255});

    mainMenu.addButton(screen->width()/2, 750, 200, 20, [this] () {
        client->disconnect();
        server->stop();
        this->exit();
    }, "Exit", 5, 5, MinecraftConsts::MAIN_MENU_GUI, {0, 66}, {0, 86}, {0, 46}, Consts::MEDIUM_FONT, {255, 255, 255});

}

void Minecraft::update() {
    // This code executed every time step:

    server->update();
    client->update();

    // Check all input after this condition please
    if (!screen->hasFocus())
        return;

    if(keyboard->isKeyTapped(sf::Keyboard::Escape)) {
        inGame = !inGame;
        screen->setMouseCursorVisible(!inGame);
    }

    if (keyboard->isKeyTapped(sf::Keyboard::O)) {
        setGlEnable(!glEnable());
    }

    if (keyboard->isKeyTapped(sf::Keyboard::Tab)) {
        setDebugInfo(!showDebugInfo());
    }

    if (keyboard->isKeyTapped(sf::Keyboard::P)) {
        screen->startRender();
    }

    if (keyboard->isKeyTapped(sf::Keyboard::L)) {
        screen->stopRender();
    }

    if(inGame) {
        screen->setTitle("Minecraft");
        playerController->update();
    } else {
        mainMenu.update();
    }

    setUpdateWorld(inGame);

    // background sounds and music control
    if(SoundController::getStatus(SoundTag("backNoise")) != sf::Sound::Status::Playing) {
        SoundController::loadAndPlay(SoundTag("backNoise"), MinecraftConsts::BACK_NOISE);
    }

    if(SoundController::getStatus(SoundTag("music")) != sf::Sound::Status::Playing) {
        int soundNum = round((double) rand() / RAND_MAX * 8) + 1; // random number from 1 to 9
        SoundController::loadAndPlay(SoundTag("music"), MinecraftConsts::MUSIC_PATH + "music" + std::to_string(soundNum) + ".ogg");
    }
}

void Minecraft::gui() {

    if(inGame) {
        sf::Sprite sprite;
        sprite.setTexture(*ResourceManager::loadTexture(MinecraftConsts::MAIN_MENU_GUI));
        sprite.setTextureRect(sf::IntRect(243, 3, 9, 9));
        sprite.scale(6, 6);
        sprite.setPosition(screen->width() / 2 - 27, screen->height() / 2 - 27);
        screen->drawSprite(sprite);
    }
}

void Minecraft::play() {
    inGame = true;
    screen->setMouseCursorVisible(false);
}

void Minecraft::spawnPlayer(sf::Uint16 id) {
    std::string name = "Player_" + std::to_string(id);

    std::shared_ptr<Player> newPlayer = std::make_shared<Player>(ObjectNameTag(name),
                                                                 MinecraftConsts::BODY_OBJ,
                                                                 Vec3D{0.4, 0.4, 0.4});

    client->addPlayer(id, newPlayer);
    world->addBody(newPlayer);
    newPlayer->setVisible(true);
    newPlayer->setCollision(false);
    newPlayer->setAcceleration(Vec3D{0, 0, 0});

    // add head and other stuff:
    world->loadBody(ObjectNameTag(name + "_head"), MinecraftConsts::HEAD_OBJ, Vec3D{0.4, 0.4, 0.4});
    world->body(ObjectNameTag(name + "_head"))->translate(Vec3D{0, 2.2, 0});
    newPlayer->attach(world->body(ObjectNameTag(name + "_head")));

    world->loadBody(ObjectNameTag(name + "_foot_1"), MinecraftConsts::FOOT_OBJ, Vec3D{0.4, 0.4, 0.4});
    world->body(ObjectNameTag(name + "_foot_1"))->translate(Vec3D{-0.25, 0, 0});
    newPlayer->attach(world->body(ObjectNameTag(name + "_foot_1")));

    world->loadBody(ObjectNameTag(name + "_foot_2"), MinecraftConsts::FOOT_OBJ, Vec3D{0.4, 0.4, 0.4});
    world->body(ObjectNameTag(name + "_foot_2"))->translate(Vec3D{0.25, 0, 0});
    newPlayer->attach(world->body(ObjectNameTag(name + "_foot_2")));

    int colorBodyNum = static_cast<int> (static_cast<double>((rand() - 1)) / RAND_MAX * 5.0);
    int colorFootNum = static_cast<int> (static_cast<double>((rand() - 1)) / RAND_MAX * 5.0);

    newPlayer->setColor(Consts::WHITE_COLORS[colorBodyNum]);
    world->body(ObjectNameTag(name + "_foot_1"))->setColor(Consts::DARK_COLORS[colorFootNum]);
    world->body(ObjectNameTag(name + "_foot_2"))->setColor(Consts::DARK_COLORS[colorFootNum]);
}

void Minecraft::removePlayer(sf::Uint16 id) {
    std::string name = "Player_" + std::to_string(id);

    auto playerToRemove = world->body(ObjectNameTag(name));

    Timeline::addAnimation<AScale>(AnimationListTag(name + "_remove"), playerToRemove, Vec3D(0.01, 0.01, 0.01));
    Timeline::addAnimation<AFunction>(AnimationListTag(name + "_remove"), [this, name](){
        world->removeBody(ObjectNameTag(name));
        world->removeBody(ObjectNameTag(name + "_head"));
        world->removeBody(ObjectNameTag(name + "_weapon"));
        world->removeBody(ObjectNameTag(name + "_foot_1"));
        world->removeBody(ObjectNameTag(name + "_foot_2"));
    });
}

void Minecraft::addCube() {
    auto rayCast = world->rayCast(camera->position(), camera->position() + camera->lookAt(), "Player");
    if ((rayCast.pointOfIntersection - camera->position()).abs() < MinecraftConsts::REACH_RANGE*MinecraftConsts::WORLD_SCALE) {
        Vec3D cubePoint = rayCast.intersectedTriangle.position() + rayCast.intersectedTriangle.norm()*MinecraftConsts::WORLD_SCALE/2;
        Vec3D pos = Vec3D(round(cubePoint.x() / MinecraftConsts::WORLD_SCALE), round(cubePoint.y() / MinecraftConsts::WORLD_SCALE), round(cubePoint.z() / MinecraftConsts::WORLD_SCALE));

        map->addCube(pos, player->selectedBlock());

        client->addCube(pos, player->selectedBlock());
    }
}

Cube::Type Minecraft::removeCube() {
    auto rayCast = world->rayCast(camera->position(), camera->position() + camera->lookAt(), "Player");
    if ((rayCast.pointOfIntersection - camera->position()).abs() < MinecraftConsts::REACH_RANGE*MinecraftConsts::WORLD_SCALE) {
        Vec3D cubePoint = rayCast.intersectedTriangle.position() - rayCast.intersectedTriangle.norm()*MinecraftConsts::WORLD_SCALE/2;
        Vec3D pos = Vec3D(round(cubePoint.x() / MinecraftConsts::WORLD_SCALE), round(cubePoint.y() / MinecraftConsts::WORLD_SCALE), round(cubePoint.z() / MinecraftConsts::WORLD_SCALE));

        Cube::Type t = map->removeCube(pos);

        client->removeCube(pos);
        return t;
    }
    return Cube::none;
}

void Minecraft::updateCubeInHandColor() {
    world->body(ObjectNameTag("cube_in_hand"))->setColor(Cube::cubeColor(player->selectedBlock()));
}
