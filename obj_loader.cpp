#include<stdio.h>
#include<vector>
#include<string>
#include<cstring>

#include<glm/glm.hpp>
#include<glm/vec3.hpp>
#include<glm/vec2.hpp>

#include "obj_loader.hpp"

using namespace std;
using namespace glm;

bool loadOBJ(
    const char* path,
    std::vector<glm::vec3>& out_vertices,
    std::vector<glm::vec2>& out_uvs,
    std::vector<glm::vec3>& gourad_normals,
    std::vector<glm::vec3>& flat_normals
){
    printf("Loading OBJ file %s...\n", path);

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE* file = fopen(path, "r");
    if (file == NULL){
        printf("file not found\n");
        getchar();
        return false;
    }
    
    while(1){
        char lineHeader[128];
        //read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if(res == EOF) break;
        
        if(strcmp(lineHeader, "v")==0){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
            temp_normals.push_back(vec3(0,0,0));
        } else if (strcmp(lineHeader, "vt")==0){
             glm::vec2 uv;
             fscanf(file, "%f %f\n", &uv.x, &uv.y);
             temp_uvs.push_back(uv);
        } else if ( strcmp( lineHeader, "vn" ) == 0){
             glm::vec3 normal;
             fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
             temp_normals.push_back(normal);
        } else if ( strcmp( lineHeader, "f" ) ==0){
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d %d %d\n",
                                 &vertexIndex[0],
                                 &vertexIndex[1],
                                 &vertexIndex[2]);

            if (matches != 3){
                printf("File can't be read by simple parser :\n");
                return false;
            }

            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            //uvIndices    .push_back(uvIndex[0]); 
            //uvIndices    .push_back(uvIndex[1]); 
            //uvIndices    .push_back(uvIndex[2]); 
            //normalIndices.push_back(normalIndex[0]);
            //normalIndices.push_back(normalIndex[1]);
            //normalIndices.push_back(normalIndex[2]);
        }else{
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    for( unsigned int i=0; i<vertexIndices.size(); i+=3){
      unsigned int vertexIndex1 = vertexIndices[i];
      unsigned int vertexIndex2 = vertexIndices[i+1];
      unsigned int vertexIndex3 = vertexIndices[i+2];
      //unsigned int vertexIndex = vertexIndices[i];
        //unsigned int uvIndex = uvIndices[i];
      // unsigned int normalIndex = normalIndices[i];

        glm::vec3 v1 = temp_vertices[vertexIndex1-1];
        glm::vec3 v2 = temp_vertices[vertexIndex2-1];
        glm::vec3 v3 = temp_vertices[vertexIndex3-1];

        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;

        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        temp_normals[vertexIndex1-1] += normal;
        temp_normals[vertexIndex2-1] += normal;
        temp_normals[vertexIndex3-1] += normal;
        
        //glm::vec2 uv = temp_uvs[uvIndex-1];
        //glm::vec3 vertex = temp_vertices[vertexIndex-1];
        //glm::vec3 normal = temp_normals[normalIndex-1];

        out_vertices.push_back(v1);
        out_vertices.push_back(v2);
        out_vertices.push_back(v3);

        flat_normals.push_back(normal);
        flat_normals.push_back(normal);
        flat_normals.push_back(normal);
        //out_vertices.push_back(vertex);
        //gourad_normals .push_back(normal);
        //out_uvs     .push_back(uv);
    }

    for(unsigned int i=0; i<vertexIndices.size(); i++)
    {
      unsigned int vertexIndex = vertexIndices[i];
      glm::vec3 normal = temp_normals[vertexIndex-1];
      normal = glm::normalize(normal);
      gourad_normals.push_back(normal);
    }

    fclose(file);
    return true;
}

