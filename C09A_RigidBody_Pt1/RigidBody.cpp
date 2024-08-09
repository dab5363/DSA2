#include "RigidBody.h"

//Accessors
bool RigidBody::GetVisible(void) { return m_bVisible; }
float RigidBody::GetRadius(void) { return m_fRadius; }
vector3 RigidBody::GetColor(void) { return m_v3Color; }
void RigidBody::SetColor(vector3 a_v3Color) { m_v3Color = a_v3Color; }
vector3 RigidBody::GetCenterLocal(void) { return m_v3Center; }
vector3 RigidBody::GetMinLocal(void) { return m_v3MinL; }
vector3 RigidBody::GetMaxLocal(void) { return m_v3MaxL; }
vector3 RigidBody::GetCenterGlobal(void){	return vector3(m_m4ToWorld * vector4(m_v3Center, 1.0f)); }
vector3 RigidBody::GetMinGlobal(void) { return m_v3MinG; }
vector3 RigidBody::GetMaxGlobal(void) { return m_v3MaxG; }
vector3 RigidBody::GetHalfWidth(void) { return m_v3HalfWidth; }
matrix4 RigidBody::GetModelMatrix(void) { return m_m4ToWorld; }
void RigidBody::SetModelMatrix(matrix4 a_m4ModelMatrix) { m_m4ToWorld = a_m4ModelMatrix; }
//Allocation
void RigidBody::Init(void)
{
	m_pMeshMngr = BTX::ModelManager::GetInstance();
	m_bVisible = true;

	m_fRadius = 0.0f;

	m_v3Color = C_WHITE;

	m_v3Center = ZERO_V3;

	m_v3MinL = ZERO_V3;
	m_v3MaxL = ZERO_V3;

	m_v3MinG = ZERO_V3;
	m_v3MaxG = ZERO_V3;

	m_v3HalfWidth = ZERO_V3;

	m_m4ToWorld = IDENTITY_M4;
}
void RigidBody::Swap(RigidBody& other)
{
	std::swap(m_pMeshMngr , other.m_pMeshMngr);
	std::swap(m_bVisible , other.m_bVisible);

	std::swap(m_fRadius, other.m_fRadius);

	std::swap(m_v3Color , other.m_v3Color);

	std::swap(m_v3Center , other.m_v3Center);
	std::swap(m_v3MinL , other.m_v3MinL);
	std::swap(m_v3MaxL , other.m_v3MaxL);

	std::swap(m_v3MinG , other.m_v3MinG);
	std::swap(m_v3MaxG , other.m_v3MaxG);

	std::swap(m_v3HalfWidth , other.m_v3HalfWidth);

	std::swap(m_m4ToWorld , other.m_m4ToWorld);
}
void RigidBody::Release(void)
{
	m_pMeshMngr = nullptr;
}
//The big 3
RigidBody::RigidBody(std::vector<vector3> pointList)
{
	int vertexCount = pointList.size();
	if (vertexCount < 1)
		return;
	Init();

	//Eye balling it
	//m_fRadius = 5.0f;
	//m_v3Center = vector3(0.0f, 2.0f, 0.0f);
	
	////find the average
	////Bad Way Start
	//vector3 v3Average = pointList[0];
	//// Remember for exam
	//// We set uint to 1 and not 0 to avoid double counting pointList 0 in and out of the for loop
	//for (uint i = 1; i < vertexCount; i++)
	//{
	//	v3Average += pointList[i];
	//}
	//v3Average = v3Average / static_cast<float>(vertexCount);
	//
	//m_fRadius = 0.0f;
	//for (uint i = 1; i < vertexCount; i++)
	//{
	//	float fDistance = glm::distance(v3Average, pointList[i]);
	//	if (fDistance > m_fRadius)
	//		m_fRadius = fDistance;
	//}
	//m_v3Center = v3Average;
	////Bad Way End

	// Find the Extremes
	m_v3MinL = m_v3MaxL = pointList[0];
	for (uint i = 0; i < vertexCount; i++)
	{
		if (m_v3MinL.x > pointList[i].x)
			m_v3MinL.x = pointList[i].x;
		if (m_v3MaxL.x < pointList[i].x)
			m_v3MaxL.x = pointList[i].x;

		if (m_v3MinL.y > pointList[i].y)
			m_v3MinL.y = pointList[i].y;
		if (m_v3MaxL.y < pointList[i].y)
			m_v3MaxL.y = pointList[i].y;

		if (m_v3MinL.z > pointList[i].z)
			m_v3MinL.z = pointList[i].z;
		if (m_v3MaxL.z < pointList[i].z)
			m_v3MaxL.z = pointList[i].z;
	}

	m_v3Center = (m_v3MaxL + m_v3MinL) / 2.0f;
	m_fRadius = 0.0f;

	for (uint i = 0; i < vertexCount; i++)
	{
		float fDistance = glm::distance(m_v3Center, pointList[i]);
			if (fDistance > m_fRadius)
				m_fRadius = fDistance;
	}
	// Half width is like radius but for bounding box
	m_v3HalfWidth = (m_v3MaxL - m_v3MinL) / 2.0f;
}
RigidBody::RigidBody(RigidBody const& other)
{
	m_pMeshMngr = other.m_pMeshMngr;
	m_bVisible = other.m_bVisible;

	m_fRadius = other.m_fRadius;

	m_v3Color = other.m_v3Color;

	m_v3Center = other.m_v3Center;
	m_v3MinL = other.m_v3MinL;
	m_v3MaxL = other.m_v3MaxL;

	m_v3MinG = other.m_v3MinG;
	m_v3MaxG = other.m_v3MaxG;

	m_v3HalfWidth = other.m_v3HalfWidth;

	m_m4ToWorld = other.m_m4ToWorld;
}
RigidBody& RigidBody::operator=(RigidBody const& other)
{
	if(this != &other)
	{
		Release();
		Init();
		RigidBody temp(other);
		Swap(temp);
	}
	return *this;
}
RigidBody::~RigidBody(){Release();};

//--- Non Standard Singleton Methods
void RigidBody::AddToRenderList(void)
{
	if (!m_bVisible)
		return;
	matrix4 m4ToWorld = m_m4ToWorld * 
		glm::translate(m_v3Center) * 
		// Dividing in RigidBody and multiplying here by two will help with hw5
		glm::scale(vector3(m_fRadius));

	m_pMeshMngr->AddWireSphereToRenderList(m4ToWorld, C_ORANGE);

	m4ToWorld = m_m4ToWorld *
		glm::translate(m_v3Center) *
		// Dividing in RigidBody and multiplying here by two will help with hw5
		glm::scale(vector3(m_v3HalfWidth * 2.0f));

	m_pMeshMngr->AddWireCubeToRenderList(m4ToWorld, C_GREEN_LIME);
}
bool RigidBody::IsColliding(RigidBody* const other)
{
	return false;
}

//--- Set values for the temp functions
void RigidBody::SetCenterGlobal(vector3 a_v3Input)
{
	m_v3Center = m_m4ToWorld * vector4(a_v3Input, 1.0f);
}
void RigidBody::SetRadius(float a_fInput)
{
	m_fRadius = a_fInput;
}
void RigidBody::SetHalfWidth(vector3 a_v3Input)
{
	m_v3HalfWidth = a_v3Input;
}