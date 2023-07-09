#include "shader.hpp"


Shader::Shader(const char* vertexPath, const char* fragmentPath) {

    // VERTEX
    std::string vertexSource;
    std::ifstream vertexIn;
    vertexIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        vertexIn.open(vertexPath);
        std::stringstream vertexStream;
        vertexStream << vertexIn.rdbuf();
        vertexIn.close();
        vertexSource = vertexStream.str();
    }
    catch(std::ifstream::failure e) {
        std::cerr << "ERROR::VERTEX_SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* vertexShaderSource = vertexSource.c_str();

    unsigned int vertexShader;
    int success;
    char infoLog[512];
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::VERTEX_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // FRAGMENT
    std::string fragmentSource;
    std::ifstream fragmentIn;
    fragmentIn.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fragmentIn.open(fragmentPath);
        std::stringstream fragmentStream;
        fragmentStream << fragmentIn.rdbuf();
        fragmentIn.close();
        fragmentSource = fragmentStream.str();
    }
    catch(std::ifstream::failure e) {
        std::cerr << "ERROR::FRAGMENT_SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }

    const char* fragmentShaderSource = fragmentSource.c_str();

    unsigned int fragmentShader;
    
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::FRAGMENT_SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    
    // SHADER PROGRAM
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

}

void Shader::use() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform1i(uniformLocation, (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform1i(uniformLocation, value);
}

void Shader::setFloat(const std::string &name, float value) const {
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform1f(uniformLocation, value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniform3f(uniformLocation, vec.x, vec.y, vec.z);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    int uniformLocation = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &mat[0][0]);
}



// EOF