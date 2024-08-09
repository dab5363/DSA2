#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	BTX::vector3 v3Position(0.0f, 2.0f, 5.0f);
	BTX::vector3 v3Target(0.0f, 2.0f, 4.0f);
	BTX::vector3 v3Upward = BTX::AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);
	m_pModel = new BTX::Model("Minecraft\\Cube.obj");
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();

	BTX::Camera* pCamera = m_pCameraMngr->GetCamera();

	//Position 5000 models and render them individually
	for (uint i = 0; i < 5000; i++)
	{
		matrix4 m4Position = glm::translate(matrix4(), vector3(0.0f, 0.0f, i * -1.0f));
		m_pModel->Render(pCamera->GetProjectionMatrix(), pCamera->GetViewMatrix(), m4Position);
	}

	//render list call
	m_uRenderCallCount = m_pModelMngr->Render();

	//clear the render list
	m_pModelMngr->ClearRenderList();

	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release GUI
	ShutdownGUI();
}