#include "lab_m1/tema2/tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

void Tema2::CreateDrone() {
    {
        Mesh* mesh = new Mesh("drone_body");
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-1.0f, 0.1f, 0.1f), glm::vec3(1), glm::vec3(0, 0, 1)),  
            VertexFormat(glm::vec3(1.0f, 0.1f, 0.1f), glm::vec3(1), glm::vec3(0, 0, 1)),   
            VertexFormat(glm::vec3(1.0f, -0.1f, 0.1f), glm::vec3(1), glm::vec3(0, 0, 1)),  
            VertexFormat(glm::vec3(-1.0f, -0.1f, 0.1f), glm::vec3(1), glm::vec3(0, 0, 1)), 

            VertexFormat(glm::vec3(-1.0f, 0.1f, -0.1f), glm::vec3(1), glm::vec3(0, 0, -1)), 
            VertexFormat(glm::vec3(1.0f, 0.1f, -0.1f), glm::vec3(1), glm::vec3(0, 0, -1)),  
            VertexFormat(glm::vec3(1.0f, -0.1f, -0.1f), glm::vec3(1), glm::vec3(0, 0, -1)), 
            VertexFormat(glm::vec3(-1.0f, -0.1f, -0.1f), glm::vec3(1), glm::vec3(0, 0, -1)), 
        };

        vector<unsigned int> indices = {
            0, 1, 2,    0, 2, 3,    
            4, 5, 6,    4, 6, 7,   
            0, 4, 7,    0, 7, 3,    
            1, 5, 6,    1, 6, 2,    
            0, 1, 5,    0, 5, 4,   
            3, 2, 6,    3, 6, 7,    
        };

        mesh->InitFromData(vertices, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        float cubeSize = 0.15f;
        Mesh* mesh = new Mesh("prop_support");
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-cubeSize, 0, cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(cubeSize, 0, cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(cubeSize, cubeSize * 2, cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-cubeSize, cubeSize * 2, cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, 1)),

            VertexFormat(glm::vec3(-cubeSize, 0, -cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, -1)),
            VertexFormat(glm::vec3(cubeSize, 0, -cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, -1)),
            VertexFormat(glm::vec3(cubeSize, cubeSize * 2, -cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, -1)),
            VertexFormat(glm::vec3(-cubeSize, cubeSize * 2, -cubeSize), glm::vec3(1.0f), glm::vec3(0, 0, -1))
        };

        vector<unsigned int> indices = {
            0, 1, 2,    0, 2, 3,    
            4, 5, 6,    4, 6, 7,    
            0, 4, 7,    0, 7, 3,    
            1, 5, 6,    1, 6, 2,    
            3, 2, 6,    3, 6, 7,    
            0, 1, 5,    0, 5, 4     
        };

        mesh->InitFromData(vertices, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("propeller");
        vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-0.1f, 0.1f, -0.5f), glm::vec3(0.2f), glm::vec3(0.2f)),
            VertexFormat(glm::vec3(0.1f, 0.1f, -0.5f), glm::vec3(0.2f), glm::vec3(0.2f)),
            VertexFormat(glm::vec3(-0.1f, 0.1f, 0.5f), glm::vec3(0.2f), glm::vec3(0.2f)),
            VertexFormat(glm::vec3(0.1f, 0.1f, 0.5f), glm::vec3(0.2f), glm::vec3(0.2f))
        };

        vector<unsigned int> indices = {
            0, 1, 2,    1, 3, 2,
            0, 2, 1,    1, 2, 3
        };

        mesh->InitFromData(vertices, indices);
        meshes[mesh->GetMeshID()] = mesh;
    }
}

bool Tema2::CheckCollisionWithBuilding(const glm::vec3& dronePos, float droneRadius) {
    for (const auto& obstacle : sceneGenerator.getObstacles()) {
        if (obstacle.type == "building") {
            glm::vec2 droneXZ(dronePos.x, dronePos.z);
            glm::vec2 buildingXZ(obstacle.position.x, obstacle.position.z);
            float distance = glm::length(droneXZ - buildingXZ);

            float buildingRadius = obstacle.radius * 1.2f;

            if (distance < (droneRadius + buildingRadius) &&
                dronePos.y < (obstacle.position.y + obstacle.radius * 4)) {
                return true;
            }
        }
    }
    return false;
}

