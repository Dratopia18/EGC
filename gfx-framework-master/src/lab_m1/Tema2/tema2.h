#pragma once
#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"
#include "lab_m1/tema2/scene_generator.h"

namespace m1
{
    struct CollisionSphere {
        glm::vec3 center;
        float radius;
    };

    struct GateState {
        bool hasPassedGreen;
        glm::vec3 lastPosition;
        float lastZCoord;
    };

    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();
        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;
        void RenderMeshMinimap(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::mat4& viewMatrix);
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        glm::vec4 lastDronePos;

        const float MAX_PITCH_ANGLE = glm::radians(15.0f);  // Maximum pitch up/down
        const float MAX_ROLL_ANGLE = glm::radians(15.0f);   // Maximum roll left/right
        const float ANGLE_CHANGE_SPEED = 2.0f;              // Speed of angle change
        const float THRUST_POWER = 10.0f;                   // Base thrust power
        const float GRAVITY = 9.81f;                        // Gravity constant
        const float AIR_RESISTANCE = 0.5f;                  // Air resistance factor

        struct DronePhysics {
            glm::vec3 velocity = glm::vec3(0);
            float thrust = 0;
        } dronePhysics;

    protected:
        implemented::Camera* camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        float fov;
        float aspectRatio;
        float zNear;
        float zFar;

        struct DroneComponents {
            glm::vec3 position;
            glm::vec3 rotation;
            float scale;
        } drone;

        float propellerAngle;
        float propellerSpeed;
        float droneSpeed;
        float droneAcceleration;
        float rotationSpeed;

        void CreateDrone();
        void RenderDrone();
        void updatePropeller(float deltaTimeSeconds);
        void CreateTerrain(int n, int m);

        SceneGenerator sceneGenerator;
        bool CheckCollisionWithBuilding(const glm::vec3& dronePos, float droneRadius);
        bool CheckCollisionWithTree(const glm::vec3& dronePos, float droneRadius);
        bool CheckCollisionWithTerrain(const glm::vec3& dronePos);
        bool CheckGateCollision(const glm::vec3& pos, const Gate& gate);
        float GetTerrainHeight(float x, float z);

        int currentGateIndex = 0;
        void RenderGates();
        bool CheckGatePass(const glm::vec3& pos, const Gate& gate) const;
        bool isRaceComplete = false;
        std::map<int, GateState> gateStates;

        bool isThirdPerson = false;
        float thirdPersonDistance = 5.0f;

        void CreateArrowIndicator();
        void RenderDirectionArrow();
        Mesh* arrowMesh;

        float gameTimer;
        static constexpr float TIME_LIMIT = 120.0f;
        void CreateRectangleMesh();
        void RenderTimerBar();
		Mesh* rectangleMesh;
        void RenderMinimap();
    };
}   // namespace m1
