#include "Model.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Model::Model(const std::string& filePath) {
    currentAnimation = -1;
    animationTime = 0.0f;
    path = filePath;
    LoadModel(path);
}

Model::~Model() {
    for (auto& mesh : meshes) {
        mesh.vao.Delete();
        mesh.vbo.Delete();
        mesh.ebo.Delete();
        for (auto& texture : mesh.textures) {
            texture.Delete();
        }
    }
}

void Model::LoadModel(const std::string& path) {
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret;
    if (path.ends_with(".glb")) {
        ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);
    } else {
        ret = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path);
    }

    if (!warn.empty()) {
        std::cout << "GLTF warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "GLTF error: " << err << std::endl;
    }

    if (!ret) {
        std::cout << "Failed to load GLTF model: " << path << std::endl;
        return;
    }

    nodes.resize(gltfModel.nodes.size());
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i].meshIndex = -1;
        nodes[i].parent = -1;
        nodes[i].localTransform = glm::mat4(1.0f);
    }

    if (gltfModel.scenes.size() > 0) {
        for (int rootNodeIdx : gltfModel.scenes[0].nodes) {
            ProcessNode(gltfModel, rootNodeIdx, -1);
        }
    }

    ProcessAnimations(gltfModel);
}

void Model::ProcessNode(tinygltf::Model& model, int nodeIndex, int parentIndex) {
    auto& node = model.nodes[nodeIndex];
    
    nodes[nodeIndex].name = node.name;
    nodes[nodeIndex].parent = parentIndex;
    
    if (parentIndex >= 0) {
        nodes[parentIndex].children.push_back(nodeIndex);
    }
    
    if (node.matrix.size() == 16) {
        glm::mat4 matrix;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                matrix[i][j] = static_cast<float>(node.matrix[i * 4 + j]);
            }
        }
        nodes[nodeIndex].localTransform = matrix;
    } else {
        glm::mat4 translation = glm::mat4(1.0f);
        glm::mat4 rotation = glm::mat4(1.0f);
        glm::mat4 scale = glm::mat4(1.0f);
        
        if (node.translation.size() == 3) {
            translation = glm::translate(translation, glm::vec3(
                node.translation[0], node.translation[1], node.translation[2]));
        }
        
        if (node.rotation.size() == 4) {
            glm::quat q = glm::quat(
                static_cast<float>(node.rotation[3]), 
                static_cast<float>(node.rotation[0]),
                static_cast<float>(node.rotation[1]), 
                static_cast<float>(node.rotation[2]));
            rotation = glm::mat4_cast(q);
        }
        
        if (node.scale.size() == 3) {
            scale = glm::scale(scale, glm::vec3(
                node.scale[0], node.scale[1], node.scale[2]));
        }
        
        nodes[nodeIndex].localTransform = translation * rotation * scale;
    }
    
    if (node.mesh >= 0) {
        nodes[nodeIndex].meshIndex = static_cast<int>(meshes.size());
        ProcessMesh(model, node.mesh);
    }
    
    for (int childIndex : node.children) {
        ProcessNode(model, childIndex, nodeIndex);
    }
}

