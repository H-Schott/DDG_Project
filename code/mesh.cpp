#include "mesh.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}


Mesh::Mesh(std::string path) {
    Assimp::Importer import;
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    AddAiNode(scene->mRootNode, scene);

    std::cout << indices.size() / 3. << " faces." << std::endl;
    std::cout << vertices.size() << " vertices." << std::endl;
    setupMesh();
}


void Mesh::AddAiNode(const aiNode* node, const aiScene* scene) {
    for(int i = 0; i < node->mNumMeshes; i++) {
        AddAiMesh(scene->mMeshes[node->mMeshes[i]], scene);
    }

    for(int i = 0; i < node->mNumChildren; i++) {
        AddAiNode(node->mChildren[i], scene);
    }
}


void Mesh::AddAiMesh(const aiMesh* mesh, const aiScene* scene) {
    int offset = vertices.size();
    for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
        glm::vec3 p;
        p.x = mesh->mVertices[i].x;
        p.y = mesh->mVertices[i].y;
        p.z = mesh->mVertices[i].z;
        // update min/max
        for (int j = 0; j < 3; j++) {
            if (p[j] > max_vertex[j]) max_vertex[j] = p[j];
            if (p[j] < min_vertex[j]) min_vertex[j] = p[j];
        }
        
        glm::vec3 n;
        n.x = mesh->mNormals[i].x;
        n.y = mesh->mNormals[i].y;
        n.z = mesh->mNormals[i].z;
        //glm::vec3 c(0.8, 0.8, 0.8);
        glm::vec3 c(0.38, 0.306, 0.102);
        vertices.push_back(Vertex(p, n, c));
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(offset + face.mIndices[j]);
    }
}


Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Mesh::setupMesh() {
    if (VAO == 0) glGenVertexArrays(1, &VAO);
    if (VBO == 0) glGenBuffers(1, &VBO);
    if (EBO == 0) glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    glBindVertexArray(0);
}


void Mesh::Center() {
    glm::vec3 centre = (max_vertex + min_vertex);
    centre *= 0.5;
    int nb_vertices = vertices.size();
    for (int i = 0; i < nb_vertices; i++) {
        vertices[i].Position -= centre;
    }

    min_vertex -= centre;
    max_vertex -= centre;
}


void Mesh::Scale(double coeff) {
    int nb_vertices = vertices.size();
    for (int i = 0; i < nb_vertices; i++) {
        vertices[i].Position *= coeff;
    }

    min_vertex *= coeff;
    max_vertex *= coeff;
}


void Mesh::SetPrimitives(GLenum prim) {
    primitives = prim;
}


void Mesh::Draw(Shader& shader) {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(primitives, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


Mesh Mesh::Frame() {
    std::vector<Vertex> v;
    v.push_back(Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)));
    v.push_back(Vertex(glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(1, 0, 0)));
    v.push_back(Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
    v.push_back(Vertex(glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)));
    v.push_back(Vertex(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)));
    v.push_back(Vertex(glm::vec3(0, 0, 1), glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)));
    std::vector<unsigned int> i {0, 1, 2, 3, 4, 5};
    std::vector<Texture> t;

    Mesh m = Mesh(v, i, t);
    m.SetPrimitives(GL_LINES);

    return m;
}


Mesh Mesh::Grid(int size) {
    return Grid(size, glm::vec3(0, 0, 0), glm::vec3(1.5, 0, 0), glm::vec3(0, 0, 1.5));
}


Mesh Mesh::Grid(int size, glm::vec3 center, glm::vec3 hsize1, glm::vec3 hsize2) {
    glm::vec3 n = glm::normalize(glm::cross(hsize1, hsize2));
    glm::vec3 white = glm::vec3(1, 1, 1);
    std::vector<Vertex> v;

    v.push_back(Vertex(center + hsize1, n, white));
    v.push_back(Vertex(center - hsize1, n, white));

    v.push_back(Vertex(center + hsize2, n, white));
    v.push_back(Vertex(center - hsize2, n, white));

    for (int j = 1; j < size + 1; j++) {
        glm::vec3 p = center + j / float(size) * hsize1;
        v.push_back(Vertex(p + hsize2, n, white));
        v.push_back(Vertex(p - hsize2, n, white));
        p = center - j / float(size) * hsize1;
        v.push_back(Vertex(p + hsize2, n, white));
        v.push_back(Vertex(p - hsize2, n, white));
        p = center + j / float(size) * hsize2;
        v.push_back(Vertex(p + hsize1, n, white));
        v.push_back(Vertex(p - hsize1, n, white));
        p = center - j / float(size) * hsize2;
        v.push_back(Vertex(p + hsize1, n, white));
        v.push_back(Vertex(p - hsize1, n, white));
    }
    

    std::vector<unsigned int> i;
    for (int j = 0; j < v.size(); j++) {
        i.push_back(j);
    }
    
    std::vector<Texture> t;

    Mesh m = Mesh(v, i, t);
    m.SetPrimitives(GL_LINES);
    return m;
}


Mesh Mesh::Sphere(int size) {

    glm::vec3 white = glm::vec3(1, 1, 1);
    std::vector<Vertex> v;

    // top and bottom vertices
    v.push_back(Vertex(glm::vec3(0., 0., -1), glm::vec3(0., 0., -1), white));
    v.push_back(Vertex(glm::vec3(0., 0., 1),  glm::vec3(0., 0., 1),  white));

    // min / max vertices
    glm::vec3 mi{ 0, 0, 0 };
    glm::vec3 ma{ 0, 0, 0 };

    // fill other vertices
    for (int i = 0; i < size; i++) {
        double xy_angle = 2 * M_PI / size * i;
        for (int j = 1; j < size; j++) {
            double z_angle = M_PI / size * j;
            glm::vec3 point(cos(xy_angle) * sin(z_angle), sin(xy_angle) * sin(z_angle), cos(z_angle));
            v.push_back(Vertex(point, point, white));
            // update min/max
            for (int j = 0; j < 3; j++) {
                if (point[j] > ma[j]) ma[j] = point[j];
                if (point[j] < mi[j]) mi[j] = point[j];
            }
        }
    }

    // create triangles
    std::vector<unsigned int> ids;
    /*ids.push_back(0);
    ids.push_back(2);
    ids.push_back(2 + size - 1);*/
    for (int i = 0; i < size; i++) {
        // top triangle
        ids.push_back(0);
        ids.push_back(2 + size - 2 + ((i + 1) % size) * (size - 1));
        ids.push_back(2 + size - 2 + i * (size - 1));
        for (int j = 0; j < size - 2; j++) {
            ids.push_back(2 + i * (size - 1) + j);
            ids.push_back(2 + i * (size - 1) + j + 1);
            ids.push_back(2 + ((i + 1) % size) * (size - 1) + j);

            ids.push_back(2 + i * (size - 1) + j + 1);
            ids.push_back(2 + ((i + 1) % size) * (size - 1) + j + 1);
            ids.push_back(2 + ((i + 1) % size) * (size - 1) + j);
        }
        // top triangle
        ids.push_back(1);
        ids.push_back(2 + i * (size - 1));
        ids.push_back(2 + ((i + 1) % size) * (size - 1));
    }

    std::vector<Texture> t;

    Mesh m = Mesh(v, ids, t);
    m.min_vertex = mi;
    m.max_vertex = ma;
    m.SetPrimitives(GL_TRIANGLES);
    return m;
}


// EOF