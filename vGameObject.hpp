#pragma once

#include "model.hpp"

#include <memory>

namespace vwdw {

    struct Transform2DComponent {
        glm::vec2 translation{};
        glm::vec2 scale;
        float rotation;
        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);

            glm::mat2 rotMat{ {c,s},{-s,c} };
            glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
            return rotMat * scaleMat;
        
        }
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
        Transform2DComponent transform{};

    private:
        vGameObject(id_t id) : id{ id } {}

        id_t id;
    
    };
}