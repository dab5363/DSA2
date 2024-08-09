#include "AppClass.h"
void Application::InitVariables(void)
{
	//Message for the App
	m_sToPrint = "This are two objects, not only one!";
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 10.0f), vector3(), AXIS_Y);//Set Camera position
	
	//Model to load
	m_sModel = "Minecraft\\Enderman.obj";
	//m_sModel = "SphereCube.obj";
	//Load said model
	m_pModelMngr->LoadModel(m_sModel);
	m_pRB = new RigidBody(m_pModelMngr->GetVertexList(m_sModel));
	//Missed this line, watch 2/29 zoom video to see what it is
}
void Application::Update(void)
{
	m_pSystem->Update();//Update the system
	ArcBall();//Is the arcball active?
	CameraRotation(); //Is the first person camera active?

	//Add the Model to the render list
	m_pModelMngr->AddModelToRenderList(m_sModel, OrientByArcball());
	m_pRB->SetModelMatrix(OrientByArcball());

	m_pEntityMngr->Update(); //Update Entity Manager
	m_pEntityMngr->AddEntityToRenderList(-1, true); //Add objects to render list
}
void Application::Display(void)
{
	ClearScreen(); // Clear the screen
	m_pModelMngr->AddSkyboxToRenderList(); // draw a skybox
	m_pRB->AddToRenderList();
	m_uRenderCallCount = m_pModelMngr->Render(); //render list call
	m_pModelMngr->ClearRenderList(); //clear the render list
	DrawGUI(); //draw gui
	m_pWindow->display(); //end the frame (swap front and back buffers)

}
void Application::Release(void)
{
	if (m_pRB != nullptr)
	{
		delete m_pRB;
		m_pRB = nullptr;
	}
	ShutdownGUI();//release GUI
}