bool Tema2::CheckCollisionWithTree(const glm::vec3& dronePos, float droneRadius) {
    for (const auto& obstacle : sceneGenerator.getObstacles()) {
        if (obstacle.type == "tree") {

            glm::vec2 droneXZ(dronePos.x, dronePos.z);
            glm::vec2 treeXZ(obstacle.position.x, obstacle.position.z);
            float distance = glm::length(droneXZ - treeXZ);

            float trunkRadius = obstacle.radius * 0.2f;
            float treeHeight = obstacle.radius * 4.0f;


            if (distance < (droneRadius + trunkRadius) &&
                dronePos.y < (obstacle.position.y + treeHeight)) {
                return true;
            }


            if (distance < (droneRadius + obstacle.radius * (1.0f - dronePos.y / treeHeight)) &&
                dronePos.y < (obstacle.position.y + treeHeight)) {
                return true;
            }
        }
    }
    return false;
}

bool Tema2::CheckGateCollision(const glm::vec3& pos, const Gate& gate) {

    float droneHalfWidth = 0.5f;
    float droneHalfHeight = 0.5f;
    float droneHalfDepth = 0.5f;
    glm::vec3 droneMin = pos - glm::vec3(droneHalfWidth, droneHalfHeight, droneHalfDepth);
    glm::vec3 droneMax = pos + glm::vec3(droneHalfWidth, droneHalfHeight, droneHalfDepth);

    glm::vec3 gateMin = gate.position - glm::vec3(gate.width / 2, 0, 0.25f);
    glm::vec3 gateMax = gate.position + glm::vec3(gate.width / 2, gate.height, 0.25f);

    bool collisionX = droneMax.x >= gateMin.x && droneMin.x <= gateMax.x;
    bool collisionY = droneMax.y >= gateMin.y && droneMin.y <= gateMax.y;
    bool collisionZ = droneMax.z >= gateMin.z && droneMin.z <= gateMax.z;
    if (collisionX && collisionY && collisionZ) {

        glm::mat4 gateTransform = glm::translate(glm::mat4(1.0f), gate.position) *
            glm::rotate(glm::mat4(1.0f), gate.rotation.y, glm::vec3(0, 1, 0));

        glm::vec4 localPos = glm::inverse(gateTransform) * glm::vec4(pos, 1.0f);

        glm::vec3 droneForward = glm::normalize(glm::vec3(
            sin(drone.rotation.y),
            0,
            cos(drone.rotation.y)
        ));

        glm::vec4 localForward = glm::inverse(gateTransform) * glm::vec4(droneForward, 0.0f);

        if (glm::dot(glm::vec3(localForward), glm::vec3(0, 0, -1)) > 0.5f) {
            return true;
        }
    }
    return false;
}

bool Tema2::CheckGatePass(const glm::vec3& pos, const Gate& gate) const {
    const float GATE_DEPTH = 0.5f;

    glm::mat4 gateTransform = glm::translate(glm::mat4(1.0f), gate.position) *
        glm::rotate(glm::mat4(1.0f), gate.rotation.y, glm::vec3(0, 1, 0));

    glm::vec4 localPos = glm::inverse(gateTransform) * glm::vec4(pos, 1.0f);

    glm::vec3 droneForward = glm::normalize(glm::vec3(
        sin(drone.rotation.y),
        0,
        cos(drone.rotation.y)
    ));

    glm::vec4 localForward = glm::inverse(gateTransform) * glm::vec4(droneForward, 0.0f);

    bool withinXBounds = abs(localPos.x) < gate.width / 2;
    bool withinYBounds = localPos.y > -gate.height && localPos.y < gate.height;
    bool nearGatePlane = abs(localPos.z) < GATE_DEPTH;
    bool withinBounds = withinXBounds && withinYBounds && nearGatePlane;

    float dotProduct = glm::dot(glm::vec3(localForward), glm::vec3(0, 0, -1));
    bool correctDirection = dotProduct > 0.5f;

    if (abs(localPos.z) < GATE_DEPTH * 4) {
        std::cout << "Gate " << gate.sequenceNumber << " check:\n"
            << "  Position relative to gate:\n"
            << "    Left/Right: " << localPos.x << " (must be between " << -gate.width / 2 << " and " << gate.width / 2 << ")\n"
            << "    Up/Down: " << localPos.y << " (must be between " << -gate.height << " and " << gate.height << ")\n"
            << "    Front/Back: " << localPos.z << " (must be between " << -GATE_DEPTH << " and " << GATE_DEPTH << ")\n"
            << "  Direction: " << dotProduct << " (must be > 0.5 to pass through correctly)\n"
            << "  Status: " << (withinXBounds ? "yes" : "no") << " X bounds, "
            << (withinYBounds ? "yes" : "no") << " Y bounds, "
            << (nearGatePlane ? "yes" : "no") << " Z bounds, "
            << (correctDirection ? "yes" : "no") << " direction\n";
    }

    return withinBounds;
}

