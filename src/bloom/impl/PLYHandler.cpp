#include "PLYHandler.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<glm::vec3> PLYHandler::readPLY(const std::string& filePath) {
    std::vector<glm::vec3> points;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return points;
    }

    std::string line;
    bool dataStarted = false;
    bool isVertexData = false;
    int vertexCount = 40000;
    int vertexRead = 0;

    while (std::getline(file, line)) {
        if (line.find("end_header") != std::string::npos) {
            dataStarted = true;
            std::cout << "Data started" << std::endl; // Debug: data start detected
            continue;
        }

        if (dataStarted) {
            if (!isVertexData) {
                // Check if it's the beginning of vertex data
            //     if (line.find("element vertex") != std::string::npos) {
            //         std::istringstream iss(line);
            //         std::string token;
            //         iss >> token >> token >> vertexCount;
            //         isVertexData = true;
            //         std::cout << "Vertex data detected. Vertex count: " << vertexCount << std::endl; // Debug: vertex data detected
            //     }
            // } else {
                // Read vertex data
                if (vertexRead < vertexCount) {
                    std::istringstream iss(line);
                    float x, y, z;
                    if (iss >> x >> y >> z) {
                        points.push_back(glm::vec3(x, y, z));
                        ++vertexRead;
                    } else {
                        std::cerr << "Error: Invalid data line in " << filePath << std::endl; // Debug: invalid data line
                    }
                }
            }
        }
    }

    std::cout << "Number of points read: " << points.size() << std::endl; // Debug: number of points read

    return points;
}
