#include "AppClass.h"
void Application::InitVariables(void)
{
	////Change this to your name and email
	m_sProgrammer = "Alberto Bobadilla - labigm@rit.edu";
	vector3 v3Position(0.0f, 3.0f, 15.0f);
	vector3 v3Target = ZERO_V3;
	vector3 v3Upward = AXIS_Y;
	m_pCameraMngr->SetPositionTargetAndUpward(v3Position, v3Target, v3Upward);

	//Add the first cube (under)
	m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube");//Add cube entities
	m_pEntityMngr->UsePhysicsSolver();
	BTX::Solver* pSolver = new BTX::Solver();
	pSolver->SetSeparationStrength(1.15f);//Change values to this line to fine-tune
	m_pEntityMngr->SetNewSolver(pSolver);

	//Add the second cube (over)
	m_pEntityMngr->AddEntity("Minecraft\\Cube.obj", "Cube");//Add cube entities
	m_pEntityMngr->UsePhysicsSolver();

	//No need to release solver as that is handled in the entity itself
	pSolver = new BTX::Solver();
	pSolver->SetSeparationStrength(1.15f);//Change values to this line to fine-tune
	m_pEntityMngr->SetNewSolver(pSolver);
	m_pEntityMngr->SetModelMatrix(glm::translate(vector3(0.0f, 5.0f, 0.0f)));
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
	
	matrix4 m4Model = BTX::ToMatrix4(m_qArcBall);//model matrix

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
	//release GUI
	ShutdownGUI();
}