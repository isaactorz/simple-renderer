#include <vector>

#ifndef OBJLOADER_H
#define OBJLOADER_H

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& gourad_normals,
    std::vector<glm::vec3>& flat_normals
);

#endif