void Model::ProcessMesh(tinygltf::Model& model, int meshIndex) {
    Mesh mesh;
    auto& gltfMesh = model.meshes[meshIndex];
    
    std::vector<float> vertexData;
    std::vector<unsigned int> indices;
    
    for (auto& primitive : gltfMesh.primitives) {
        if (primitive.attributes.find("POSITION") == primitive.attributes.end()) {
            continue;
        }
        
        const auto& posAccessor = model.accessors[primitive.attributes["POSITION"]];
        const auto& posBufferView = model.bufferViews[posAccessor.bufferView];
        const auto& posBuffer = model.buffers[posBufferView.buffer];
        
        int vertCount = static_cast<int>(posAccessor.count);
        
        std::vector<glm::vec3> positions(vertCount);
        std::vector<glm::vec3> colors(vertCount, glm::vec3(1.0f));
        std::vector<glm::vec2> texCoords(vertCount, glm::vec2(0.0f));
        std::vector<glm::vec3> normals(vertCount, glm::vec3(0.0f, 1.0f, 0.0f));
        
        const float* posData = reinterpret_cast<const float*>(
            &posBuffer.data[posBufferView.byteOffset + posAccessor.byteOffset]);
        
        for (int i = 0; i < vertCount; i++) {
            positions[i] = glm::vec3(
                posData[i * 3 + 0],
                posData[i * 3 + 1],
                posData[i * 3 + 2]
            );
        }
        
        if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
            const auto& normalAccessor = model.accessors[primitive.attributes["NORMAL"]];
            const auto& normalBufferView = model.bufferViews[normalAccessor.bufferView];
            const auto& normalBuffer = model.buffers[normalBufferView.buffer];
            
            const float* normalData = reinterpret_cast<const float*>(
                &normalBuffer.data[normalBufferView.byteOffset + normalAccessor.byteOffset]);
            
            for (int i = 0; i < vertCount; i++) {
                normals[i] = glm::vec3(
                    normalData[i * 3 + 0],
                    normalData[i * 3 + 1],
                    normalData[i * 3 + 2]
                );
            }
        }
        
        if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
            const auto& uvAccessor = model.accessors[primitive.attributes["TEXCOORD_0"]];
            const auto& uvBufferView = model.bufferViews[uvAccessor.bufferView];
            const auto& uvBuffer = model.buffers[uvBufferView.buffer];
            
            const float* uvData = reinterpret_cast<const float*>(
                &uvBuffer.data[uvBufferView.byteOffset + uvAccessor.byteOffset]);
            
            for (int i = 0; i < vertCount; i++) {
                texCoords[i] = glm::vec2(
                    uvData[i * 2 + 0],
                    uvData[i * 2 + 1]
                );
            }
        }
        
        if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
            const auto& colorAccessor = model.accessors[primitive.attributes["COLOR_0"]];
            const auto& colorBufferView = model.bufferViews[colorAccessor.bufferView];
            const auto& colorBuffer = model.buffers[colorBufferView.buffer];
            
            const float* colorData = reinterpret_cast<const float*>(
                &colorBuffer.data[colorBufferView.byteOffset + colorAccessor.byteOffset]);
            
            for (int i = 0; i < vertCount; i++) {
                colors[i] = glm::vec3(
                    colorData[i * 3 + 0],
                    colorData[i * 3 + 1],
                    colorData[i * 3 + 2]
                );
            }
        }
        
        if (mesh.textures.empty()) {
            // Ustaw wszystkie kolory na czarne
            for (int i = 0; i < vertCount; i++) {
                colors[i] = glm::vec3(0.0f, 0.0f, 0.0f);
            }
        }
        
        for (int i = 0; i < vertCount; i++) {
            vertexData.push_back(positions[i].x);
            vertexData.push_back(positions[i].y);
            vertexData.push_back(positions[i].z);
            
            vertexData.push_back(colors[i].r);
            vertexData.push_back(colors[i].g);
            vertexData.push_back(colors[i].b);
            
            vertexData.push_back(texCoords[i].s);
            vertexData.push_back(texCoords[i].t);
        }
        
        if (primitive.indices >= 0) {
            const auto& idxAccessor = model.accessors[primitive.indices];
            const auto& idxBufferView = model.bufferViews[idxAccessor.bufferView];
            const auto& idxBuffer = model.buffers[idxBufferView.buffer];
            
            int idxCount = static_cast<int>(idxAccessor.count);
            mesh.indexCount = idxCount;
            
            if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
                const uint16_t* idxData = reinterpret_cast<const uint16_t*>(
                    &idxBuffer.data[idxBufferView.byteOffset + idxAccessor.byteOffset]);
                
                for (int i = 0; i < idxCount; i++) {
                    indices.push_back(static_cast<unsigned int>(idxData[i]));
                }
            } else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
                const uint32_t* idxData = reinterpret_cast<const uint32_t*>(
                    &idxBuffer.data[idxBufferView.byteOffset + idxAccessor.byteOffset]);
                
                for (int i = 0; i < idxCount; i++) {
                    indices.push_back(static_cast<unsigned int>(idxData[i]));
                }
            } else if (idxAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
                const uint8_t* idxData = reinterpret_cast<const uint8_t*>(
                    &idxBuffer.data[idxBufferView.byteOffset + idxAccessor.byteOffset]);
                
                for (int i = 0; i < idxCount; i++) {
                    indices.push_back(static_cast<unsigned int>(idxData[i]));
                }
            }
        }
        
        if (primitive.material >= 0) {
            auto& material = model.materials[primitive.material];
            
            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0) {
                int texIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                int imgIndex = model.textures[texIndex].source;
                
                if (imgIndex >= 0) {
                    auto& gltfImg = model.images[imgIndex];
                    
                    std::string texPath;
                    if (!gltfImg.uri.empty()) {
                        // £aduj z pliku (obecny kod)
                        std::string modelPath = fs::path(path).parent_path().string();
                        texPath = modelPath + "/" + gltfImg.uri;
                        Texture tex(texPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
                        mesh.textures.push_back(tex);
                    } else if (!gltfImg.image.empty()) {
                        // £aduj z surowych danych (raw RGBA8)
                        int width = gltfImg.width;
                        int height = gltfImg.height;
                        int channels = gltfImg.component; // zwykle 4 (RGBA)
                        GLuint texID;
                        glGenTextures(1, &texID);
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, texID);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, gltfImg.image.data());
                        glGenerateMipmap(GL_TEXTURE_2D);
                        glBindTexture(GL_TEXTURE_2D, 0);

                        // Stwórz obiekt Texture tylko z ID (jeœli chcesz zachowaæ spójnoœæ interfejsu)
                        Texture tex;
                        tex.ID = texID;
                        tex.type = GL_TEXTURE_2D;
                        mesh.textures.push_back(tex);
                    }
                }
            }
        }
    }
    
    mesh.vao.Bind();

    if (!vertexData.empty()) {
        mesh.vbo = VBO(reinterpret_cast<GLfloat*>(vertexData.data()), 
                       static_cast<GLsizeiptr>(vertexData.size() * sizeof(float)));
    }

    if (!indices.empty()) {
        mesh.ebo = EBO(reinterpret_cast<GLuint*>(indices.data()), 
                       static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)));
    }
    
    mesh.vao.LinkAttrib(mesh.vbo, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
    mesh.vao.LinkAttrib(mesh.vbo, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    mesh.vao.LinkAttrib(mesh.vbo, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    mesh.vao.Unbind();
    mesh.vbo.Unbind();
    if (!indices.empty()) {
        mesh.ebo.Unbind();
    }
    
    meshes.push_back(mesh);
}

void Model::ProcessAnimations(tinygltf::Model& model) {
    for (auto& gltfAnimation : model.animations) {
        Animation animation;
        animation.name = gltfAnimation.name;
        animation.duration = 0.0f;
        
        for (auto& gltfChannel : gltfAnimation.channels) {
            AnimationChannel channel;
            channel.targetNode = gltfChannel.target_node;
            channel.path = gltfChannel.target_path;
            
            auto& sampler = gltfAnimation.samplers[gltfChannel.sampler];
            
            const auto& timeAccessor = model.accessors[sampler.input];
            const auto& timeBufferView = model.bufferViews[timeAccessor.bufferView];
            const auto& timeBuffer = model.buffers[timeBufferView.buffer];
            
            const float* timeData = reinterpret_cast<const float*>(
                &timeBuffer.data[timeBufferView.byteOffset + timeAccessor.byteOffset]);
            
            for (int i = 0; i < timeAccessor.count; i++) {
                channel.times.push_back(timeData[i]);
                animation.duration = std::max(animation.duration, timeData[i]);
            }
            
            const auto& valueAccessor = model.accessors[sampler.output];
            const auto& valueBufferView = model.bufferViews[valueAccessor.bufferView];
            const auto& valueBuffer = model.buffers[valueBufferView.buffer];
            
            if (channel.path == "translation" || channel.path == "scale") {
                const float* valueData = reinterpret_cast<const float*>(
                    &valueBuffer.data[valueBufferView.byteOffset + valueAccessor.byteOffset]);
                
                for (int i = 0; i < valueAccessor.count; i++) {
                    channel.values.push_back(glm::vec4(
                        valueData[i * 3 + 0],
                        valueData[i * 3 + 1],
                        valueData[i * 3 + 2],
                        0.0f
                    ));
                }
            } 
            else if (channel.path == "rotation") {
                const float* valueData = reinterpret_cast<const float*>(
                    &valueBuffer.data[valueBufferView.byteOffset + valueAccessor.byteOffset]);
                
                for (int i = 0; i < valueAccessor.count; i++) {
                    channel.values.push_back(glm::vec4(
                        valueData[i * 4 + 0],
                        valueData[i * 4 + 1],
                        valueData[i * 4 + 2],
                        valueData[i * 4 + 3]
                    ));
                }
            }
            
            animation.channels.push_back(channel);
        }
        
        animations.push_back(animation);
    }
    
    if (!animations.empty()) {
        currentAnimation = 0;
    }
}

void Model::Draw(Shader& shader) {
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].parent == -1) {
            UpdateNodeHierarchy(i, glm::mat4(1.0f));
        }
    }
    
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i].meshIndex >= 0) {
            auto& mesh = meshes[nodes[i].meshIndex];
            
            glUniformMatrix4fv(
                glGetUniformLocation(shader.ID, "modelMatrix"), 
                1, GL_FALSE, 
                glm::value_ptr(nodes[i].globalTransform)
            );
            
            if (!mesh.textures.empty()) {
                mesh.textures[0].Bind();
            }
            
            mesh.vao.Bind();
            
            if (mesh.indexCount > 0) {
                glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
            } else {
                int vertCount = static_cast<int>(mesh.vbo.GetSize() / (8 * sizeof(float)));
                glDrawArrays(GL_TRIANGLES, 0, vertCount);
            }
            
            mesh.vao.Unbind();
        }
    }
}

