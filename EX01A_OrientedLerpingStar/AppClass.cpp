#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	vector3 v3Position(0.0f, 0.0f, 10.0f);
	vector3 v3Target = ZERO_V3;
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);
	//init the mesh
	m_pMesh = new Mesh();
	m_pMesh->GenerateStar(4.0f, m_uRays, C_GREEN);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	//CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);

	//List of all the stops the shape will go about
	std::vector<vector3> listOfSteps;
	listOfSteps.push_back(vector3(-1.5f,-1.5f, 0.0f));
	listOfSteps.push_back(vector3( 1.5f,-1.5f, 0.0f));
	listOfSteps.push_back(vector3( 1.5f, 1.5f, 0.0f));
	listOfSteps.push_back(vector3(-1.5f, 1.5f, 0.0f));

	//This will generate a new clock in the system
	static uint nClock = m_pSystem->GenClock();
	//This will calculate the time passed in between calls
	float fDelta = m_pSystem->GetDeltaTime(nClock);
	//Counts the constant time
	static float fTimer = 0.0f;
	//Increments the time passed
	fTimer += fDelta;
	//The duration of the transitions
	float fSeconds = 2.0f;
	//This will make the value from 0 to seconds to 0 to 1 as we need it
	float fMappedValue = BTX::MapValue(fTimer, 0.0f, fSeconds, 0.0f, 1.0f);
	//Call to the lerping method in mesh so we can move
	m_m4Model = m_pMesh->PositionLERP(listOfSteps, fMappedValue);
	//after the desired seconds have pass we reset the system and increment rays
	if (fTimer > fSeconds)
	{
		fTimer = 0.0f;
		m_uRays ++;
		m_pMesh->GenerateStar(4.0f, m_uRays, C_GREEN);
	}
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Calculate camera
	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix

	//Get a timer
	static uint uClock = m_pSystem->GenClock();
	float fTimer = m_pSystem->GetTimeSinceStart(uClock);

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();
	m_pModelMngr->AddGridToRenderList();

	//render list call
	m_uRenderCallCount = m_pModelMngr->Render();

	//clear the render list
	m_pModelMngr->ClearRenderList();

	// render the object
	m_pMesh->Render(m4Projection, m4View, m_m4Model);

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