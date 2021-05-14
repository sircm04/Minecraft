#pragma once

struct Camera
{
	glm::vec3 front = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f)),
		up = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));

	float fov = 70.0f, sensitivity = 0.15f;
};
