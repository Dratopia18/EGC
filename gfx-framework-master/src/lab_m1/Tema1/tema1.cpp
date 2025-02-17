#include <lab_m1/Tema1/tema1.h>
#include <lab_m1/Tema1/Tank.h>
#include <lab_m1/Tema1/transform2D.h>

#include <core/engine.h>
#include <utils/gl_utils.h>

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
	// stergem toate obiectele create
	heightMap.clear();
	quadVertices.clear();
	quadIndices.clear();
	projectiles.clear();
    for (auto& mesh : meshes) {
        delete mesh.second;
    }
    meshes.clear();
}

void Tema1::Init() {
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    std::vector<glm::vec2> vertices;

	float xStart = 0.0f; // terenul incepe de la 0
	float xEnd = 1920.0f; // terenul se termina la 1920
	step = 7.5f; // distanta intre punctele de pe teren
	float heightScale = 360.0f; // inaltimea maxima a terenului

    for (float x = xStart; x <= xEnd; x += step) {
        float y = static_cast<float>(abs(sin(0.005f * x) + 0.6f * sin(0.01f * x) + 0.3f * sin(0.02f * x)));
		// adaugam inaltimea terenului la vectorul de inaltimi
		y = (y + 1.0f) * 0.5f;  // normalizam inaltimea la intervalul [0, 1]
        heightMap.push_back(y * heightScale);
        vertices.push_back(glm::vec2(x, y * heightScale));
    }

	// aplicam un filtru de smoothing pentru a face terenul mai natural
    std::vector<float> smoothedHeights = heightMap;
	int smoothingPasses = 3;  // numarul de pasi de smoothing
	float sigma = 1.5f;      // deviatia standard a filtrului Gaussian

    for (int pass = 0; pass < smoothingPasses; pass++) {
        for (size_t i = 2; i < heightMap.size() - 2; i++) {
            float sum = 0.0f;
            float weightSum = 0.0f;

			// aplicam un filtru Gaussian pentru smoothing
            for (int j = -2; j <= 2; j++) {
                float weight = exp(-(j * j) / (2 * sigma * sigma));
                sum += heightMap[i + j] * weight;
                weightSum += weight;
            }

            smoothedHeights[i] = sum / weightSum;
        }
        heightMap = smoothedHeights;
    }

	// creem un mesh pentru teren
    for (size_t i = 0; i < heightMap.size() - 1; ++i) {
		float x1 = i * step; // primul punct de pe teren
		float y1 = heightMap[i]; // inaltimea primului punct
		float x2 = (i + 1) * step; // al doilea punct de pe teren
		float y2 = heightMap[i + 1]; // inaltimea celui de-al doilea punct

		// Adaugam cele 4 puncte pentru fiecare dreptunghi
        quadVertices.push_back(VertexFormat(glm::vec3(x1, y1, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x2, y2, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x2, 0, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x1, 0, 0), glm::vec3(0.59f, 0.29f, 0.0f)));

		// Adaugam indicii pentru cele 2 triunghiuri din dreptunghi
        quadIndices.push_back(i * 4 + 0);
        quadIndices.push_back(i * 4 + 1);
        quadIndices.push_back(i * 4 + 2);
        quadIndices.push_back(i * 4 + 0);
        quadIndices.push_back(i * 4 + 2);
        quadIndices.push_back(i * 4 + 3);
    }

	// initializam mesh-ul pentru teren
    quadMesh = new Mesh("quad");
    quadMesh->InitFromData(quadVertices, quadIndices);
    AddMeshToList(quadMesh);

	//creem un tanc
    TankPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	float bodyLength = 70.0f;      // marimea bazei
	float wheelLength = 20.0f;      // marimea sectiunii roatei
	float archLength = 20.0f;       // marimea domului tunului
	float cannonLength = 25.0f;    // marinmea tunului
	float healthBar = 1.0f; // 100% viata
	glm::vec3 color(0.87f, 0.72f, 0.53f);  // culoare maro deschis
    bool fill = true;
    
	// adaugam tancul in lista de obiecte
    Mesh* tank = Tank::CreateTank("tank1", TankPosition, bodyLength, wheelLength,
        archLength, cannonLength, cannonAngle, healthBar, color, fill);
	AddMeshToList(tank);

	trajectoryMesh = new Mesh("trajectoryLine");
	trajectoryMesh->SetDrawMode(GL_LINE_STRIP);
	AddMeshToList(trajectoryMesh);

	// creem un al doilea tanc (identic cu primul)
    TankPosition2 = glm::vec3(0.0f, 0.0f, 0.0f);
    float bodyLength2 = 70.0f;      
    float wheelLength2 = 20.0f;      
    float archLength2 = 20.0f;       
    float cannonLength2 = 25.0f;    
    float healthBar2 = 1.0f;
    glm::vec3 color2(0.0f, 0.39f, 0.0f);
    bool fill2 = true;

    Mesh* tank2 = Tank::CreateTank("tank2", TankPosition2, bodyLength2, wheelLength2,
        archLength2, cannonLength2, cannonAngle2, healthBar2, color2, fill2);
    AddMeshToList(tank2);

	trajectoryMesh2 = new Mesh("trajectoryLine2");
	trajectoryMesh2->SetDrawMode(GL_LINE_STRIP);
	AddMeshToList(trajectoryMesh2);
	// creeam soarele
	glm::vec3 sunColor(1.0f, 1.0f, 0.0f); // galben
	Mesh* sun = Tank::CreateCircle("sun", glm::vec3(100.0f, 1000.0f, 0.0f), 50, sunColor, true);
	AddMeshToList(sun);


    float wheelsWidth = bodyLength * 1.1f;
    float bodyOffsetX = (wheelsWidth - bodyLength) / 2.0f;
    float healthBarWidth = bodyLength * 0.8f;
    float healthBarHeight = 10.0f;
    glm::vec3 healthBarPosition = TankPosition + glm::vec3(bodyOffsetX, wheelLength * 4.0f, 0);

    // Create health bar frame mesh
    Mesh* healthBarFrame = Tank::CreateRectangle("healthBarFrame", healthBarPosition,
        healthBarWidth, healthBarHeight, glm::vec3(0.0f, 0.0f, 0.0f), false);
    AddMeshToList(healthBarFrame);

	cannonHasMoved = true;
	lastCannonAngle = cannonAngle;
	lastCannonAngle2 = cannonAngle2;
}


