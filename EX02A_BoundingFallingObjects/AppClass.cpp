#include "AppClass.h"
void Application::InitVariables(void)
{
	//Message for the App
	
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 15.0f), 
		vector3(0.0f, 0.0f, 0.0f), 
		AXIS_Y);//Set Camera position
	
	//Models to load
	m_pMyEntityMngr = EntityManager::GetInstance();

	uint uCount = ENTITIES;

	m_sToPrint = std::to_string(uCount) + " objects!";
	for (uint i = 0; i < uCount; i++)
	{
		m_pMyEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube");
		vector3 v3Position = vector3(glm::sphericalRand(4.0f));
		v3Position.y = 10.0f;

		m_pMyEntityMngr->SetModelMatrix(glm::translate(v3Position), -1);
		entitySpeed[i] = i + 1;
		Entity* pEntity = m_pMyEntityMngr->GetEntity(-1);
		RigidBody* pRB = pEntity->GetRigidBody();
		pRB->SetVisibleARBB(false);
		pRB->SetVisibleBS(true);
		pRB->SetVisibleOBB(false);
	}

	std::vector<vector3> listOfPoints;
	for (uint i = 0; i < uCount; i++)
	{
		Entity* pEntity = m_pMyEntityMngr->GetEntity(i);
		RigidBody* pRB = pEntity->GetRigidBody();
		listOfPoints.push_back(pRB->GetMinGlobal());
		listOfPoints.push_back(pRB->GetMaxGlobal());
	}
	m_pMyEntityMngr->AddEntity("Minecraft\\Enderman.obj", "Enderman");

	m_pModelMngr->SetWireSphereThickness(1.0f); //to reduce the line thickness, makes it easy to see
}
void Application::Update(void)
{
	m_pSystem->Update();//Update the system
	ArcBall();//Is the arcball active?
	CameraRotation(); //Is the first person camera active?

	static uint uClock = m_pSystem->GenClock();
	static float fStart = m_pSystem->GetTimeSinceStart(uClock);
	float fDelta = m_pSystem->GetDeltaTime(uClock);

	uint uEntities = m_pMyEntityMngr->GetEntityCount();
	for (uint i = 0; i < uEntities -1; i++)
	{
		Entity* pEntity = m_pMyEntityMngr->GetEntity(i);
		matrix4 m4ToWorld = pEntity->GetModelMatrix();
		float height = -fDelta * entitySpeed[i];
		if (m4ToWorld[3][1] < -10.0f)
		{
			vector3 v3Position = vector3(glm::sphericalRand(4.0f));
			v3Position.y = 10.0f;
			m4ToWorld[3][0] = v3Position.x;
			m4ToWorld[3][1] = v3Position.y;
			m4ToWorld[3][2] = v3Position.z;
		}
		pEntity->SetModelMatrix(m4ToWorld * glm::translate(vector3(0.0f, height, 0.0f)));	
	}
	m_pMyEntityMngr->SetModelMatrix(OrientByArcball(), "Enderman");
	

	//Add the model matrix to the entities
	m_pMyEntityMngr->Update();
	
	//Render the entieies
	//-1 means all of the entities
	m_pMyEntityMngr->AddEntityToRenderList(-1, true);
	
	m_pEntityMngr->Update(); //Update Entity Manager (the one in the system)
	m_pEntityMngr->AddEntityToRenderList(-1, true); //Add objects to render list
}
void Application::Display(void)
{
	ClearScreen(); // Clear the screen
	m_pModelMngr->AddSkyboxToRenderList(); // draw a skybox
	m_uRenderCallCount = m_pModelMngr->Render(); //render list call
	m_pModelMngr->ClearRenderList(); //clear the render list
	DrawGUI(); //draw gui
	m_pWindow->display(); //end the frame (swap front and back buffers)
}
void Application::Release(void)
{
	//Release the memory of the Rigid Body
	m_pMyEntityMngr->ReleaseInstance();

	ShutdownGUI();//release GUI
}