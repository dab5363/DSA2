#include "Mesh.h"

Mesh::Mesh()
{
	Init();
}

void Mesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = BTX::ShaderManager::GetInstance();
}

Mesh::~Mesh() { Release(); }

void Mesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}

Mesh::Mesh(Mesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}

Mesh& Mesh::operator=(Mesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Mesh temp(other);
		Swap(temp);
	}
	return *this;
}

void Mesh::Swap(Mesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}

void Mesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}

void Mesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}

void Mesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}

void Mesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}

void Mesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{

	if (!m_bBinded)
		return;

	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader);

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));

	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth((GLfloat)3.0f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}

void Mesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
		//A--B
//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void Mesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}

void Mesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue, -fValue, fValue); //0
	vector3 point1(fValue, -fValue, fValue); //1
	vector3 point2(fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue, -fValue, -fValue); //4
	vector3 point5(fValue, -fValue, -fValue); //5
	vector3 point6(fValue, fValue, -fValue); //6
	vector3 point7(-fValue, fValue, -fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

													   //F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//BTX::Mesh* pMesh = new BTX::Mesh();
	//pMesh->GenerateCube(1.0f, a_v3Color);
	//m_lVertexPos = pMesh->GetVertexList();
	//m_uVertexCount = m_lVertexPos.size();
	//SafeDelete(pMesh);
	// 
	
		//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointList;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fRadius,
			glm::sin(fDelta * i) * a_fRadius,
			0.0f);
		pointList.push_back(v3NewPoint);
	}

	//The edge of each shape will be half the height away from the center
	vector3 v3Center(0, a_fHeight / -2, 0);

	//Offset points that make up the base so they are half the height away from the center of the shape
	matrix4 m4Offset = glm::translate(matrix4(), vector3(0, a_fHeight / -2, 0));
	matrix4 m4RotateOffset = glm::rotate(matrix4(), glm::radians(90.0f), vector3(1.0f, 0.0f, 0.0f));
	//Rotate the base then offset to avoid skewing the end result
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		pointList[i] = m4RotateOffset * vector4(pointList[i], 1.0f);
		pointList[i] = m4Offset * vector4(pointList[i], 1.0f);
	}

	//Draw base of the shape
	//for each point use the next point and the origin, 
	//once you are out of points use the first again (%)
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Center, pointList[i], pointList[(i + 1) % a_nSubdivisions]);
	}

	//Change the center point to be offset in the opposite direction
	v3Center.y *= -1;
	
	//Draw top of cone 
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Center, pointList[(i + 1) % a_nSubdivisions], pointList[i]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Replace this with your code
	//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointList;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fRadius,
			glm::sin(fDelta * i) * a_fRadius,
			0.0f);
		pointList.push_back(v3NewPoint);
	}

	//The edge of each shape will be half the height away from the center
	vector3 v3Center(0, a_fHeight / 2, 0);

	//Offset points that make up the base so they are half the height away from the center of the shape
	matrix4 m4Offset = glm::translate(matrix4(), vector3(0, a_fHeight / 2, 0));
	matrix4 m4RotateOffset = glm::rotate(matrix4(), glm::radians(270.0f), vector3(1.0f, 0.0f, 0.0f));

	//Rotate the base then offset to avoid skewing the end result
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		pointList[i] = m4RotateOffset * vector4(pointList[i], 1.0f);
		pointList[i] = m4Offset * vector4(pointList[i], 1.0f);
	}
	
	//Draw base of the shape
	//for each point use the next point and the origin, 
	//once you are out of points use the first again (%)
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Center, pointList[i], pointList[(i + 1) % a_nSubdivisions]);
	}

	
	//Change the center point to be offset in the opposite direction
	v3Center.y *= -1;

	//For the top face the points only need to be offset in the other direction and not rotated again as they are already aligned properly
	m4Offset = glm::translate(matrix4(), vector3(0, a_fHeight * -1, 0));
	std::vector<vector3> pointListTop;
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		pointListTop.push_back(m4Offset * vector4(pointList[i], 1.0f));
	}
	
	
	//Draw "top" of cylinder
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Center, pointListTop[(i + 1) % a_nSubdivisions], pointListTop[i]);
	}
	
	//Connect top and bottom of cylinder with sides
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddQuad(pointList[i], pointListTop[i], pointList[(i + 1) % a_nSubdivisions], pointListTop[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointListInnerTop;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fInnerRadius,
			glm::sin(fDelta * i) * a_fInnerRadius,
			0.0f);
		pointListInnerTop.push_back(v3NewPoint);
	}

	//Calculate an angle for each increment
	//float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointListOuterTop;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fOuterRadius,
			glm::sin(fDelta * i) * a_fOuterRadius,
			0.0f);
		pointListOuterTop.push_back(v3NewPoint);
	}


	//Offset points that make up the base so they are half the height away from the center of the shape
	matrix4 m4Offset = glm::translate(matrix4(), vector3(0, a_fHeight / 2, 0));
	matrix4 m4RotateOffset = glm::rotate(matrix4(), glm::radians(270.0f), vector3(1.0f, 0.0f, 0.0f));

	//Rotate the base then offset to avoid skewing the end result
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		pointListInnerTop[i] = m4RotateOffset * vector4(pointListInnerTop[i], 1.0f);
		pointListInnerTop[i] = m4Offset * vector4(pointListInnerTop[i], 1.0f);
		pointListOuterTop[i] = m4RotateOffset * vector4(pointListOuterTop[i], 1.0f);
		pointListOuterTop[i] = m4Offset * vector4(pointListOuterTop[i], 1.0f);
	}

	m4Offset = glm::translate(matrix4(), vector3(0, a_fHeight * -1, 0));

	std::vector<vector3> pointListInnerBottom;
	std::vector<vector3> pointListOuterBottom;
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		pointListInnerBottom.push_back(m4Offset * vector4(pointListInnerTop[i], 1.0f));
		pointListOuterBottom.push_back(m4Offset * vector4(pointListOuterTop[i], 1.0f));
	}

	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		//Connect inner faces along the length
		AddQuad(pointListInnerTop[i], pointListInnerTop[(i + 1) % a_nSubdivisions], pointListInnerBottom[i], pointListInnerBottom[(i + 1) % a_nSubdivisions]);
		//Connect outer faces along the length
		AddQuad(pointListOuterTop[i], pointListOuterBottom[i], pointListOuterTop[(i + 1) % a_nSubdivisions], pointListOuterBottom[(i + 1) % a_nSubdivisions]);

		//Connect
		AddQuad(pointListInnerTop[i], pointListOuterTop[i], pointListInnerTop[(i + 1) % a_nSubdivisions], pointListOuterTop[(i + 1) % a_nSubdivisions]);
		AddQuad(pointListInnerBottom[i], pointListInnerBottom[(i + 1) % a_nSubdivisions], pointListOuterBottom[i], pointListOuterBottom[(i + 1) % a_nSubdivisions]);
	}


	//// Replace this with your code
	//BTX::Mesh* pMesh = new BTX::Mesh();
	//pMesh->GenerateCube(1.0f, a_v3Color);
	//m_lVertexPos = pMesh->GetVertexList();
	//m_uVertexCount = m_lVertexPos.size();
	//SafeDelete(pMesh);
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	//SubdivisionA is the number of subdivisions each circle that makes up the torus has
	//SubdivisionB is the number of circles that make up the torus

	// Replace this with your code
	//This is the raidus of the circles that make the torus
	//It is the difference between the outer radius and inner radius of the torus giving the diameter of the torus thickness which is then halved for the raidus
	float a_fRadius = (a_fOuterRadius - a_fInnerRadius) / 2;

	//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisionsA);

	//make a place to hold your new vertices
	std::vector<vector3> pointList;
	std::vector<std::vector<vector3>> listOfCircles;

	//Make the circles that make up the torus
	for (uint i = 0; i < a_nSubdivisionsA + 1; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fRadius,
			glm::sin(fDelta * i) * a_fRadius,
			0.0f);
		pointList.push_back(v3NewPoint);
	}

	listOfCircles.push_back(pointList);


	float fTheta = static_cast<float>(2.0 * PI) / static_cast<float>(a_nSubdivisionsB);

	//Offset the circles from before to the correct position and angle for the torus
	for (uint j = 0; j < a_nSubdivisionsB + 1; j++)
	{
		// Make a backup of the original list of points point
		std::vector<vector3> newPointList = pointList;

		// Create the offset matrix to store the transform
		matrix4 m4Offset = glm::rotate(matrix4(), fTheta * j, vector3(0.0f, 1.0f, 0.0f))
			* glm::translate(vector3((a_fInnerRadius - (a_fRadius / 2)), 0.0f, 0.0f));

		//apply the offset to each of the vertices that will make the other circles
		for (uint i = 0; i < a_nSubdivisionsA + 1; i++)
		{
			newPointList[i] = m4Offset * vector4(newPointList[i], 1.0f);
		}

		listOfCircles.push_back(newPointList);
	}

	//Connect the circles together to draw the torus
	for (uint j = 1; j < a_nSubdivisionsB + 1; j++)
	{
		//Inner Circles
		for (uint i = 0; i < a_nSubdivisionsA; i++)
		{
			vector3 v3Origin;
			matrix4 m4Offset = glm::rotate(matrix4(), fTheta * j, vector3(0.0f, 1.0f, 0.0f))
				* glm::translate(vector3(5.0f, 0.0f, 0.0f));

			v3Origin = m4Offset * vector4(v3Origin, 1.0);
			AddQuad(listOfCircles[j][i], listOfCircles[j + 1][i], listOfCircles[j][(i + 1)], listOfCircles[j + 1][(i + 1)]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void Mesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	// Replace this with your code

	//This sphere works with any number of subdivisions that are 2 or more

	float fDelta = static_cast<float>(PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<std::vector<vector3>> listOfCircles;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions + 1; i++)
	{
		std::vector<vector3> pointList;
		for (uint j = 0; j < a_nSubdivisions + 1; j++)
		{
			//Use spherical coordiantes to calculate the x,y,z for each point of the sphere
			vector3 v3NewPoint(glm::cos(fDelta * (i)) * glm::sin(fDelta * (j)) * a_fRadius,
				glm::sin(fDelta * (i)) * glm::sin(fDelta * (j)) * a_fRadius,
				glm::cos(fDelta * (j)) * a_fRadius);
			pointList.push_back(v3NewPoint);
		}
		listOfCircles.push_back(pointList);
	}


	//Draw one half of the sphere
	for (uint j = 0; j < a_nSubdivisions; j++)
	{
		for (uint i = 0; i < a_nSubdivisions; i++)
		{
			AddQuad(listOfCircles[j][i], listOfCircles[j][(i + 1)], listOfCircles[j + 1][i], listOfCircles[j + 1][(i + 1)]);
		}
	}


	//Rotate 180 degrees to make the other half of the sphere
	matrix4 m4RotateOffset = glm::rotate(matrix4(), glm::radians(180.0f), vector3(1.0f, 0.0f, 0.0f));

	//Apply rotation offset
	for (uint i = 0; i < a_nSubdivisions + 1; i++)
	{
		for (uint j = 0; j < a_nSubdivisions + 1; j++)
		{
			listOfCircles[j][i] = m4RotateOffset * vector4(listOfCircles[j][i], 1.0f);
		}
	}

	//Draw the other half of the sphere
	for (uint j = 0; j < a_nSubdivisions; j++)
	{
		for (uint i = 0; i < a_nSubdivisions; i++)
		{
			AddQuad(listOfCircles[j][i], listOfCircles[j][(i + 1)], listOfCircles[j + 1][i], listOfCircles[j + 1][(i + 1)]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}

void Mesh::GenerateCircle(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 3)
	{
		return;
	}
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	// Replace this with your code
	//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointList;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(	glm::cos(fDelta * i) * a_fRadius,
							glm::sin(fDelta * i) * a_fRadius,
							0.0f);
		pointList.push_back(v3NewPoint);
	}
	vector3 v3Origin; // all triangles will share an origin
	//for each point use the next point and the origin, 
	//once you are out of points use the first again (%)
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Origin, pointList[i], pointList[(i + 1) % a_nSubdivisions]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();

}
void Mesh::GenerateCCircles(float a_fRadius, int a_nSubdivisions, int a_nCircles, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 3)
	{
		return;
	}
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	// Replace this with your code
	//Calculate an angle for each increment
	float fDelta = static_cast<float>(2.0f * PI) / static_cast<float>(a_nSubdivisions);
	//make a place to hold your new vertices
	std::vector<vector3> pointList;
	//Create new points around the circumference using cos(t), sin(t) as many as subdivisions
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		vector3 v3NewPoint(glm::cos(fDelta * i) * a_fRadius,
			glm::sin(fDelta * i) * a_fRadius,
			0.0f);
		pointList.push_back(v3NewPoint);
	}

	vector3 v3Origin; // all triangles will share an origin
	//for each point use the next point and the origin, 
	//once you are out of points use the first again (%)
	for (uint i = 0; i < a_nSubdivisions; i++)
	{
		AddTri(v3Origin, pointList[i], pointList[(i + 1) % a_nSubdivisions]);
	}

	//create additional circles here
	float fTheta = static_cast<float>(2.0 * PI) / static_cast<float>(a_nCircles - 1);
	for (uint j = 0; j < a_nCircles - 1; j++)
	{
		// Make a backup of the original list of points point
		std::vector<vector3> newPointList = pointList;
		
		// Create the offset matrix to store the transform
		matrix4 m4Offset = glm::rotate(matrix4(), fTheta * j, vector3(0.0f, 1.0f, 0.0f));
		
		//apply the offset to each of the vertices that will make the other circles
		for (uint i = 0; i < a_nSubdivisions; i++)
		{
			newPointList[i] = m4Offset * vector4(newPointList[i], 1.0f);
		}
		
		//create the new circle
		for (uint i = 0; i < a_nSubdivisions; i++)
		{
			AddTri(v3Origin, newPointList[i], newPointList[(i + 1) % a_nSubdivisions]);
		}
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}