#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	vector3 v3Position(0.0f, 0.0f, 10.0f);
	vector3 v3Target = ZERO_V3;
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);

	//Load a model for example with complex models
	m_pModel = new BTX::Model("Minecraft\\Steve.obj");
	
	//create a new camera
	m_pCamera = new Camera();
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

	static uint uClock = m_pSystem->GenClock();//Clock to count
	static float fTimer = 0.0f;//Timer to keep track
	float fDeltaTime = m_pSystem->GetDeltaTime(uClock);//Time since last call to this clock
	fTimer += fDeltaTime;//Append the delta to the timer
	//fTimer will increment equally as seconds increment

	//Camera position
	m_pCamera->SetPosition(vector3(fTimer, 0.0f, 10.0f));
	m_pCamera->SetTarget(vector3(fTimer, 0.0f, 9.0f));

	matrix4 m4Model;
	matrix4 m4View = m_pCamera->GetViewMatrix();
	matrix4 m4Projection = m_pCamera->GetProjectionMatrix();

	m_pModel->Render(m4Projection, m4View, m4Model);

	m_pModel->Render(m_pCameraMngr->GetProjectionMatrix(), m_pCameraMngr->GetViewMatrix(), m4Model);
	
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

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}