#pragma once

#include "model.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace vwdw {

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.f,1.f,1.f};
        glm::vec3 rotation{};
        glm::mat4 mat4();
        glm::mat3 normalMat();
    };

    class vGameObject { //change to either entetity component system or object oriented
    
    public:
        using id_t = unsigned int;

        static vGameObject createGameObject()
        {
            static id_t nextId = 0;
            return vGameObject{ nextId++ };
        }

        vGameObject(const vGameObject&) = delete;
        vGameObject& operator=(const vGameObject&) = delete;
        vGameObject(vGameObject&&) = default;
        vGameObject& operator=(vGameObject&&) = default;


        id_t getId() const { return id; }

        std::shared_ptr<VModel> model{};
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        vGameObject(id_t id) : id{ id } {}

        id_t id;
    
    };
}