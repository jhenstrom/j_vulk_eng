#include "vCamera.hpp"

#include <cassert>
#include <limits>

namespace vwdw {

	void VCamera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
		projection = glm::mat4{ 1.0f };
		projection[0][0] = 2.f / (right - left);
		projection[1][1] = 2.f / (bottom - top);
		projection[2][2] = 1.f / (far - near);
		projection[3][0] = -(right + left) / (right - left);
		projection[3][1] = -(bottom + top) / (bottom - top);
		projection[3][2] = -near / (far - near);
	}

	void VCamera::setPerspectiveProjection(float fov, float aspectRatio, float near, float far) {
		assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);
		const float tanHalfFovy = tan(fov / 2.f);
		projection = glm::mat4{ 0.0f };
		projection[0][0] = 1.f / (aspectRatio * tanHalfFovy);
		projection[1][1] = 1.f / (tanHalfFovy);
		projection[2][2] = far / (far - near);
		projection[2][3] = 1.f;
		projection[3][2] = -(far * near) / (far - near);
	}

    void VCamera::setView(glm::vec3 cameraPos,glm::vec3 direction, glm::vec3 cameraUp) {
        const glm::vec3 w{glm::normalize(direction)};
        const glm::vec3 u{glm::normalize(glm::cross(w, cameraUp))};
        const glm::vec3 v{glm::cross(w, u)};

        view = glm::mat4{1.f};
        view[0][0] = u.x;
        view[1][0] = u.y;
        view[2][0] = u.z;
        view[0][1] = v.x;
        view[1][1] = v.y;
        view[2][1] = v.z;
        view[0][2] = w.x;
        view[1][2] = w.y;
        view[2][2] = w.z;
        view[3][0] = -glm::dot(u, cameraPos);
        view[3][1] = -glm::dot(v, cameraPos);
        view[3][2] = -glm::dot(w, cameraPos);
    }

void VCamera::setViewTarget( glm::vec3 cameraPos, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
  setView(cameraPos, cameraTarget - cameraPos, cameraUp); // should assert that target-pos !=0
}

void VCamera::setViewXYZ(glm::vec3 cameraPos, glm::vec3 rotation){
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
  const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
  const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
  view = glm::mat4{1.f};
  view[0][0] = u.x;
  view[1][0] = u.y;
  view[2][0] = u.z;
  view[0][1] = v.x;
  view[1][1] = v.y;
  view[2][1] = v.z;
  view[0][2] = w.x;
  view[1][2] = w.y;
  view[2][2] = w.z;
  view[3][0] = -glm::dot(u, cameraPos);
  view[3][1] = -glm::dot(v, cameraPos);
  view[3][2] = -glm::dot(w, cameraPos);
}
}