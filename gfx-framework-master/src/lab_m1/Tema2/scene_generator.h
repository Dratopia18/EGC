#pragma once
#include "utils/glm_utils.h"
#include <vector>
#include "core/gpu/mesh.h"

struct Position {
    float x;
    float y;
    float z;
};

struct Obstacle {
    glm::vec3 position;
    float radius;
    std::string type;
};

struct Gate {
    glm::vec3 position;
    glm::vec3 rotation;  // Orientation of the gate
    float width;
    float height;
    bool isPassed;
    int sequenceNumber;
};

class SceneGenerator {
private:
    std::vector<Obstacle> obstacles;
    std::vector<Gate> gates;
    const float SCENE_SIZE = 50.0f;
    const float MIN_DISTANCE = 2.0f;
    const float GATE_MIN_DISTANCE = 5.0f;
    const float FREQUENCY = 0.2f;    // Match vertex shader
    const float AMPLITUDE = 2.0f;    // Match vertex shader
    const glm::vec3 DRONE_START_POS = glm::vec3(0, 2, 0);
    const float DRONE_SAFETY_RADIUS = 1.0f;  // Safe zone around drone

    bool isNearDrone(const glm::vec3& pos) const {
        float distance = glm::length(glm::vec2(pos.x - DRONE_START_POS.x, pos.z - DRONE_START_POS.z));
        return distance < DRONE_SAFETY_RADIUS;
    }

    inline float fract(float x) const { return x - std::floor(x); }
    float random(glm::vec2 st) const;
    float noise(glm::vec2 st) const;
    float getTerrainHeight(float x, float z) const;
    bool checkOverlap(const glm::vec3& pos, float radius);
    bool checkGateOverlap(const glm::vec3& pos, float width);
    glm::vec3 generateRandomPosition();
    glm::vec3 generateGatePosition();
    glm::vec3 generateCloudPosition();

public:
    void generateScene();
    void createTree(Mesh* trunk, Mesh* leaves, const Obstacle& obstacle);
    void createBuilding(Mesh* building, const Obstacle& obstacle);
	void createCloud(Mesh* cloud, const Obstacle& obstacle);
    void updateCloudPositions(float deltaTimeSeconds, float cloudSpeed);
    void createGate(Mesh* frame, Mesh* directionSquare, Mesh* nextSquare, const Gate& gate);
    const std::vector<Obstacle>& getObstacles() const;
    const std::vector<Gate>& getGates() const { return gates; }
    bool checkGatePass(const glm::vec3& pos, const Gate& gate) const;
};