#include "Tank.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

#include <iostream>



Mesh* Tank::CreateSquare(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* Tank::CreateRectangle(const std::string& name,
                      glm::vec3 leftBottomCorner, 
                      float width, float height, 
                      glm::vec3 color, bool fill) 
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(width, 0, 0), color),
        VertexFormat(corner + glm::vec3(width, height, 0), color),
        VertexFormat(corner + glm::vec3(0, height, 0), color)
    };

    Mesh* rectangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        rectangle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    rectangle->InitFromData(vertices, indices);
    return rectangle;
}

Mesh* Tank::CreateTrapezium(const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill,
	bool flip
) {
	glm::vec3 corner = leftBottomCorner;
	float height = length * 0.4f;
    float topAngle = length * 0.1f;
    std::vector<VertexFormat> vertices;

    if (!flip) {
        vertices =
        {
            VertexFormat(corner, color),
            VertexFormat(corner + glm::vec3(length, 0, 0), color),
            VertexFormat(corner + glm::vec3(length - topAngle, height * 0.7f, 0), color),
            VertexFormat(corner + glm::vec3(topAngle, height * 0.7f, 0), color)
        };
    }
    else {
        vertices =
        {
			VertexFormat(corner + glm::vec3(0, height * 0.6f, 0), color),  // ajustam inaltimea
			VertexFormat(corner + glm::vec3(length, height * 0.6f, 0), color),  // ajustam inaltimea
			VertexFormat(corner + glm::vec3(length - topAngle, 0, 0), color),  // il inversam
			VertexFormat(corner + glm::vec3(topAngle, 0, 0), color)  // il inversam
        };

    }

	Mesh* trapezium = new Mesh(name);
	std::vector<unsigned int> indices;

	if (!fill) {
		indices = { 0, 1, 2, 3, 0};
		trapezium->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		// Draw 2 triangles. Add the remaining 2 indices
		indices = { 0, 1, 2, 0, 2, 3 };
	}

	trapezium->InitFromData(vertices, indices);
	return trapezium;
}

Mesh* Tank::CreateArch(const std::string& name,
	glm::vec3 leftBottomCorner,
	float length,
	glm::vec3 color,
	bool fill
) {
	float radius = length / 2;
	float centerX = leftBottomCorner.x + radius;
	float centerY = leftBottomCorner.y + radius;
	int segments = 30;
	float angle = 0;

	std::vector<VertexFormat> vertices;
	std::vector<unsigned int> indices;

	vertices.push_back(VertexFormat(glm::vec3(centerX, centerY, 0), color));

	for (int i = 0; i <= segments; i++) {
		angle = M_PI * i / segments;
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
	}

    if (!fill) {
		for (int i = 0; i <= segments; i++) {
			indices.push_back(i);
		}
		indices.push_back(1);
    }
    else {
		for (int i = 1; i <= segments; i++) {
			indices.push_back(0);
			indices.push_back(i);
			indices.push_back(i + 1);
		}
    }

    Mesh* arch = new Mesh(name);

    if (!fill) {
        arch->SetDrawMode(GL_LINE_STRIP);
	}
	else {
		arch->SetDrawMode(GL_TRIANGLES);
	}

    arch->InitFromData(vertices, indices);
    return arch;
}

Mesh* Tank::CreateCannon(
    const std::string& name,
    glm::vec3 leftCenter,
    float length,
    glm::vec3 color,
    bool fill
) {
    float width = length * 0.15f;

    glm::vec3 bottomLeft = leftCenter + glm::vec3(0, -width / 2, 0);
    glm::vec3 topLeft = leftCenter + glm::vec3(0, width / 2, 0);
    glm::vec3 bottomRight = bottomLeft + glm::vec3(length, 0, 0);
    glm::vec3 topRight = topLeft + glm::vec3(length, 0, 0);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(bottomLeft, color),
        VertexFormat(bottomRight, color),
        VertexFormat(topRight, color),
        VertexFormat(topLeft, color)
    };

    Mesh* cannon = new Mesh(name);
    std::vector<unsigned int> indices;

    if (!fill) {
        indices = { 0, 1, 2, 3, 0 };
        cannon->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices = { 0, 1, 2, 0, 2, 3 };
    }

    cannon->InitFromData(vertices, indices);
    return cannon;
}

