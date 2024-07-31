#include "KalGen2D.hpp"

namespace vwdw {

	KalGen2D::KalGen2D(glm::vec2 scale, float scale_diff, float rot_speed)
	{
		base_scale = scale;
		scale_differential = scale_diff;
		rotation_speed = rot_speed;

		for (auto& color : colors)
		{
			color = glm::mod(color, glm::vec3{ 2.2f });
		}
	}

	vGameObject KalGen2D::next(std::shared_ptr<VModel> model, int i)
	{
		auto triangle = vGameObject::createGameObject();
		triangle.model = model;
		triangle.transform.scale = base_scale + i * 0.005f;
		triangle.transform.rotation = i * glm::pi<float>() * rotation_speed;
		triangle.color = colors[i % colors.size()];
		return triangle;
	}
}