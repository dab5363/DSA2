#include "AppClass.h"
void Application::InitVariables(void)
{
	//Message for the App
	
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(10.0f, 0.0f, 50.0f), vector3(), AXIS_Y);//Set Camera position
	
	//Models to load
	m_pMyEntityMngr = EntityManager::GetInstance();

	uint uCount = 10000;

#ifdef DEBUG
	uCount = 1000;
#endif // DEBUG

	m_sToPrint = std::to_string(uCount) + " objects!";
	for (uint i = 0; i < uCount; i++)
	{
		m_pMyEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube");
		vector3 v3Position = vector3(glm::sphericalRand(14.0f));
		uint nDimension = i;

		m_pMyEntityMngr->AddDimension(-1, nDimension);

		m_pMyEntityMngr->SetModelMatrix(glm::translate(v3Position), -1);
	}
	
}
void Application::Update(void)
{
	m_pSystem->Update();//Update the system
	ArcBall();//Is the arcball active?
	CameraRotation(); //Is the first person camera active?

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