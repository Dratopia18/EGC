#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace Tank
{
	// Create square with given bottom left corner, length and color
	Mesh* CreateSquare(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateRectangle(const std::string& name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill = false);
	Mesh* CreateTriangle(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateTrapezium(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false, bool flip = false);
	Mesh* CreateArch(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateCircle(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateCannon(const std::string& name, glm::vec3 leftBottomCorner, float length, glm::vec3 color, bool fill = false);
	Mesh* CreateTank(const std::string& name, glm::vec3 center, float bodyLength, float wheelLength, float archLength,
		float cannonLength, float cannonAngle, float healthBarLength, glm::vec3 color, bool fill = false);
}
