#ifndef PLYHANDLER_H
#define PLYHANDLER_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

class PLYHandler {
public:
    static std::vector<glm::vec3> readPLY(const std::string& filePath);
};

#endif // FILEHANDLER_H
