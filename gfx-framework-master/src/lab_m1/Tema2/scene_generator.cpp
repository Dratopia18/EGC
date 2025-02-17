#include "lab_m1/tema2/scene_generator.h"
#include <cstdlib>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

float SceneGenerator::random(glm::vec2 st) const {
    return fract(sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
}

float SceneGenerator::noise(glm::vec2 st) const {
    glm::vec2 i = glm::floor(st);
    glm::vec2 f = st - i; 

    float a = random(i);
    float b = random(i + glm::vec2(1.0f, 0.0f));
    float c = random(i + glm::vec2(0.0f, 1.0f));
    float d = random(i + glm::vec2(1.0f, 1.0f));

    glm::vec2 u = f * f * (glm::vec2(3.0f) - 2.0f * f);

    return glm::mix(a, b, u.x) +
        (c - a) * u.y * (1.0f - u.x) +
        (d - b) * u.x * u.y;
}

float SceneGenerator::getTerrainHeight(float x, float z) const {
    glm::vec2 noisePos = glm::vec2(x, z) * FREQUENCY;
    float noise_value = noise(noisePos);
    return noise_value * AMPLITUDE;
}

bool SceneGenerator::checkOverlap(const glm::vec3& pos, float radius) {
    for (const auto& obstacle : obstacles) {
        float distance = glm::length(obstacle.position - pos);
        if (distance < (obstacle.radius + radius + MIN_DISTANCE)) {
            return true;
        }
    }
    return false;
}

bool SceneGenerator::checkGateOverlap(const glm::vec3& pos, float width) {
    for (const auto& obstacle : obstacles) {
        float distance = glm::length(obstacle.position - pos);
        if (distance < (obstacle.radius + width / 2 + GATE_MIN_DISTANCE)) {
            return true;
        }
    }

    for (const auto& gate : gates) {
        float distance = glm::length(gate.position - pos);
        if (distance < (gate.width + GATE_MIN_DISTANCE)) {
            return true;
        }
    }

    return false;
}

glm::vec3 SceneGenerator::generateRandomPosition() {
    float x = (float)(rand() % (int)(SCENE_SIZE * 2)) - SCENE_SIZE;
    float z = (float)(rand() % (int)(SCENE_SIZE * 2)) - SCENE_SIZE;
    return glm::vec3(x, 0, z);
}

glm::vec3 SceneGenerator::generateGatePosition() {
    const float RESTRICTED_SIZE = 40.0f;
    float x = (float)(rand() % (int)(RESTRICTED_SIZE * 2)) - RESTRICTED_SIZE;
    float z = (float)(rand() % (int)(RESTRICTED_SIZE * 2)) - RESTRICTED_SIZE;
    return glm::vec3(x, 0, z);
}

glm::vec3 SceneGenerator::generateCloudPosition() {
	const float CLOUD_HEIGHT = 10.0f + static_cast<float>(rand()) / RAND_MAX * 30.0f;
	float x = (float)(rand() % (int)(SCENE_SIZE * 2)) - SCENE_SIZE;
	float z = (float)(rand() % (int)(SCENE_SIZE * 2)) - SCENE_SIZE;
	return glm::vec3(x, CLOUD_HEIGHT, z);
}

void SceneGenerator::generateScene() {
    obstacles.clear();
    gates.clear();

    int numBuildings = 20 + (rand() % 3);
    for (int i = 0; i < numBuildings; i++) {
        glm::vec3 pos;
        float radius = 1.5f + static_cast<float>(rand()) / RAND_MAX;

        int attempts = 0;
        do {
            pos = generateRandomPosition();
            glm::vec2 noisePos = glm::vec2(pos.x, pos.z) * FREQUENCY;
            float noise_value = noise(noisePos);
            pos.y = noise_value * AMPLITUDE;
            attempts++;
        } while ((checkOverlap(pos, radius) || isNearDrone(pos)) && attempts < 100);

        if (attempts < 100) {
            obstacles.push_back({ pos, radius, "building" });
        }
    }

    int numTrees = 100 + (rand() % 6);
    int treesPlaced = 0;
    int attempts = 0;

    while (treesPlaced < numTrees && attempts < 1000) {
        glm::vec3 pos = generateRandomPosition();

        glm::vec2 noisePos = glm::vec2(pos.x, pos.z) * FREQUENCY;
        float noise_value = noise(noisePos);
        float terrainHeight = noise_value * AMPLITUDE;

        pos.y = terrainHeight;

        float radius = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.5f;

        if (noise_value > 0.2f && noise_value < 0.8f && !isNearDrone(pos)) {
            bool isValidPlacement = true;

            for (float dx = -0.5f; dx <= 0.5f && isValidPlacement; dx += 0.5f) {
                for (float dz = -0.5f; dz <= 0.5f && isValidPlacement; dz += 0.5f) {
                    glm::vec2 checkPos = glm::vec2(pos.x + dx, pos.z + dz) * FREQUENCY;
                    float checkHeight = noise(checkPos) * AMPLITUDE;

                    if (abs(checkHeight - terrainHeight) > 0.5f) {
                        isValidPlacement = false;
                    }
                }
            }

            if (isValidPlacement && !checkOverlap(pos, radius)) {
                obstacles.push_back({ pos, radius, "tree" });
                treesPlaced++;
            }
        }
        attempts++;
    }

    int numGates = 10;
    for (int i = 0; i < numGates; i++) {
        Gate gate;
        gate.width = 2.0f;
        gate.height = 2.0f;
        gate.isPassed = false;
        gate.sequenceNumber = i;

        bool validPosition = false;
        int attempts = 0;

        while (!validPosition && attempts < 100) {
            glm::vec3 pos = generateGatePosition();
            float terrainHeight = getTerrainHeight(pos.x, pos.z);
            pos.y = terrainHeight + gate.height / 2 + 2.0f;
            gate.position = pos;

            float rotation = static_cast<float>(rand()) / RAND_MAX * glm::two_pi<float>();
            gate.rotation = glm::vec3(0, rotation, 0);
            gate.position = pos;

            validPosition = !checkGateOverlap(pos, gate.width) && !isNearDrone(pos);
            attempts++;
        }

        if (validPosition) {
            gates.push_back(gate);
        }
    }

    std::cout << "\nGate Coordinates:\n";
    for (const auto& gate : gates) {
        std::cout << "Gate " << gate.sequenceNumber << ": ("
            << gate.position.x << ", "
            << gate.position.y << ", "
            << gate.position.z << ")\n";
    }
    std::cout << std::endl;

	int cloudCount = 10;
	for (int i = 0; i < cloudCount; i++) {
		glm::vec3 pos = generateCloudPosition();
		float radius = 1.0f + static_cast<float>(rand()) / RAND_MAX * 1.0f;
		obstacles.push_back({ pos, radius, "cloud" });
	}
}

void SceneGenerator::createTree(Mesh* trunk, Mesh* leaves, const Obstacle& obstacle) {
    std::vector<VertexFormat> trunkVertices;
    std::vector<unsigned int> trunkIndices;

    float height = obstacle.radius * 4.0f;
    float trunkRadius = obstacle.radius * 0.2f;

    const int segments = 10;
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = trunkRadius * cosf(theta);
        float z = trunkRadius * sinf(theta);

        trunkVertices.push_back(VertexFormat(
            glm::vec3(x, 0, z),
            glm::vec3(0.5f, 0.25f, 0),
            glm::vec3(x, 0, z)
        ));

        trunkVertices.push_back(VertexFormat(
            glm::vec3(x, height * 0.4f, z),
            glm::vec3(0.5f, 0.25f, 0),
            glm::vec3(x, 0, z)
        ));
    }

    for (int i = 0; i < segments; i++) {
        trunkIndices.push_back(i * 2);
        trunkIndices.push_back((i + 1) * 2);
        trunkIndices.push_back(i * 2 + 1);

        trunkIndices.push_back((i + 1) * 2);
        trunkIndices.push_back((i + 1) * 2 + 1);
        trunkIndices.push_back(i * 2 + 1);
    }

    trunk->InitFromData(trunkVertices, trunkIndices);

    std::vector<VertexFormat> leavesVertices;
    std::vector<unsigned int> leavesIndices;

    leavesVertices.push_back(VertexFormat(
        glm::vec3(0, height, 0),
        glm::vec3(0, 0.5f, 0),
        glm::vec3(0, 1, 0)
    ));

    float leafRadius = obstacle.radius;
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(segments);
        float x = leafRadius * cosf(theta);
        float z = leafRadius * sinf(theta);

        leavesVertices.push_back(VertexFormat(
            glm::vec3(x, height * 0.4f, z),
            glm::vec3(0, 0.5f, 0),
            glm::vec3(x, -1, z)
        ));
    }

    for (int i = 1; i <= segments; i++) {
        leavesIndices.push_back(0);
        leavesIndices.push_back(i);
        leavesIndices.push_back(i + 1);
    }

    leaves->InitFromData(leavesVertices, leavesIndices);
}

