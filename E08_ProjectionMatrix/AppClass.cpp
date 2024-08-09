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
	m_pSteve = new BTX::Model("Minecraft\\Steve.obj");
	m_pCreeper = new BTX::Model("Minecraft\\Creeper.obj");
	
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
	//CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//Position Target and Upward();
	vector3 v3Position(0.0f, 2.0f, 3.0f);
	vector3 v3Target(0.0f, 2.0f, 0.0f);
	vector3 v3Up(0.0f, 1.0f, 0.0f);
	m_pCamera->SetPositionTargetAndUpward(v3Position, v3Target, v3Up);

	bool bPerspective = true;
	float fFOV = 90.0f;
	vector2 v2NearFar = vector2(0.0001f, 1000.0f);
	vector2 v2ScreenSize = vector2(m_pSystem->GetWindowWidth(), m_pSystem->GetWindowHeight());
	vector2 v2LeftRight = vector2(-2.2f, 2.2f);
	vector2 v2DownUp = vector2(-2.2f, 2.2f);

	//Switch the camera
	switch (m_uScene)
	{
	default:
	case 0: //Default Perspective
		break;
	case 1: //Default Orthogonal
		bPerspective = false;
		break;
#pragma region ONLY SECTION THAT NEEDS CHANGES
	case 2: 
		//Can't see top of creeper's head
		//Steve is missing from the scene
		bPerspective = false;
		v2NearFar = vector2(3.0f, 1000.0f);
		break;
	case 3:
		//Can see top of Steve's shoulders
		//Creeper is missing from the scene
		bPerspective = true;
		v2NearFar = vector2(0.0001f, 3.25f);
		break;
	case 4:
		//Cannot see the top of Steve's shoulders
		//Can only see the left side of Steve and Creeper,
		//the projection is distorted
		bPerspective = false; 
		v2ScreenSize = vector2(m_pSystem->GetWindowWidth(), m_pSystem->GetWindowHeight() * 1.5);
		v2LeftRight = vector2(-2.2f, 0.0f);
		break;
	case 5:
		//Cannot see the top of Steve's shoulders
		//The objects are squished
		bPerspective = false;
		v2ScreenSize = vector2(m_pSystem->GetWindowWidth(), m_pSystem->GetWindowHeight() * 2);
		break;
	case 6:
		//Can see top of Steve's shoulders
		//Objects are large, but camera didn't move
		bPerspective = true;
		fFOV = 25.0f;
		break;
	case 7:
		//Can see top of Steve's shoulders
		//Objects are small, but camera didn't move
		bPerspective = true;
		fFOV = 155.0f;
		break;
	case 8:
		//Cannot see the top of Steve's shoulders
		//Objects are upside down and facing the wrong direction
		bPerspective = false;
		v2DownUp = vector2(2.2f, -2.2f); 
		break;
	case 9:
		//Can see the top of Steve's shoulders
		//Objects are upside down and facing the wrong direction
		bPerspective = true;
		fFOV = 270.0f;
		break;
#pragma endregion
	}

	//Send values to the camera
	m_pCamera->SetPerspectiveFlag(bPerspective);
	m_pCamera->SetFOV(fFOV);
	m_pCamera->SetScreenSize(v2ScreenSize);
	m_pCamera->SetNearFarPlanes(v2NearFar);
	m_pCamera->SetHorizontalView(v2LeftRight);
	m_pCamera->SetVerticalview(v2DownUp);
		
	//Render the models with the specified View and Projections
	matrix4 m4View = m_pCamera->GetViewMatrix();
	matrix4 m4Projection = m_pCamera->GetProjectionMatrix();

	m_pSteve->Render(m4Projection, m4View, matrix4());
	m_pCreeper->Render(m4Projection, m4View, glm::translate(matrix4(), vector3(-0.5f, 0.0f, -1.0f)));

	// draw a skybox
	m_pModelMngr->AddSkyboxToRenderList();
		
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
	//Release the models
	SafeDelete(m_pSteve);
	SafeDelete(m_pCreeper);

	//release the camera
	SafeDelete(m_pCamera);

	//release GUI
	ShutdownGUI();
}