#pragma once

#include "vGameObject.hpp"
#include "model.hpp"
#include "VDevice.hpp"
#include<glm/glm.hpp>
#include<glm/gtc/constants.hpp>


namespace vwdw {

	class KalGen2D {

	public:
		KalGen2D(glm::vec2 scale, float scale_diff, float rot_speed);

		vGameObject next(std::shared_ptr<VModel> model, int i);

	private:
		std::vector<glm::vec3> colors{
		{1.0f, 5.0f, 5.0f},
		{1.0f, 1.0f, 5.0f},
		{1.0f, 5.0f, 1.0f},
		{5.0f, 1.0f, 5.0f},
		{5.0f, 1.0f, 1.0f},
		{5.0f, 5.0f, 1.0f}
		};

		glm::vec2 base_scale;
		float scale_differential;
		float rotation_speed;
	};

}