void Model::UpdateAnimation(float deltaTime) {
    if (currentAnimation < 0 || currentAnimation >= animations.size()) {
        return;
    }
    
    auto& animation = animations[currentAnimation];
    
    animationTime += deltaTime;
    if (animationTime > animation.duration) {
        animationTime = fmodf(animationTime, animation.duration);
    }
    
    for (auto& channel : animation.channels) {
        int nodeIndex = channel.targetNode;
        if (nodeIndex < 0 || nodeIndex >= nodes.size()) {
            continue;
        }
        
        glm::vec4 value = InterpolateValues(channel.times, channel.values, animationTime);
        
        if (channel.path == "translation") {
            nodes[nodeIndex].localTransform = glm::translate(
                glm::mat4(1.0f), glm::vec3(value.x, value.y, value.z));
        } 
        else if (channel.path == "rotation") {
            glm::quat rotation = glm::quat(value.w, value.x, value.y, value.z);
            nodes[nodeIndex].localTransform = glm::mat4_cast(rotation);
        } 
        else if (channel.path == "scale") {
            nodes[nodeIndex].localTransform = glm::scale(
                glm::mat4(1.0f), glm::vec3(value.x, value.y, value.z));
        }
    }
}

void Model::SetAnimation(int animIndex) {
    if (animIndex >= 0 && animIndex < animations.size()) {
        currentAnimation = animIndex;
        animationTime = 0.0f;
    }
}

glm::vec4 Model::InterpolateValues(const std::vector<float>& times, 
                                 const std::vector<glm::vec4>& values, 
                                 float currentTime) {
    if (times.size() == 1) {
        return values[0];
    }
    
    for (int i = 0; i < times.size() - 1; i++) {
        if (currentTime < times[i + 1]) {
            float t = (currentTime - times[i]) / (times[i + 1] - times[i]);
            return glm::mix(values[i], values[i + 1], t);
        }
    }
    
    return values.back();
}

void Model::UpdateNodeHierarchy(int nodeIndex, const glm::mat4& parentTransform) {
    auto& node = nodes[nodeIndex];
    
    node.globalTransform = parentTransform * node.localTransform;
    
    for (int childIndex : node.children) {
        UpdateNodeHierarchy(childIndex, node.globalTransform);
    }
}