float fract(float x) { return x - floor(x); }

float Tema2::GetTerrainHeight(float x, float z) {
    float frequency = 0.2f;
    float amplitude = 2.0f;

    glm::vec2 noisePos = glm::vec2(x, z) * frequency;
    float i_x = floor(noisePos.x);
    float i_z = floor(noisePos.y);

    auto random = [](float x, float z) {
        return fract(sin(dot(glm::vec2(x, z), glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
        };

    float a = random(i_x, i_z);
    float b = random(i_x + 1.0f, i_z);
    float c = random(i_x, i_z + 1.0f);
    float d = random(i_x + 1.0f, i_z + 1.0f);

    float f_x = noisePos.x - i_x;
    float f_z = noisePos.y - i_z;

    f_x = f_x * f_x * (3.0f - 2.0f * f_x);
    f_z = f_z * f_z * (3.0f - 2.0f * f_z);

    float height = glm::mix(
        glm::mix(a, b, f_x),
        glm::mix(c, d, f_x),
        f_z
    );

    return height * amplitude;
}

bool Tema2::CheckCollisionWithTerrain(const glm::vec3& dronePos) {
    
    float terrainHeight = GetTerrainHeight(dronePos.x, dronePos.z);

    float collisionThreshold = 0.1f;

    return dronePos.y < (terrainHeight + collisionThreshold);
}

void Tema2::CreateTerrain(int m, int n) {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    float cellWidth = 2.0f;
    float startX = -(m * cellWidth) / 2;  
    float startZ = -(n * cellWidth) / 2;

    for (int i = 0; i <= m; i++) {
        for (int j = 0; j <= n; j++) {
            float x = startX + i * cellWidth;
            float z = startZ + j * cellWidth;

            vertices.push_back(VertexFormat(
                glm::vec3(x, 0, z),
                glm::vec3(0.5f),  
                glm::vec3(0, 1, 0)  
            ));
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            unsigned int topLeft = i * (n + 1) + j;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (i + 1) * (n + 1) + j;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }

    Mesh* terrain = new Mesh("terrain");
    terrain->InitFromData(vertices, indices);
    meshes[terrain->GetMeshID()] = terrain;
}

void Tema2::CreateArrowIndicator() {
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

    const int numSegments = 12;  
    const float radius = 0.2f;   
    const float height = 0.5f;   

    vertices.push_back(VertexFormat(glm::vec3(0, height, 0), glm::vec3(1, 1, 0)));  

    for (int i = 0; i < numSegments; i++) {
        float angle = (2.0f * M_PI * i) / numSegments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back(VertexFormat(glm::vec3(x, 0, z), glm::vec3(0.8f, 0.8f, 0)));
    }

    for (int i = 0; i < numSegments; i++) {
        indices.push_back(0);  
        indices.push_back(1 + i);
        indices.push_back(1 + ((i + 1) % numSegments));
    }

    for (int i = 1; i < numSegments - 1; i++) {
        indices.push_back(1);
        indices.push_back(1 + i);
        indices.push_back(1 + i + 1);
    }

    arrowMesh = new Mesh("direction_arrow");
    arrowMesh->InitFromData(vertices, indices);
    meshes[arrowMesh->GetMeshID()] = arrowMesh;
}

void Tema2::CreateRectangleMesh() {
    vector<VertexFormat> vertices = {
        VertexFormat(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(1, 1, 0), glm::vec3(0, 1, 0)),
        VertexFormat(glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)),
    };

    vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };

    Mesh* mesh = new Mesh("rectangle");
    mesh->InitFromData(vertices, indices);
    meshes[mesh->GetMeshID()] = mesh;
}



void Tema2::Init()
{
    renderCameraTarget = false;

    camera = new implemented::Camera();

    fov = 60;
    aspectRatio = window->props.aspectRatio;
    zNear = 0.01f;
    zFar = 200.0f;

    projectionMatrix = glm::perspective(RADIANS(fov), aspectRatio, zNear, zFar);

    drone.position = glm::vec3(0, 2, 0);
    drone.rotation = glm::vec3(0);
    drone.scale = 0.5f;

    propellerAngle = 0;
    propellerSpeed = 50.0f;
    droneSpeed = 5.0f;
    rotationSpeed = 2.0f;

    glm::vec3 cameraOffset = glm::vec3(0, 0.3f, 0);
    camera->Set(drone.position + cameraOffset,
        drone.position + cameraOffset + glm::vec3(0, 0, -1),
        glm::vec3(0, 1, 0));

    sceneGenerator.generateScene();

    for (const auto& obstacle : sceneGenerator.getObstacles()) {
        if (obstacle.type == "tree") {
            std::string trunkId = "trunk_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
            std::string leavesId = "leaves_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);

            Mesh* trunk = new Mesh(trunkId);
            Mesh* leaves = new Mesh(leavesId);
            sceneGenerator.createTree(trunk, leaves, obstacle);
            meshes[trunk->GetMeshID()] = trunk;
            meshes[leaves->GetMeshID()] = leaves;
        }
        else if (obstacle.type == "building") {
            std::string buildingId = "building_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
            Mesh* building = new Mesh(buildingId);
            sceneGenerator.createBuilding(building, obstacle);
            meshes[building->GetMeshID()] = building;
        }
        else {
			std::string cloudId = "cloud_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
			Mesh* cloud = new Mesh(cloudId);
			sceneGenerator.createCloud(cloud, obstacle);
			meshes[cloud->GetMeshID()] = cloud;
        }
    }
    CreateTerrain(50, 50);
    
    Shader* shader = new Shader("TerrainShader");
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
    shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
    shader->CreateAndLink();
    shaders[shader->GetName()] = shader;

    const auto& gates = sceneGenerator.getGates();
    for (const auto& gate : gates) {
        std::string frameId = "gate_frame_" + std::to_string(gate.sequenceNumber);
        std::string squareId = "gate_square_" + std::to_string(gate.sequenceNumber);
		std::string blueSquareId = "gate_square_blue_" + std::to_string(gate.sequenceNumber);

        Mesh* frame = new Mesh(frameId);
        Mesh* square = new Mesh(squareId);
		Mesh* blueSquare = new Mesh(blueSquareId);

        sceneGenerator.createGate(frame, square, blueSquare, gate);

        meshes[frame->GetMeshID()] = frame;
        meshes[square->GetMeshID()] = square;
		meshes[blueSquare->GetMeshID()] = blueSquare;
    }

    currentGateIndex = 0;
    isRaceComplete = false;

    CreateArrowIndicator();
    CreateRectangleMesh();
    gameTimer = TIME_LIMIT;

    CreateDrone();
}


void Tema2::FrameStart()
{
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema2::RenderDrone() {
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, drone.position);

    modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));

    modelMatrix = glm::rotate(modelMatrix, drone.rotation.y, glm::vec3(0, 1, 0)); 

    modelMatrix = glm::scale(modelMatrix, glm::vec3(drone.scale));

    RenderMesh(meshes["drone_body"], shaders["VertexColor"], modelMatrix);

    glm::mat4 secondArmMatrix = modelMatrix;
    secondArmMatrix = glm::rotate(secondArmMatrix, RADIANS(90.0f), glm::vec3(0, 1, 0));
    RenderMesh(meshes["drone_body"], shaders["VertexColor"], secondArmMatrix);

    glm::vec3 propellerPositions[] = {
        glm::vec3(-1.0f, 0, 0),
        glm::vec3(1.0f, 0, 0),
        glm::vec3(0, 0, -1.0f),
        glm::vec3(0, 0, 1.0f)
    };

    for (const auto& pos : propellerPositions) {
        glm::mat4 supportMatrix = modelMatrix;
        supportMatrix = glm::translate(supportMatrix, pos);
        RenderMesh(meshes["prop_support"], shaders["VertexColor"], supportMatrix);

        glm::mat4 propellerMatrix = supportMatrix;
        propellerMatrix = glm::translate(propellerMatrix, glm::vec3(0, 0.23f, 0));
        propellerMatrix = glm::rotate(propellerMatrix, propellerAngle, glm::vec3(0, 1, 0));
        RenderMesh(meshes["propeller"], shaders["VertexColor"], propellerMatrix);
    }
}

