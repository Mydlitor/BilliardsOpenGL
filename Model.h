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
    glm::vec4 baseColor = glm::vec4(1.0f);
    Mesh() : indexCount(0) {}
};

struct AnimationChannel {
    std::string path;
    std::vector<float> times;
    std::vector<glm::vec4> values;
    int targetNode = -1; // inicjalizacja
};

struct Animation {
    std::string name;
    float duration = 0.0f; // inicjalizacja
    std::vector<AnimationChannel> channels;
};

struct Node {
    std::string name;
    glm::mat4 localTransform = glm::mat4(1.0f);
    glm::mat4 globalTransform = glm::mat4(1.0f);
    glm::mat4 originalTransform = glm::mat4(1.0f); // Oryginalna transformacja z modelu
    int parent = -1;
    std::vector<int> children;
    int meshIndex = -1;
};

class Model {
public:
    Model(const std::string& path);
    Model(const std::string& path, const glm::mat4& transform);
    ~Model();    void Draw(Shader& shader);
    void UpdateAnimation(float time);
    void TriggerOneShotAnimation();
    bool IsAnimationPlaying() const;
    void SetDoubleSided(bool doubleSided); // Nowa metoda do kontrolowania face culling

private:
    std::string path;
    std::vector<Mesh> meshes;
    std::vector<Node> nodes;    std::vector<Animation> animations;
    std::vector<bool> activeAnimations;
    float animationTime;
    bool animationPlaying;    bool oneShotMode;
    glm::vec4 baseColor = glm::vec4(1.0f);
    glm::mat4 modelTransform = glm::mat4(1.0f); // Dodana transformacja modelu
    bool doubleSided = false; // Flaga kontrolująca face culling
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
