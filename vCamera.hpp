#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include<glm/glm.hpp>

namespace vwdw {

class VCamera
{

    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fov, float aspectRatio, float near, float far);

        void setView(glm::vec3 cameraPos,glm::vec3 direction, glm::vec3 cameraUp = glm::vec3(0.f, -1.f, 0.f));
        void setViewTarget( glm::vec3 cameraPos, glm::vec3 cameraTarget, glm::vec3 cameraUp = glm::vec3(0.f, -1.f, 0.f));

        void setViewXYZ(glm::vec3 cameraPos, glm::vec3 rotation);

        const glm::mat4& getProjection() const { return projection; }
        const glm::mat4& getView() const { return view; }
    private:
        glm::mat4 projection{ 1.0f };
        glm::mat4 view{ 1.0f };
};

}