void SceneGenerator::createBuilding(Mesh* building, const Obstacle& obstacle) {
    std::vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(-1, 0, 1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(1, 0, 1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(1, 2, 1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-1, 2, 1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(-1, 0, -1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, -1)),
        VertexFormat(glm::vec3(1, 0, -1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, -1)),
        VertexFormat(glm::vec3(1, 2, -1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, -1)),
        VertexFormat(glm::vec3(-1, 2, -1) * obstacle.radius, glm::vec3(0.7f), glm::vec3(0, 0, -1))
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,    0, 2, 3,  
        4, 5, 6,    4, 6, 7, 
        0, 4, 7,    0, 7, 3,  
        1, 5, 6,    1, 6, 2, 
        3, 2, 6,    3, 6, 7, 
        0, 1, 5,    0, 5, 4   
    };

    building->InitFromData(vertices, indices);
}

void SceneGenerator::createGate(Mesh* frame, Mesh* directionSquare, Mesh* nextSquare, const Gate& gate) {
    float thickness = 0.01f;
    float poleHeight = gate.height;
    float poleWidth = 0.2f;
    float frameThickness = 0.2f;
    float supportHeight = 3.0f;

    std::vector<VertexFormat> frameVertices = {
        VertexFormat(glm::vec3(-gate.width / 2 - poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),  // White color
        VertexFormat(glm::vec3(-gate.width / 2 + poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2 + poleWidth / 2, poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2 - poleWidth / 2, poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(gate.width / 2 - poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 + poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 + poleWidth / 2, poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 - poleWidth / 2, poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(-gate.width / 2, poleHeight / 2 - frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2, poleHeight / 2 - frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2, poleHeight / 2 + frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2, poleHeight / 2 + frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(-gate.width / 2, -poleHeight / 2 - frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2, -poleHeight / 2 - frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2, -poleHeight / 2 + frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2, -poleHeight / 2 + frameThickness, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(-gate.width / 2 - poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2 + poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2 + poleWidth / 2, -(poleHeight / 2 + supportHeight), 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-gate.width / 2 - poleWidth / 2, -(poleHeight / 2 + supportHeight), 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(gate.width / 2 - poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 + poleWidth / 2, -poleHeight / 2, 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 + poleWidth / 2, -(poleHeight / 2 + supportHeight), 0), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(gate.width / 2 - poleWidth / 2, -(poleHeight / 2 + supportHeight), 0), glm::vec3(1.0f), glm::vec3(0, 0, 1))
    };

    float squareSize = gate.width * 0.8f;
    float lineWidth = 0.05f;  

    std::vector<VertexFormat> squareVertices = {
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2 - lineWidth, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2 - lineWidth, 0), glm::vec3(0, 1, 0)),

        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2 + lineWidth, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2 + lineWidth, 0), glm::vec3(0, 1, 0)),

        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, 0), glm::vec3(0, 1, 0)),

        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, squareSize / 2, 0), glm::vec3(0, 1, 0)),

        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2 - lineWidth, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2 - lineWidth, -thickness), glm::vec3(1, 0, 0)),

        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2 + lineWidth, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2 + lineWidth, -thickness), glm::vec3(1, 0, 0)),

        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, -thickness), glm::vec3(1, 0, 0)),

        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, -thickness), glm::vec3(1, 0, 0)),
        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, squareSize / 2, -thickness), glm::vec3(1, 0, 0))
    };

    std::vector<unsigned int> squareIndices;
    for (int i = 0; i < 8; i++) {
        int base = i * 4;
        squareIndices.push_back(base + 0);
        squareIndices.push_back(base + 1);
        squareIndices.push_back(base + 2);
        squareIndices.push_back(base + 0);
        squareIndices.push_back(base + 2);
        squareIndices.push_back(base + 3);
    }

    std::vector<VertexFormat> nextSquareVertices = {

        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2 - lineWidth, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2 - lineWidth, 0), glm::vec3(0, 0, 1)),

        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2 + lineWidth, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2 + lineWidth, 0), glm::vec3(0, 0, 1)),
        
        VertexFormat(glm::vec3(-squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-squareSize / 2 + lineWidth, squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(-squareSize / 2, squareSize / 2, 0), glm::vec3(0, 0, 1)),
        
        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, -squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2, squareSize / 2, 0), glm::vec3(0, 0, 1)),
        VertexFormat(glm::vec3(squareSize / 2 - lineWidth, squareSize / 2, 0), glm::vec3(0, 0, 1))
    };

	std::vector<unsigned int> nextSquareIndices;

	for (int i = 0; i < 4; i++) {
		int base = i * 4;
		nextSquareIndices.push_back(base + 0);
		nextSquareIndices.push_back(base + 1);
		nextSquareIndices.push_back(base + 2);
		nextSquareIndices.push_back(base + 0);
		nextSquareIndices.push_back(base + 2);
		nextSquareIndices.push_back(base + 3);
	}

    std::vector<unsigned int> frameIndices = {
        0, 1, 2,    0, 2, 3,    
        4, 5, 6,    4, 6, 7,    
        8, 9, 10,   8, 10, 11,  
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    frame->InitFromData(frameVertices, frameIndices);
    directionSquare->InitFromData(squareVertices, squareIndices);
	nextSquare->InitFromData(nextSquareVertices, nextSquareIndices);
}