void Tema1::FrameStart()
{
    
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

// verificam daca proiectilul a lovit terenul
bool Tema1::CheckCollision(glm::vec3 position, float radius) {
	// setam indexul terenului pe baza pozitiei proiectilului
    size_t terrainIndex = static_cast<size_t>(position.x / step);

	// verificam daca proiectilul a iesit din teren
    if (terrainIndex >= heightMap.size() - 1 || position.x < 0) {
        return false;
    }

    float x1 = terrainIndex * step;
    float x2 = (terrainIndex + 1) * step;
    float y1 = heightMap[terrainIndex];
    float y2 = heightMap[terrainIndex + 1];

	// interpolam inaltimea terenului la pozitia proiectilului
    float t = (position.x - x1) / (x2 - x1);
    float interpolatedHeight = y1 * (1 - t) + y2 * t;

	// verificam daca proiectilul a lovit terenul
    bool collision = position.y <= interpolatedHeight;

    if (collision) {
		// creem un crater in teren
        float craterRadius = radius;
		float maxDeformation = 50.0f; // ajustabil pentru adancimea craterului

		// calculam indicii de start si de final pentru crater
        int startIdx = static_cast<int>((position.x - craterRadius * 1.5f) / step);
        int endIdx = static_cast<int>((position.x + craterRadius * 1.5f) / step);

		// setam limitele pentru indici
        startIdx = std::max(0, startIdx);
        endIdx = std::min(static_cast<int>(heightMap.size() - 1), endIdx);

		// salvam inaltimile originale
        std::vector<float> originalHeights;
        for (int i = startIdx; i <= endIdx; i++) {
            originalHeights.push_back(heightMap[i]);
        }

		// aplicam craterul
        for (int i = startIdx; i <= endIdx; i++) {
            float distanceX = abs(i * step - position.x);
            if (distanceX <= craterRadius * 1.5f) {
				// folosim distanta normalizata pentru a calcula deformarea
                float normalizedDist = distanceX / craterRadius;

				// folosim o functie exponentiala pentru a crea un crater mai natural
                float deformation = maxDeformation * exp(-4 * normalizedDist * normalizedDist);

				// aplicam deformarea
                float targetHeight = originalHeights[i - startIdx] - deformation;
                heightMap[i] = std::max(0.0f, targetHeight);
            }
        }

		// rafinam tranzitia dintre inaltimile originale si cele deformate
        for (int smoothPass = 0; smoothPass < 2; smoothPass++) {
            std::vector<float> smoothedHeights = heightMap;
            for (int i = startIdx + 1; i < endIdx; i++) {
                smoothedHeights[i] = (heightMap[i - 1] + heightMap[i] * 2 + heightMap[i + 1]) / 4.0f;
            }
            heightMap = smoothedHeights;
        }
    }

    return collision;
}

// verificam daca proiectilul a lovit un tanc
bool Tema1::CheckTankCollision(glm::vec3 projectilePos, glm::vec3 tankPos, float tankRotation, float cannonAngle) {
	float bodyWidth = 70.0f;   // lungimea bazei
	float bodyHeight = 20.0f;  // inaltimea bazei
	float turretRadius = 20.0f; // raza domului tunului
	float wheelHeight = 20.0f;  // inaltimea roatei

	// transformam coordonatele proiectilului in coordonate locale fata de tanc
    float cosRot = cos(-tankRotation);
    float sinRot = sin(-tankRotation);

    float relX = projectilePos.x - tankPos.x;
    float relY = projectilePos.y - tankPos.y;

    float localX = relX * cosRot - relY * sinRot;
    float localY = relX * sinRot + relY * cosRot;

	// verificam coliziunea cu baza tancului
    bool bodyHit =
        localX >= -bodyWidth / 2 + collisionThreshold&&
        localX <= bodyWidth / 2  - collisionThreshold&&
        localY >= -wheelHeight + collisionThreshold&&
        localY <= bodyHeight - collisionThreshold;

	// verificam coliziunea cu domul tunului
    float turretCenterX = 0;
    float turretCenterY = bodyHeight;
    float dx = localX - turretCenterX;
    float dy = localY - turretCenterY;
    bool turretHit = (dx * dx + dy * dy) <= ((turretRadius + collisionThreshold) * (turretRadius + collisionThreshold));

    return bodyHit || turretHit;
}

// actualizam viata tancului
void Tema1::UpdateTankHealth(std::string tankName) {
    if (tankName == "tank1") {
		tank1Health -= 10.0f; // reducem viata cu 10%
		if (tank1Health < 0) tank1Health = 0; // ne asiguram ca viata nu scade sub 0
		float healthBarLength = tank1Health / 100.0f; // scurtam bara de viata cu 10%

		//se reface tancul doar daca este inca in viata
        if (tank1Health > 0) {
            float bodyLength = 70.0f;
            float wheelLength = 20.0f;
            float archLength = 20.0f;
            float cannonLength = 25.0f;
            bool fill = true;

            glm::vec3 color1(0.87f, 0.72f, 0.53f);
            Mesh* tank1 = Tank::CreateTank(tankName, glm::vec3(0), bodyLength, wheelLength,
                archLength, cannonLength, cannonAngle, healthBarLength, color1, fill);
            meshes.erase(tankName);
            AddMeshToList(tank1);
        }
    }
	// aplicam acelasi principiu pentru al doilea tanc
    else if (tankName == "tank2") {
        tank2Health -= 10.0f;
        if (tank2Health < 0) tank2Health = 0;
        float healthBarLength = tank2Health / 100.0f; 

        if (tank2Health > 0) {
            float bodyLength = 70.0f;
            float wheelLength = 20.0f;
            float archLength = 20.0f;
            float cannonLength = 25.0f;
            bool fill = true;

            glm::vec3 color2(0.0f, 0.39f, 0.0f);
            Mesh* tank2 = Tank::CreateTank(tankName, glm::vec3(0), bodyLength, wheelLength,
                archLength, cannonLength, cannonAngle2, healthBarLength, color2, fill);
            meshes.erase(tankName);
            AddMeshToList(tank2);
        }
    }
}


// actualizam pozitia tancului pe teren
void Tema1::UpdateTankPositionOnTerrain() {
    float verticalOffset = 5.0f;
    float bodyLength = 70.0f;
    float wheelLength = 20.0f;
	float maxX = (heightMap.size() - 1) * step; // limita maxima pe axa x

	// daca tancul nu iese din teren, actualizam pozitia
    if (TankPosition.x >= 0 && TankPosition.x < heightMap.size() * step) {
        size_t terrainIndex = static_cast<size_t>(TankPosition.x / step);
        float terrainHeightAtTank = heightMap[terrainIndex];
        TankPosition.y = terrainHeightAtTank + verticalOffset + wheelLength / 2.0f;

		// calculam unghiul de inclinare al tancului
        if (terrainIndex + 1 < heightMap.size()) {
            float terrainHeightNext = heightMap[terrainIndex + 1];
            float angle = atan2(terrainHeightNext - terrainHeightAtTank, step);
            tankRotationAngle = angle;
        }
    }

	// aceeasi logica pentru al doilea tanc
    if (TankPosition2.x >= 0 && TankPosition2.x < heightMap.size() * step) {
        size_t terrainIndex = static_cast<size_t>(TankPosition2.x / step);
        float terrainHeightAtTank = heightMap[terrainIndex];
        TankPosition2.y = terrainHeightAtTank + verticalOffset + wheelLength / 2.0f;

        if (terrainIndex + 1 < heightMap.size()) {
            float terrainHeightNext = heightMap[terrainIndex + 1];
            float angle = atan2(terrainHeightNext - terrainHeightAtTank, step);
            tankRotationAngle2 = angle;
        }
    }
}

// desenam traiectoria proiectilului
void Tema1::DrawTrajectoryView(glm::vec3 startPos, float angle, float initialVelocity, glm::vec3 color) {
	// creeam un mesh pentru traiectorie
    std::string meshName = "trajectoryLine";
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;

	// calculam traiectoria proiectilului
	float deltaTime = 0.02f;  // timpul dintre doua puncte
	float maxTime = 2.0f;     // timpul maxim de desenare a traiectoriei
    float gravity = 1600.0f;

	// velocitatea initiala a proiectilului
    glm::vec3 velocity(
        initialVelocity * cos(angle),
        initialVelocity * sin(angle),
        0.0f
    );

    glm::vec3 pos = startPos;
    int pointCount = 0;

	// Calculam punctele traiectoriei
    for (float t = 0; t <= maxTime; t += deltaTime) {
        if (pos.x >= 0 && pos.x < heightMap.size() * step) {
            vertices.push_back(VertexFormat(pos, color));
            indices.push_back(pointCount++);
        }

		// calculam urmatoarea pozitie a proiectilului
        glm::vec3 nextPos = pos;
        nextPos.x += velocity.x * deltaTime;
        nextPos.y += velocity.y * deltaTime;

        velocity.y -= gravity * deltaTime;

        size_t terrainIndex = static_cast<size_t>(nextPos.x / step);
        if (terrainIndex >= heightMap.size() - 1 || nextPos.x < 0) {
            break;
        }

		// Interpolam inaltimea terenului la pozitia proiectilului
        float x1 = terrainIndex * step;
        float x2 = (terrainIndex + 1) * step;
        float y1 = heightMap[terrainIndex];
        float y2 = heightMap[terrainIndex + 1];

        float t_interp = (nextPos.x - x1) / (x2 - x1);
        float interpolatedHeight = y1 * (1 - t_interp) + y2 * t_interp;

        if (nextPos.y <= interpolatedHeight) {
			// calculam punctul de coliziune
            glm::vec3 collisionPoint = nextPos;
			collisionPoint.y = interpolatedHeight; // desenam proiectilul la inaltimea terenului

            vertices.push_back(VertexFormat(collisionPoint, color));
            indices.push_back(pointCount);
            break;
        }

        pos = nextPos;
    }

	// creeam mesh-ul pentru traiectorie
    if (pointCount > 0) {
        Mesh* trajectoryMesh = new Mesh(meshName);
        trajectoryMesh->SetDrawMode(GL_LINE_STRIP);
        trajectoryMesh->InitFromData(vertices, indices);
        AddMeshToList(trajectoryMesh);

		// renderizam traiectoria
        RenderMesh2D(trajectoryMesh, shaders["VertexColor"], glm::mat3(1));
    }
}

// renderizam tancul 1
void Tema1::RenderTank(float x) {
	float bodyLength = 70.0f; // marimea bazei
	float wheelLength = 20.0f; // inaltimea roatei
    float heightScale = 360.0f;
    float y = static_cast<float>(abs(sin(0.005f * x) + 0.6f * sin(0.01f * x) + 0.3f * sin(0.02f * x)));
	float scaledY = y * heightScale;

    float initialVelocity = 100.0f;
    float gravity = 9.8f;

	// setam pozitia tancului la inceputul jocului
	if (TankPosition == glm::vec3(0.0f, 0.0f, 0.0f)) {
		TankPosition = glm::vec3(x, scaledY, 0.0f);
	}

    glm::mat3 modelMatrix = transform2Dtank::Translate(TankPosition.x, TankPosition.y);

	modelMatrix *= transform2Dtank::Translate(0, -wheelLength / 2.0f); // ajustam pentru inaltimea roatei
    modelMatrix *= transform2Dtank::Rotate(-tankRotationAngle);
    modelMatrix *= transform2Dtank::Translate(-bodyLength / 2.0f, -wheelLength / 2.0f);

	RenderMesh2D(meshes["tank1"], shaders["VertexColor"], modelMatrix);

	RenderMesh2D(meshes["healthBarFrame"], shaders["VertexColor"], modelMatrix);
}

// renderizam tancul 2 (identic cu tancul 1)
void Tema1::RenderTank2(float x) {
    float bodyLength = 70.0f;
    float wheelLength = 20.0f; 
    float heightScale = 360.0f;
    float y = static_cast<float>(abs(sin(0.005f * x) + 0.6f * sin(0.01f * x) + 0.3f * sin(0.02f * x)));
    float scaledY = y * heightScale;

    float initialVelocity = 100.0f;
    float gravity = 9.8f;
    
	if (TankPosition2 == glm::vec3(0.0f, 0.0f, 0.0f)) {
		TankPosition2 = glm::vec3(x, scaledY, 0.0f);
	}

    glm::mat3 modelMatrix = transform2Dtank::Translate(TankPosition2.x, TankPosition2.y);

    modelMatrix *= transform2Dtank::Translate(0, -wheelLength / 2.0f);
    modelMatrix *= transform2Dtank::Rotate(-tankRotationAngle2);
    modelMatrix *= transform2Dtank::Translate(-bodyLength / 2.0f, -wheelLength / 2.0f);

    RenderMesh2D(meshes["tank2"], shaders["VertexColor"], modelMatrix);

    RenderMesh2D(meshes["healthBarFrame"], shaders["VertexColor"], modelMatrix);
}


void Tema1::Update(float deltaTimeSeconds)
{
	float bodyLength = 70.0f; // lungimea bazei
	float wheelLength = 20.0f; // lungimea roatei

    // Render the terrain quads
    RenderMesh2D(quadMesh, shaders["VertexColor"], glm::mat3(1));
	RenderMesh2D(meshes["sun"], shaders["VertexColor"], glm::mat3(1));
	// move the sun
	float sunSpeed = 200.0f;
	float sunX = 100.0f;
	float sunY = 1000.0f;
	sunX += sunSpeed * deltaTimeSeconds;
	glm::mat3 sunModelMatrix = transform2Dtank::Translate(sunX, sunY);
    if (tank1Health > 0) {
        float x = 300.0f;
		// renderizam tancul 1 doar daca este inca in viata
        RenderTank(x);

        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float cannonLength = 25.0f;

        glm::vec3 cannonBaseOffset = glm::vec3(0.0f, wheelLength, 0);
        float effectiveTankRotation = tankRotationAngle;
        float combinedAngle = effectiveTankRotation + cannonAngle;

        float rotatedOffsetX = cannonBaseOffset.x * cos(effectiveTankRotation)
            - cannonBaseOffset.y * sin(effectiveTankRotation);
        float rotatedOffsetY = cannonBaseOffset.x * sin(effectiveTankRotation)
            + cannonBaseOffset.y * cos(effectiveTankRotation);

        glm::vec3 cannonBasePosition = TankPosition + glm::vec3(rotatedOffsetX, rotatedOffsetY, 0);
        float projectileOffsetX = cannonLength * cos(combinedAngle);
        float projectileOffsetY = cannonLength * sin(combinedAngle);
        glm::vec3 startPos = cannonBasePosition + glm::vec3(projectileOffsetX, projectileOffsetY, 0);

		float projectileSpeed = 600.0f;

		// desenam traiectoria proiectilului
        DrawTrajectoryView(startPos, combinedAngle, projectileSpeed, glm::vec3(1, 1, 1));
    }
    if (tank2Health > 0) {
        float x2 = 1500.0f;
		// renderizam tancul 2 doar daca este inca in viata
        RenderTank2(x2);

        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float cannonLength = 25.0f;

        glm::vec3 cannonBaseOffset = glm::vec3(0.0f, wheelLength, 0);
        float effectiveTankRotation = tankRotationAngle2;
        float combinedAngle = effectiveTankRotation + cannonAngle2;

        float rotatedOffsetX = cannonBaseOffset.x * cos(effectiveTankRotation)
            - cannonBaseOffset.y * sin(effectiveTankRotation);
        float rotatedOffsetY = cannonBaseOffset.x * sin(effectiveTankRotation)
            + cannonBaseOffset.y * cos(effectiveTankRotation);

        glm::vec3 cannonBasePosition = TankPosition2 + glm::vec3(rotatedOffsetX, rotatedOffsetY, 0);
        float projectileOffsetX = cannonLength * cos(combinedAngle);
        float projectileOffsetY = cannonLength * sin(combinedAngle);
        glm::vec3 startPos = cannonBasePosition + glm::vec3(projectileOffsetX, projectileOffsetY, 0);

        float projectileSpeed = 600.0f;

        DrawTrajectoryView(startPos, combinedAngle, projectileSpeed, glm::vec3(1, 1, 1));
    }


	// renderizam proiectilele
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        it->Update(deltaTimeSeconds);
        glm::mat3 projectileModelMatrix = transform2Dtank::Translate(it->GetPosition().x, it->GetPosition().y);
        RenderMesh2D(it->GetMesh(), shaders["VertexColor"], projectileModelMatrix);

        bool removeProjectile = false;
		float explosionRadius = 100.0f;

		// Verificam coliziunea proiectilului cu terenul si cu tancurile
        if (tank1Health > 0 && CheckTankCollision(it->GetPosition(), TankPosition, tankRotationAngle, cannonAngle)) {
            UpdateTankHealth("tank1");
            removeProjectile = true;
        }
        else if (tank1Health > 0 && CheckTankCollision(it->GetPosition(), TankPosition2, tankRotationAngle2, cannonAngle2)) {
            UpdateTankHealth("tank2");
            removeProjectile = true;
        }
		// Verificam coliziunea proiectilului cu terenul
        else if (CheckCollision(it->GetPosition(), explosionRadius)) {
            removeProjectile = true;
		}
		// Daca proiectilul face coliziune, il stergem
        if (removeProjectile) {
            it = projectiles.erase(it);
        }
        else {
            ++it;
        }
    }


	// efectuam alunecarea terenului
	float threshold = 1.0f; // threshold pentru diferenta de inaltime
	float transferAmount = 0.1f; // cantitatea de inaltime transferata

	// transferam inaltimea intre punctele vecine
    for (size_t i = 0; i < heightMap.size() - 1; ++i) {
        float heightDiff = heightMap[i] - heightMap[i + 1];
        if (abs(heightDiff) > threshold) {
            if (heightDiff > 0) {
                heightMap[i] -= transferAmount;
                heightMap[i + 1] += transferAmount;
            }
            else {
                heightMap[i] += transferAmount;
                heightMap[i + 1] -= transferAmount;
            }
        }
    }

	// actualizam mesh-ul terenului
    quadVertices.clear();
    quadIndices.clear();

    for (size_t i = 0; i < heightMap.size() - 1; ++i) {
        float x1 = i * step;
        float y1 = heightMap[i];
        float x2 = (i + 1) * step;
        float y2 = heightMap[i + 1];

        quadVertices.push_back(VertexFormat(glm::vec3(x1, y1, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x2, y2, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x2, 0, 0), glm::vec3(0.59f, 0.29f, 0.0f)));
        quadVertices.push_back(VertexFormat(glm::vec3(x1, 0, 0), glm::vec3(0.59f, 0.29f, 0.0f)));

        quadIndices.push_back(i * 4 + 0);
        quadIndices.push_back(i * 4 + 1);
        quadIndices.push_back(i * 4 + 2);
        quadIndices.push_back(i * 4 + 0);
        quadIndices.push_back(i * 4 + 2);
        quadIndices.push_back(i * 4 + 3);
    }

	// actualizam mesh-ul terenului
    quadMesh->InitFromData(quadVertices, quadIndices);

	// actualizam pozitia tancului pe teren
    UpdateTankPositionOnTerrain();
}

