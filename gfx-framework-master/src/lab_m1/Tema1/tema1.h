#pragma once

#include <components/simple_scene.h>

namespace m1 
{
    class Projectile {
    private:
        glm::vec3 position;
        glm::vec3 velocity;
		float gravity = 1600.0f; // acceleratia gravitationala
        Mesh* mesh;
        std::vector<glm::vec3> trajectory;

    public:
        Projectile(glm::vec3 startPos, float angle, float initialVelocity, Mesh* projectileMesh)
            : position(startPos)
            , mesh(projectileMesh)
        {
			// initializam viteza proiectilului
            velocity.x = initialVelocity * cos(angle);
            velocity.y = initialVelocity * sin(angle);
            velocity.z = 0.0f;

            trajectory.push_back(position);
        }

        void Update(float deltaTimeSeconds) {
			// setam pozitia proiectilului
            trajectory.push_back(position);

			// actualizam pozitia proiectilului
            position += velocity * deltaTimeSeconds;

			// actualizam viteza proiectilului pe baza acceleratiei gravitationale
            velocity.y -= gravity * deltaTimeSeconds;
        }

        glm::vec3 GetPosition() const { return position; }
        Mesh* GetMesh() const { return mesh; }
        const std::vector<glm::vec3>& GetTrajectory() const { return trajectory; }
    };

    class Tema1 : public gfxc::SimpleScene {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void UpdateTankPositionOnTerrain();
		bool CheckCollision(glm::vec3 position, float radius);
		void DrawTrajectoryView(glm::vec3 startPos, float angle, float initialVelocity, glm::vec3 color);
        void RenderTank(float X);
        void RenderTank2(float X);
        bool CheckTankCollision(glm::vec3 projectilePos, glm::vec3 tankPos, float tankRotation, float cannonAngle);
        void UpdateTankHealth(std::string tankName);

    protected:
        std::vector<float> heightMap;
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;
        std::vector<VertexFormat> quadVertices;
        std::vector<unsigned int> quadIndices;
        Mesh* quadMesh;
		glm::vec3 TankPosition;
        glm::vec3 TankPosition2;
		float speed;
		float gravity;
		float verticalSpeed;
		float step;
		float tankRotationAngle;
		float tankRotationAngle2;
		std::vector<Projectile> projectiles;
		float cannonAngle = M_PI / 2;
        float cannonAngle2 = M_PI / 2;
		bool cannonRotated = false;
        bool cannonRotated2 = false;
		float collisionThreshold = 5.0f;
        Mesh* trajectoryMesh;
        Mesh* trajectoryMesh2;
        bool showTrajectory;
        float x;
        float y;
        float tank1Health = 100.0f;
        float tank2Health = 100.0f;
        bool cannonHasMoved;
		float lastCannonAngle;
		float lastCannonAngle2;
    };
}