void SceneGenerator::createCloud(Mesh* cloud, const Obstacle& obstacle) {
    std::vector<VertexFormat> cloudVertices;
    std::vector<unsigned int> cloudIndices;

    float radii[3] = { obstacle.radius * 5, obstacle.radius * 4.0f, obstacle.radius * 3.0f };
    glm::vec3 positions[3] = {
        obstacle.position,
        obstacle.position + glm::vec3(obstacle.radius * 4.5f, 0, 0),
        obstacle.position + glm::vec3(-obstacle.radius * 4.0f, 0, 0)
    };

    int slices = 20;
    int stacks = 20;
    unsigned int indexOffset = 0;

    for (int s = 0; s < 3; ++s) {
        float radius = radii[s];
        glm::vec3 position = positions[s];

        for (int i = 0; i <= stacks; i++) {
            float phi = glm::pi<float>() * (float)i / (float)stacks;
            for (int j = 0; j <= slices; j++) {
                float theta = 2.0f * glm::pi<float>() * (float)j / (float)slices;
                float x = cosf(theta) * sinf(phi);
                float y = cosf(phi);
                float z = sinf(theta) * sinf(phi);

                cloudVertices.push_back(VertexFormat(
                    glm::vec3(x, y, z) * radius + position,
                    glm::vec3(1.0f),  
                    glm::vec3(x, y, z)
                ));
            }
        }

        for (int i = 0; i < slices * stacks + slices; i++) {
            cloudIndices.push_back(indexOffset + i);
            cloudIndices.push_back(indexOffset + i + slices + 1);
            cloudIndices.push_back(indexOffset + i + slices);

            cloudIndices.push_back(indexOffset + i + slices + 1);
            cloudIndices.push_back(indexOffset + i);
            cloudIndices.push_back(indexOffset + i + 1);
        }

        indexOffset += (slices + 1) * (stacks + 1);
    }

    cloud->InitFromData(cloudVertices, cloudIndices);
}



bool SceneGenerator::checkGatePass(const glm::vec3& pos, const Gate& gate) const {
    glm::mat4 gateTransform = glm::translate(glm::mat4(1), gate.position) *
        glm::rotate(glm::mat4(1), gate.rotation.y, glm::vec3(0, 1, 0));
    glm::vec4 localPos = glm::inverse(gateTransform) * glm::vec4(pos, 1.0f);

    bool withinBounds = abs(localPos.x) < gate.width / 2 &&
        localPos.y > 0 && localPos.y < gate.height &&
        abs(localPos.z) < 0.5f;

    static float lastZ = 0;
    bool correctDirection = false;

    if (withinBounds) {
        correctDirection = lastZ > 0 && localPos.z < 0;  
        lastZ = localPos.z;
    }

    return withinBounds && correctDirection;
}

const std::vector<Obstacle>& SceneGenerator::getObstacles() const {
    return obstacles;
}