void Tema1::FrameEnd()
{
}

void Tema1::OnInputUpdate(float deltaTime, int mods) {
    float speed = 200;
    float maxX = (heightMap.size() - 1) * step;

	// Miscarea tancului 1
    if (window->KeyHold(GLFW_KEY_A)) {
        TankPosition.x = std::max(0.0f, TankPosition.x - speed * deltaTime);
        UpdateTankPositionOnTerrain();
    }
    if (window->KeyHold(GLFW_KEY_D)) {
        TankPosition.x = std::min(maxX, TankPosition.x + speed * deltaTime);
        UpdateTankPositionOnTerrain();
    }

	// Miscarea tancului 2
    if (window->KeyHold(GLFW_KEY_RIGHT)) {
        TankPosition2.x = std::min(maxX, TankPosition2.x + speed * deltaTime);
        UpdateTankPositionOnTerrain();
    }
    if (window->KeyHold(GLFW_KEY_LEFT)) {
        TankPosition2.x = std::max(0.0f, TankPosition2.x - speed * deltaTime);
        UpdateTankPositionOnTerrain();
    }
	// gestionam rotatia turelei
    float rotationSpeed = 2.0f;
    
	// rotim tureta tancului 1
    if (window->KeyHold(GLFW_KEY_W)) {
        cannonAngle += rotationSpeed * deltaTime;
        cannonRotated = true;
    }
    if (window->KeyHold(GLFW_KEY_S)) {
        cannonAngle -= rotationSpeed * deltaTime;
        cannonRotated = true;
    }
	// rotim tureta tancului 2
    if (window->KeyHold(GLFW_KEY_UP)) {
        cannonAngle2 += rotationSpeed * deltaTime;
        cannonRotated2 = true;
    }
    if (window->KeyHold(GLFW_KEY_DOWN)) {
        cannonAngle2 -= rotationSpeed * deltaTime;
        cannonRotated2 = true;
    }
    
	// Tancul isi roteste turela doar in limitele unghiului de rotatie
    cannonAngle = std::max(0.0f, std::min(cannonAngle, static_cast<float>(M_PI)));
    cannonAngle2 = std::max(0.0f, std::min(cannonAngle2, static_cast<float>(M_PI)));

	// daca tureta s-a rotit, actualizam mesh-ul tancului
    if (cannonRotated) {
        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float archLength = 20.0f;
        float cannonLength = 25.0f;
        float healthBar = 1.0f * (tank1Health / 100.0f);
        glm::vec3 color(0.87f, 0.72f, 0.53f);
        bool fill = true;

		glm::vec3 resetTankPosition = glm::vec3(0.0f, 0.0f, 0.0f);

        Mesh* tank = Tank::CreateTank("tank1", resetTankPosition, bodyLength, wheelLength,
            archLength, cannonLength, cannonAngle, healthBar, color, fill);
            
		// stergem vechiul mesh si adaugam noul mesh
        meshes.erase("tank1");
        AddMeshToList(tank);
    }

	// acelasi lucru pentru tancul 2
    if (cannonRotated2) {
        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float archLength = 20.0f;
        float cannonLength = 25.0f;
        float healthBar = 1.0f * (tank2Health / 100.0f);
        glm::vec3 color(0.0f, 0.39f, 0.0f);
        bool fill = true;

        glm::vec3 resetTankPosition = glm::vec3(0.0f, 0.0f, 0.0f);

        Mesh* tank = Tank::CreateTank("tank2", resetTankPosition, bodyLength, wheelLength,
            archLength, cannonLength, cannonAngle2, healthBar, color, fill);

        meshes.erase("tank2");
        AddMeshToList(tank);
    }
}

