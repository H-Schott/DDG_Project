#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
    //glm::vec2 TexCoords;

    Vertex(glm::vec3 P, glm::vec3 N, glm::vec3 C) :
        Position(P), Normal(N), Color(C) {}
};

struct Texture {
    unsigned int id;
    std::string type;
};

class Mesh {
public:
    GLenum primitives = GL_TRIANGLES;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    glm::vec3 min_vertex{0, 0, 0};
    glm::vec3 max_vertex{0, 0, 0};

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    Mesh(std::string path);
    ~Mesh();

    void Center();
    void Scale(double coeff);
    void SetPrimitives(GLenum prim);
    
    void setupMesh();
    void Draw(Shader& shader);

    static Mesh Frame();
    static Mesh Grid(int size);
    static Mesh Grid(int size, glm::vec3 center, glm::vec3 hsize1, glm::vec3 hsize2);
    static Mesh Sphere(int size);

private:
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

    void AddAiNode(const aiNode* node, const aiScene* scene);
    void AddAiMesh(const aiMesh* mesh, const aiScene* scene);
};



#endif