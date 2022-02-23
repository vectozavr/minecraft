//
// Created by Иван Ильин on 15.03.2021.
//

#include "Map.h"
#include <iostream>
#include <fstream>

void Map::addCube(const Vec3D& pos, Cube::Type t) {
    std::string cubeName = "cube_X_" + std::to_string((int)pos.x()) + "_Y_" + std::to_string((int)pos.y()) + "_Z_" + std::to_string((int)pos.z());

    world->addBody(std::make_shared<Cube>(pos, t));
    cubes.insert({cubeName, {t, pos}});
}

Cube::Type Map::removeCube(const Vec3D& pos) {

    std::string cubeName = "cube_X_" + std::to_string((int)pos.x()) + "_Y_" + std::to_string((int)pos.y()) + "_Z_" + std::to_string((int)pos.z());

    world->removeBody(ObjectNameTag(cubeName));

    if(cubes.count(cubeName)) {
        Cube::Type t = cubes.at(cubeName).first;
        cubes.erase(cubeName);
        return t;
    }
    return Cube::none;
}

void Map::saveMap(const std::string& mapName) {
    std::ofstream outfile (mapName, std::fstream::trunc);

    for(auto& cube : cubes)
        outfile << cube.second.first << "\t" << (int)cube.second.second.x() << "\t" << (int)cube.second.second.y() << "\t" << (int)cube.second.second.z() << std::endl;

    outfile.close();
}

void Map::loadMap(const std::string &mapName) {
    std::ifstream infile (mapName, std::fstream::out);

    while (!infile.eof()) {
        int t, posX, posY, posZ;
        infile >> t >> posX >> posY >> posZ;
        addCube(Vec3D(posX, posY, posZ), static_cast<Cube::Type>(t));
    }

    infile.close();
}