void Tema1::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_SPACE && tank1Health > 0 ) {
        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float cannonLength = 25.0f;

		// creem un proiectil pentru tancul 1
        glm::vec3 projectileColor(1.0f, 0.0f, 0.0f);
        Mesh* projectileMesh = Tank::CreateCircle("projectile", glm::vec3(0), 5, projectileColor, true);

		// calculam pozitia bazei tunului in spatiul lumii pentru tancul 1
        glm::vec3 cannonBaseOffset = glm::vec3(0.0f, wheelLength, 0);

		// folosim rotatia tancului direct - este deja in orientarea corecta
        float effectiveTankRotation = tankRotationAngle;

		// calculam unghiul combinat (rotatia tancului + unghiul tunului) pentru tancul 1
        float combinedAngle = effectiveTankRotation + cannonAngle;

		// calculam pozitia bazei tunului rotat pentru tancul 1
        float rotatedOffsetX = cannonBaseOffset.x * cos(effectiveTankRotation)
            - cannonBaseOffset.y * sin(effectiveTankRotation);
        float rotatedOffsetY = cannonBaseOffset.x * sin(effectiveTankRotation)
            + cannonBaseOffset.y * cos(effectiveTankRotation);

        glm::vec3 cannonBasePosition = TankPosition + glm::vec3(rotatedOffsetX, rotatedOffsetY, 0);

		// calculam pozitia proiectilului la capatul tunului pentru tancul 1
        float projectileOffsetX = cannonLength * cos(combinedAngle);
        float projectileOffsetY = cannonLength * sin(combinedAngle);
        glm::vec3 projectilePosition = cannonBasePosition + glm::vec3(projectileOffsetX, projectileOffsetY, 0);

        float projectileSpeed = 600.0f;
        projectiles.push_back(Projectile(projectilePosition, combinedAngle, projectileSpeed, projectileMesh));
        AddMeshToList(projectileMesh);

    }
	// acelasi lucru pentru tancul 2
    if (key == GLFW_KEY_ENTER && tank2Health > 0) {
        float bodyLength = 70.0f;
        float wheelLength = 20.0f;
        float cannonLength = 25.0f;

        glm::vec3 projectileColor(1.0f, 0.0f, 0.0f);
        Mesh* projectileMesh2 = Tank::CreateCircle("projectile2", glm::vec3(0), 5, projectileColor, true);

        glm::vec3 cannonBaseOffset2 = glm::vec3(bodyLength * 0.15f , wheelLength, 0);

        float effectiveTankRotation2 = tankRotationAngle2;

        float combinedAngle2 = effectiveTankRotation2 + cannonAngle2;

        float rotatedOffsetX2 = cannonBaseOffset2.x * cos(effectiveTankRotation2)
            - cannonBaseOffset2.y * sin(effectiveTankRotation2);
        float rotatedOffsetY2 = cannonBaseOffset2.x * sin(effectiveTankRotation2)
            + cannonBaseOffset2.y * cos(effectiveTankRotation2);

        glm::vec3 cannonBasePosition2 = TankPosition2 + glm::vec3(rotatedOffsetX2, rotatedOffsetY2, 0);

        float projectileOffsetX2 = cannonLength * cos(combinedAngle2);
        float projectileOffsetY2 = cannonLength * sin(combinedAngle2);
        glm::vec3 projectilePosition2 = cannonBasePosition2 + glm::vec3(projectileOffsetX2, projectileOffsetY2, 0);

        glm::vec3 direction2 = glm::normalize(glm::vec3(
            cos(combinedAngle2),
            sin(combinedAngle2),
            0
        ));

        float projectileSpeed2 = 600.0f;
        projectiles.push_back(Projectile(projectilePosition2, combinedAngle2, projectileSpeed2, projectileMesh2));
        AddMeshToList(projectileMesh2);
    }
}


void Tema1::OnKeyRelease(int key, int mods)
{
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema1::OnWindowResize(int width, int height)
{
}