Mesh* Tank::CreateCircle(const std::string& name, glm::vec3 center, float radius, glm::vec3 color, bool fill) {
    int segments = 30;
    float angle = 0;

    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(VertexFormat(center, color));

    for (int i = 0; i <= segments; i++) {
        angle = 2 * M_PI * i / segments;
        float x = center.x + radius * cos(angle);
        float y = center.y + radius * sin(angle);
        vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
    }

    if (!fill) {
        for (int i = 0; i <= segments; i++) {
            indices.push_back(i);
        }
        indices.push_back(1);
    }
    else {
        for (int i = 1; i <= segments; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }
    }

    Mesh* circle = new Mesh(name);

    if (!fill) {
        circle->SetDrawMode(GL_LINE_STRIP);
    }
    else {
        circle->SetDrawMode(GL_TRIANGLES);
    }

    circle->InitFromData(vertices, indices);
    return circle;
}

Mesh* Tank::CreateTank(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float bodyLength,
    float wheelLength,
    float archLength,
    float cannonLength,
	float cannonAngle,
    float healthBarLength,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float wheelsWidth = bodyLength * 1.1f;
    float bodyOffsetX = (wheelsWidth - bodyLength) / 2.0f;

    Mesh* body = CreateTrapezium(name + "_body",
        leftBottomCorner + glm::vec3(bodyOffsetX, wheelLength * 0.9, 0), 
        bodyLength, color, fill, false);

    Mesh* wheels = CreateTrapezium(name + "_wheels",
        leftBottomCorner,
        wheelsWidth,
        glm::vec3(color.r * 0.8f, color.g * 0.8f, color.b * 0.8f),
        fill, true);

    glm::vec3 archPosition = leftBottomCorner +
        glm::vec3(bodyOffsetX + bodyLength * 0.3f, wheelLength, 0);

    Mesh* arch = CreateArch(name + "_arch",
        archPosition,
        bodyLength * 0.4f,
        color,
        fill);

    float cannonWidth = bodyLength * 0.5f * 0.15f;

    glm::vec3 cannonBottomLeft = archPosition + glm::vec3(archLength * 0.8f, archLength * 0.8f, 0);

    glm::vec3 cannonRotationPoint = cannonBottomLeft + glm::vec3(0, cannonWidth / 2, 0);

    Mesh* cannon = CreateCannon(name + "_cannon",
        cannonRotationPoint,
        bodyLength * 0.5f,
        glm::vec3(0.2f, 0.2f, 0.2f),
        fill);

    for (auto& vertex : cannon->vertices) {
        glm::vec3 relativePos = vertex.position - cannonRotationPoint;
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), cannonAngle, glm::vec3(0, 0, 1));
        vertex.position = glm::vec3(rotationMatrix * glm::vec4(relativePos, 1.0f)) + cannonRotationPoint;
    }

    float healthBarWidth = bodyLength * 0.8f;
    float healthBarHeight = 10.0f;
    glm::vec3 healthBarPosition = leftBottomCorner + glm::vec3(bodyOffsetX, wheelLength * 4.0f, 0);
    Mesh* healthBar = CreateRectangle(name + "_healthBar", 
        healthBarPosition, 
        healthBarWidth * healthBarLength,
        healthBarHeight, 
        glm::vec3(0.0f, 1.0f, 0.0f), 
        fill);

	Mesh* healthBarFrame = CreateRectangle(name + "_healthBarFrame",
		healthBarPosition,
		healthBarWidth,
		healthBarHeight,
		glm::vec3(0.0f, 0.0f, 0.0f),
		fill);

    unsigned int offset = 0;
    vertices.insert(vertices.end(), body->vertices.begin(), body->vertices.end());
    indices.insert(indices.end(), body->indices.begin(), body->indices.end());

    offset = vertices.size();
    vertices.insert(vertices.end(), wheels->vertices.begin(), wheels->vertices.end());
    for (auto index : wheels->indices) {
        indices.push_back(index + offset);
    }

    offset = vertices.size();
    vertices.insert(vertices.end(), arch->vertices.begin(), arch->vertices.end());
    for (auto index : arch->indices) {
        indices.push_back(index + offset);
    }

    offset = vertices.size();
    vertices.insert(vertices.end(), cannon->vertices.begin(), cannon->vertices.end());
    for (auto index : cannon->indices) {
        indices.push_back(index + offset);
    }

    offset = vertices.size();
    vertices.insert(vertices.end(), healthBar->vertices.begin(), healthBar->vertices.end());
    for (auto index : healthBar->indices) {
        indices.push_back(index + offset);
    }

    Mesh* tank = new Mesh(name);
    if (!fill) {
		tank->SetDrawMode(GL_LINE_LOOP);
    }
	tank->InitFromData(vertices, indices);
    return tank;
}
