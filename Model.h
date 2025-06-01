#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <map>
#include "tiny_gltf.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "shaderClass.h"

struct Mesh {
    VAO vao;
    VBO vbo;
    EBO ebo;
    std::vector<Texture> textures;
    int indexCount;
    std::string name;
    Mesh() : indexCount(0) {}
};

struct AnimationChannel {
    std::string path;
    std::vector<float> times;
    std::vector<glm::vec4> values;
    int targetNode;
};

struct Animation {
    std::string name;
    float duration;
    std::vector<AnimationChannel> channels;
};

struct Node {
    std::string name;
    glm::mat4 localTransform;
    glm::mat4 globalTransform;
    int parent;
    std::vector<int> children;
    int meshIndex;
};

class Model {
public:
    Model(const std::string& path);
    ~Model();

    void Draw(Shader& shader);
    void UpdateAnimation(float time);
    void SetAnimation(int animIndex);

private:
    std::string path;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;
    std::vector<Animation> animations;
    int currentAnimation;
    float animationTime;
    void LoadModel(const std::string& path);
    void ProcessNode(tinygltf::Model& model, int nodeIndex, int parentIndex);
    void ProcessMesh(tinygltf::Model& model, int meshIndex);
    void ProcessAnimations(tinygltf::Model& model);
    glm::vec4 InterpolateValues(const std::vector<float>& times, 
                               const std::vector<glm::vec4>& values, 
                               float currentTime);
    void UpdateNodeHierarchy(int nodeIndex, const glm::mat4& parentTransform);
};

#endif