void Tema2::updatePropeller(float deltaTimeSeconds) {
    propellerAngle += propellerSpeed * deltaTimeSeconds;
}

void Tema2::RenderGates() {
    const auto& gates = sceneGenerator.getGates();

    for (const auto& gate : gates) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, gate.position);
        modelMatrix = glm::rotate(modelMatrix, gate.rotation.y, glm::vec3(0, 1, 0));

        std::string frameId = "gate_frame_" + std::to_string(gate.sequenceNumber);
        RenderMesh(meshes[frameId], shaders["VertexColor"], modelMatrix);

        if (gate.sequenceNumber == currentGateIndex && !isRaceComplete) {
            std::string squareId = "gate_square_" + std::to_string(gate.sequenceNumber);
            RenderMesh(meshes[squareId], shaders["VertexColor"], modelMatrix);
        }

		else if (gate.sequenceNumber == currentGateIndex + 1 && !isRaceComplete) {
			std::string blueSquareId = "gate_square_blue_" + std::to_string(gate.sequenceNumber);
			RenderMesh(meshes[blueSquareId], shaders["VertexColor"], modelMatrix);
		}
    }
}

void Tema2::RenderDirectionArrow() {
    if (isRaceComplete || currentGateIndex >= sceneGenerator.getGates().size()) {
        return;
    }

    const auto& gates = sceneGenerator.getGates();
    const Gate& currentGate = gates[currentGateIndex];

    glm::ivec2 resolution = window->GetResolution();

    float screenX = resolution.x * 0.5f;   
    float screenY = resolution.y * 0.85f; 

    glm::vec3 toGate = currentGate.position - drone.position;

    glm::vec2 direction2D = glm::normalize(glm::vec2(toGate.x, toGate.z));

    glm::vec2 droneForward = glm::normalize(glm::vec2(
        sin(-drone.rotation.y),
        -cos(-drone.rotation.y)
    ));

    float angle = atan2(direction2D.x, direction2D.y) - atan2(droneForward.x, droneForward.y);

    if (angle > M_PI) angle -= 2 * M_PI;
    if (angle < -M_PI) angle += 2 * M_PI;

    glm::mat4 modelMatrix = glm::mat4(1);

    modelMatrix = glm::translate(modelMatrix, glm::vec3(screenX, screenY, 0));

    modelMatrix = glm::rotate(modelMatrix, angle, glm::vec3(0, 0, 1));

    float arrowScale = 40.0f;
    modelMatrix = glm::scale(modelMatrix, glm::vec3(arrowScale));

    glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y, -100.0f, 100.0f);

    glDisable(GL_DEPTH_TEST);

    shaders["VertexColor"]->Use();
    glUniformMatrix4fv(shaders["VertexColor"]->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(shaders["VertexColor"]->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shaders["VertexColor"]->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    arrowMesh->Render();

    glEnable(GL_DEPTH_TEST);
}

void Tema2::RenderTimerBar() {
    if (isRaceComplete || currentGateIndex >= sceneGenerator.getGates().size()) {
        return;
    }

    glm::ivec2 resolution = window->GetResolution();

    glm::mat4 savedProj = projectionMatrix;

    float barHeight = resolution.y * 0.03f;
    float barWidth = resolution.x * 0.4f;
    float barX = (resolution.x - barWidth) / 2;
    float barY = resolution.y * 0.1f;

    float currentWidth = glm::max((gameTimer / TIME_LIMIT) * barWidth, 0.0f);

    glDisable(GL_DEPTH_TEST);

    glm::mat4 uiProjection = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(barX, barY, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(currentWidth, barHeight, 1));

    shaders["VertexColor"]->Use();
    glUniformMatrix4fv(shaders["VertexColor"]->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glUniformMatrix4fv(shaders["VertexColor"]->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(uiProjection));
    glUniformMatrix4fv(shaders["VertexColor"]->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	meshes["rectangle"]->Render();

    glEnable(GL_DEPTH_TEST);

    projectionMatrix = savedProj;
}


void Tema2::RenderMeshMinimap(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix) {
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderMinimap() {
    glm::ivec2 resolution = window->GetResolution();
    glm::mat4 savedProj = projectionMatrix;

    glClear(GL_DEPTH_BUFFER_BIT);

    int minimapSize = resolution.x / 5;
    glViewport(resolution.x - minimapSize - 10, resolution.y - minimapSize - 10,
        minimapSize, minimapSize);

    float cameraHeight = 100.0f;
    glm::vec3 eye = drone.position + glm::vec3(0, cameraHeight, 0);
    glm::vec3 center = drone.position;
    glm::vec3 up = glm::vec3(0, 0, 1);
    glm::mat4 minimapView = glm::lookAt(eye, center, up);

    float mapSize = 30.0f;
    projectionMatrix = glm::ortho(
        -mapSize, mapSize,
        -mapSize, mapSize,
        0.1f, cameraHeight * 2.0f
    );

    glm::mat4 modelMatrix = glm::mat4(1);

    RenderMeshMinimap(meshes["terrain"], shaders["TerrainShader"], modelMatrix, minimapView);

    const auto& gates = sceneGenerator.getGates();
    for (const auto& gate : gates) {
        glm::mat4 gateMatrix = glm::translate(glm::mat4(1), gate.position);
        gateMatrix = glm::rotate(gateMatrix, gate.rotation.y, glm::vec3(0, 1, 0));

        glLineWidth(2.0f);

        std::string gateLineId = "gate_line_" + std::to_string(gate.sequenceNumber);

        glm::vec3 color;
        if (gate.sequenceNumber == currentGateIndex) {
            color = glm::vec3(0, 1, 0);
        }
        else if (gate.sequenceNumber == currentGateIndex + 1) {
            color = glm::vec3(1, 1, 0);
        }
        else {
            color = glm::vec3(1, 1, 1);
        }

        float width = gate.width;
        float height = gate.height;

        std::vector<VertexFormat> vertices = {
            VertexFormat(glm::vec3(-width / 2, 0, 0), color),      
            VertexFormat(glm::vec3(width / 2, 0, 0), color),       
            VertexFormat(glm::vec3(width / 2, height, 0), color),  
            VertexFormat(glm::vec3(-width / 2, height, 0), color)
        };

        std::vector<unsigned int> indices = { 0, 1, 2, 3, 0 };

        if (meshes.find(gateLineId) == meshes.end()) {
            meshes[gateLineId] = new Mesh(gateLineId);
        }
        meshes[gateLineId]->SetDrawMode(GL_LINE_STRIP);
        meshes[gateLineId]->InitFromData(vertices, indices);

        RenderMeshMinimap(meshes[gateLineId], shaders["VertexColor"], gateMatrix, minimapView);
    }

    for (const auto& obstacle : sceneGenerator.getObstacles()) {
        glm::mat4 obstacleMatrix = glm::translate(glm::mat4(1), obstacle.position);

        if (obstacle.type == "tree") {
            std::string leavesId = "leaves_" + std::to_string(obstacle.position.x) + "_" +
                std::to_string(obstacle.position.z);
            RenderMeshMinimap(meshes[leavesId], shaders["VertexColor"], obstacleMatrix, minimapView);
        }
        else {
            std::string buildingId = "building_" + std::to_string(obstacle.position.x) + "_" +
                std::to_string(obstacle.position.z);
            RenderMeshMinimap(meshes[buildingId], shaders["VertexColor"], obstacleMatrix, minimapView);
        }
    }

    glm::mat4 droneMatrix = glm::mat4(1);
    droneMatrix = glm::translate(droneMatrix, drone.position + glm::vec3(0, 0.5f, 0));
    droneMatrix = glm::rotate(droneMatrix, drone.rotation.y, glm::vec3(0, 1, 0));
    droneMatrix = glm::scale(droneMatrix, glm::vec3(2.0f));
    RenderMeshMinimap(arrowMesh, shaders["VertexColor"], droneMatrix, minimapView);

    glLineWidth(1.0f);

    glViewport(0, 0, resolution.x, resolution.y);
    projectionMatrix = savedProj;
}

void Tema2::Update(float deltaTimeSeconds) {
    glm::vec3 forward = glm::vec3(
        sin(-drone.rotation.y),
        0,
        -cos(-drone.rotation.y)
    );

    if (isThirdPerson) {
        glm::vec3 cameraOffset = glm::vec3(0, 2.0f, 0);
        glm::vec3 backwardOffset = -forward * thirdPersonDistance;
        camera->position = drone.position + cameraOffset + backwardOffset;
    }
    else {
        glm::vec3 cameraOffset = glm::vec3(0, 0.3f, 0);
        camera->position = drone.position + cameraOffset;
    }

    camera->forward = glm::normalize(forward);
    camera->right = glm::normalize(glm::cross(camera->forward, glm::vec3(0, 1, 0)));
    camera->up = glm::normalize(glm::cross(camera->right, camera->forward));

    if (isThirdPerson) {
        camera->Set(
            camera->position,
            drone.position + glm::vec3(0, 1.0f, 0), 
            camera->up
        );
    }
    else {
        camera->Set(
            camera->position,
            camera->position + camera->forward,
            camera->up
        );
    }

    if (!isRaceComplete) {
        const auto& gates = sceneGenerator.getGates();
        if (currentGateIndex < gates.size()) {
            const Gate& currentGate = gates[currentGateIndex];

            glm::mat4 gateTransform = glm::translate(glm::mat4(1), currentGate.position) *
                glm::rotate(glm::mat4(1), currentGate.rotation.y, glm::vec3(0, 1, 0));
            glm::vec4 localPos = glm::inverse(gateTransform) * glm::vec4(drone.position, 1.0f);

            if (CheckGatePass(drone.position, currentGate)) {
                std::cout << "Gate " << currentGateIndex + 1 << " passed!" << std::endl;

                std::string squareId = "gate_square_" + std::to_string(currentGate.sequenceNumber);
                if (meshes.find(squareId) != meshes.end()) {
                    meshes.erase(squareId);
                }

                currentGateIndex++;

                if (currentGateIndex >= gates.size()) {
                    std::cout << "Race Complete!" << std::endl;
                    isRaceComplete = true;
                }
            }
        }
        gameTimer -= deltaTimeSeconds;

        if (gameTimer <= 0) {
            std::cout << "Time's up! Game Over!" << std::endl;
            isRaceComplete = true;
        }
    }

    for (const auto& obstacle : sceneGenerator.getObstacles()) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, obstacle.position);

        if (obstacle.type == "tree") {
            std::string trunkId = "trunk_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
            std::string leavesId = "leaves_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);

            RenderMesh(meshes[trunkId], shaders["VertexColor"], modelMatrix);
            RenderMesh(meshes[leavesId], shaders["VertexColor"], modelMatrix);
        }
        else if (obstacle.type == "building") {
            std::string buildingId = "building_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
            RenderMesh(meshes[buildingId], shaders["VertexColor"], modelMatrix);
        }
        else {
			std::string cloudId = "cloud_" + std::to_string(obstacle.position.x) + "_" + std::to_string(obstacle.position.z);
			RenderMesh(meshes[cloudId], shaders["VertexColor"], modelMatrix);
        }
    }
    glm::mat4 modelMatrix = glm::mat4(1);
    RenderMesh(meshes["terrain"], shaders["TerrainShader"], modelMatrix);

	RenderGates();

    updatePropeller(deltaTimeSeconds);
    RenderDrone();

    glm::mat4 savedProj = projectionMatrix;
    glm::mat4 savedView = camera->GetViewMatrix();

    glm::ivec2 resolution = window->GetResolution();
    glm::mat4 uiProjection = glm::ortho(0.0f, (float)resolution.x, 0.0f, (float)resolution.y);

    glDisable(GL_DEPTH_TEST);

    RenderTimerBar();
    RenderDirectionArrow();

    projectionMatrix = savedProj;
    glEnable(GL_DEPTH_TEST);

    RenderMinimap();
}


void Tema2::FrameEnd()
{
    
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
        glm::vec3 newPosition = drone.position;
        float droneRadius = 0.2f * drone.scale;

        glm::vec3 forward = glm::normalize(glm::vec3(sin(-drone.rotation.y), 0, -cos(-drone.rotation.y)));
        glm::vec3 right = glm::normalize(glm::vec3(cos(-drone.rotation.y), 0, sin(-drone.rotation.y)));

        if (window->KeyHold(GLFW_KEY_W)) {
            newPosition += forward * droneSpeed * deltaTime * 2.0f;
        }
        if (window->KeyHold(GLFW_KEY_S)) {
            newPosition -= forward * droneSpeed * deltaTime * 2.0f;
        }
        if (window->KeyHold(GLFW_KEY_D)) {
            newPosition += right * droneSpeed * deltaTime * 2.0f;
        }
        if (window->KeyHold(GLFW_KEY_A)) {
            newPosition -= right * droneSpeed * deltaTime * 2.0f;
        }

        if (window->KeyHold(GLFW_KEY_LEFT_SHIFT)) {
            newPosition.y += droneSpeed * deltaTime * 2.0f;
        }
        if (window->KeyHold(GLFW_KEY_LEFT_CONTROL)) {
            newPosition.y -= droneSpeed * deltaTime * 2.0f;
        }
        
        if (window->KeyHold(GLFW_KEY_Q)) {
            drone.rotation.y += rotationSpeed * deltaTime;
        }
        if (window->KeyHold(GLFW_KEY_E)) {
            drone.rotation.y -= rotationSpeed * deltaTime;
        }

        newPosition.x = glm::clamp(newPosition.x, -50.0f, 50.0f); 
        newPosition.z = glm::clamp(newPosition.z, -50.0f, 50.0f); 
        newPosition.y = glm::clamp(newPosition.y, 0.0f, 50.0f);

        bool collision = CheckCollisionWithBuilding(newPosition, droneRadius) ||
            CheckCollisionWithTree(newPosition, droneRadius) ||
            CheckCollisionWithTerrain(newPosition);

        const auto& gates = sceneGenerator.getGates();
        for (const auto& gate : gates) {
            if (CheckGateCollision(newPosition, gate)) {
                collision = true;
                break;
            }
        }

        if (!collision) {
            drone.position = newPosition;
        }
        else {
            glm::vec3 terrainHeight = newPosition;
            terrainHeight.y = GetTerrainHeight(newPosition.x, newPosition.z) + droneRadius + 0.1f;

            if (CheckCollisionWithTerrain(newPosition) &&
                !CheckCollisionWithBuilding(terrainHeight, droneRadius) &&
                !CheckCollisionWithTree(terrainHeight, droneRadius)) {
                drone.position = terrainHeight;
            }
        }
    }
}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

    if (key == GLFW_KEY_P) {
        if (projectionMatrix == glm::perspective(RADIANS(fov), aspectRatio, zNear, zFar)) {
            projectionMatrix = glm::ortho(-window->props.aspectRatio, window->props.aspectRatio, -1.0f, 1.0f, 0.01f, 200.0f);
        }
    }

    if (key == GLFW_KEY_O) {
        projectionMatrix = glm::perspective(RADIANS(fov), aspectRatio, zNear, zFar);
    }

    if (key == GLFW_KEY_K) {
        isThirdPerson = !isThirdPerson;  
    }
}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
            camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(-deltaY * sensivityOY);
            camera->RotateThirdPerson_OY(-deltaX * sensivityOX);
        }
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}
