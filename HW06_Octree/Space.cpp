#include "Space.h"
using namespace BTX;
//  Space
uint Space::m_uSpaceCount = 0;
uint Space::m_uMaxLevel = 3;
uint Space::m_uIdealEntityCount = 5;
Space::Space(uint a_uMaxLevel, uint a_uIdealEntityCount)
{
	/*
	* This constructor is meant to be used ONLY on the root Space, there is already a working constructor
	* that will take a size and a center to create a new space
	*/

	Init();//Init the default values

	m_uSpaceCount = 0;
	m_uID = m_uSpaceCount;

	m_uMaxLevel = a_uMaxLevel;

	m_uIdealEntityCount = a_uIdealEntityCount;

	//create a rigid body that encloses all the objects in this octant, if necessary you will need
	//to subdivide the octant based on how many objects are in it already an how many you IDEALLY
	//want in it, remember each subdivision will create 8 children for this octant but not all children
	//of those children will have children of their own

	std::vector<vector3> lMinMax; //Will hold all of the Min and Max vectors each Bounding Object
	//Example values, they are hard coded, do not use them
	//lMinMax.push_back(vector3());
	//lMinMax.push_back(vector3(10.0f));

	//Count the entities
	uint uEntities = m_pEntityMngr->GetEntityCount();
	//for each one of them
	for (uint i = 0; i < uEntities; i++)
	{
		//get the entity
		Entity* pEntity = m_pEntityMngr->GetEntity(i);
		//get its rigid body
		RigidBody* pRigidBody = pEntity->GetRigidBody();
		//get its max and mins into the list
		lMinMax.push_back(pRigidBody->GetMinGlobal());
		lMinMax.push_back(pRigidBody->GetMaxGlobal());
	}
	//make a new rigid body around the objects
	m_pRigidBody = new RigidBody(lMinMax);
	
	//Increment the Space count on each Space created
	m_uSpaceCount++;
	
	//Subdivide the space based on the arguments
	Subdivide();
		
	//Once the tree is constructed check what entity corresponds to what Space
	AssignIDstoEntities();
}
//Private constructor
Space::Space(vector3 a_v3Center, vector3 a_v3Size)
{
	//This can only be applied to children not the root
	//This constructor only takes the center and the size not the entities
	if (m_uSpaceCount < 1)
		return;

	//Will create the Space object based on the center and size but will not construct children
	Init();
	std::vector<vector3> lMinMax;
	lMinMax.push_back(a_v3Center - (a_v3Size / 2.0f));//Min
	lMinMax.push_back(a_v3Center + (a_v3Size / 2.0f));//Max
	m_pRigidBody = new RigidBody(lMinMax);
	m_uID = m_uSpaceCount;//ID of the current Space
	m_uSpaceCount++;//Increment the count of the Spaces
}
bool Space::IsColliding(uint a_uRBIndex)
{
	//Get how many objects there are in the world
	//If the index given is larger than the number of elements in the bounding object there is no collision
	//As the Octree will never rotate or scale this collision is as easy as an Axis Alligned Bounding Box
	//Get all vectors in global space (the octant ones are already in Global)
	if (a_uRBIndex >= m_pEntityMngr->GetEntityCount())
		return false;

	//Get the rigid body of the Entity queried
	Entity* pEntity = m_pEntityMngr->GetEntity(a_uRBIndex);
	RigidBody* pRigidBody = pEntity->GetRigidBody();
	//Ask if the rigid body of the entity is colliding with this Space
	return m_pRigidBody->IsColliding(pRigidBody, false);
}
bool Space::ContainsAtLeast(uint a_nEntities)
{
	//This method will count how many entities are contained in the current Space
	//if there are at least a_nEntities return true
	uint currentEnts = 0;

	uint nEntities = m_pEntityMngr->GetEntityCount();
	for (uint nIndex = 0; nIndex < nEntities; nIndex++)
	{
		//Check if this Space is colliding with the entity
		if (IsColliding(nIndex))
		{
			//if it is then add the uid as the space for the entity
			currentEnts++;
		}
	}
	if (currentEnts > a_nEntities)
	{
		return true;
	}
	else
	{
		return false;
	}
	//return false;//hardcoded for class purposes
}
void Space::Subdivide(void)
{
	//For this method IF YOU HAVE NOT REACHED the maximum level
	//you want to subdivide the current Space into 8 children
	//Then divide the children if that child has more than the
	//ideal count of entities
	if (m_uLevel < m_uMaxLevel)
	{
		//If this Space has been subdivided already, return without changes
		if (m_uChildren != 0)
			return;
		//create as many entities as needed

		m_uChildren = 8;
		//m_pChild = new PNode[m_uChildren];

		//each child will be the same size, the parents divided by how many we need
		vector3 v3ChildrenSize = m_pRigidBody->GetHalfWidth() * 2.0f;
		v3ChildrenSize.x /= static_cast<float>(2.0f);
		v3ChildrenSize.y /= static_cast<float>(2.0f);
		v3ChildrenSize.z /= static_cast<float>(2.0f);
		//The center of each children will be at different positions
		vector3 v3ChildrenCenter = v3ChildrenSize / 2.0f;
		/*
		* We will iterate from left to right, bottom to top, front to back
		*/
		uint index = 0;
		vector3 v3ParentMin = m_pRigidBody->GetMinGlobal();
		vector3 v3CurrentPosition = v3ParentMin + v3ChildrenCenter;
		for (uint z = 0; z < 2; z++)
		{
			for (uint y = 0; y < 2; y++)
			{
				for (uint x = 0; x < 2; x++)
				{
					m_pChild[index] = new Space(v3CurrentPosition, v3ChildrenSize);
					m_pChild[index]->m_uLevel = m_uLevel + 1;
					if (m_pChild[index]->ContainsAtLeast(m_pChild[index]->m_uIdealEntityCount))
					{
						m_pChild[index]->Subdivide();
					}
					v3CurrentPosition.x += v3ChildrenSize.x;
					index++;
				}
				v3CurrentPosition.x = v3ParentMin.x + v3ChildrenCenter.x;
				v3CurrentPosition.y += v3ChildrenSize.y;
			}
			v3CurrentPosition.x = v3ParentMin.x + v3ChildrenCenter.x;
			v3CurrentPosition.y = v3ParentMin.y + v3ChildrenCenter.y;
			v3CurrentPosition.z += v3ChildrenSize.z;
		}
	}	
}
void Space::AssignIDstoEntities(void)
{
	//Traverse the tree and on leafs check for all entities
	//that are colliding with the current Space, then use
	//m_pEntityMngr->AddDimension(nIndex, m_uID); so the
	//entity knows they belong to this space/dimension/Space

		//Traverse the children
	for (uint nIndex = 0; nIndex < m_uChildren; nIndex++)
	{
		m_pChild[nIndex]->AssignIDstoEntities();
	}
	//Only Assign IDs to leaf nodes, otherwise its brute force
	if (IsLeaf())
	{
		//for each entity in the entity manager check if its colliding
		//with this leaf, if it is then assign this ID to the entity's 
		//space list of spaces, this ID will act as the space ID
		uint nEntities = m_pEntityMngr->GetEntityCount();
		for (uint nIndex = 0; nIndex < nEntities; nIndex++)
		{
			//Check if this Space is colliding with the entity
			if (IsColliding(nIndex))
			{
				//if it is then add the uid as the space for the entity
				m_pEntityMngr->AddSpace(nIndex, m_uID);
			}
		}
	}
}
#pragma region DOES NOT NEED CHANGES
void Space::Init(void)
{
	m_uID = 0;
	m_uLevel = 0;
	m_uChildren = 0;

	m_pRigidBody = nullptr;
	m_pChild[8] = { nullptr };

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();
}
void Space::Swap(Space& other)
{
	std::swap(m_uID, other.m_uID);
	std::swap(m_uLevel, other.m_uLevel);
	std::swap(m_uChildren, other.m_uChildren);

	std::swap(m_pRigidBody, other.m_pRigidBody);
	for (uint uChild = 0; uChild < m_uChildren; uChild++)
	{
		m_pChild[uChild] = other.m_pChild[uChild];
	}

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();
}
void Space::Release(void)
{
	//recursive behavior, get to the children first then release root
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		m_pChild[nChild]->Release();
	}
	//Once a Leaf is reach release its content
	SafeDelete(m_pRigidBody);
	
	for (uint nChild = 0; nChild < m_uChildren; nChild++)
	{
		if (m_pChild[nChild] != nullptr)
		{
			delete m_pChild[nChild];
			m_pChild[nChild] = nullptr;
		}
	}

	m_uChildren = 0;

	//at root make sure to release the entity manager
	if (m_uLevel == 0)
	{
		uint uCount = m_pEntityMngr->GetEntityCount();
		for (uint i = 0; i < uCount; i++)
		{
			Entity* pEntity = m_pEntityMngr->GetEntity(i);
			pEntity->ClearSpaceSet();
		}
	}
}
//The big 3
Space::Space(Space const& other)
{
	m_uID = other.m_uID;
	m_uLevel = other.m_uLevel;
	m_uChildren = other.m_uChildren;
	
	m_pRigidBody = other.m_pRigidBody;
	for (uint uChild = 0; uChild < m_uChildren; uChild++)
	{
		m_pChild[uChild] = other.m_pChild[uChild];
	}

	m_pModelMngr = ModelManager::GetInstance();
	m_pEntityMngr = EntityManager::GetInstance();
}
Space& Space::operator=(Space const& other)
{
	if (this != &other)
	{
		Release();
		Init();
		Space temp(other);
		Swap(temp);
	}
	return *this;
}

void Space::Display(uint a_nIndex)
{
	//If a value larger than the count of Spaces is sent render all
	if (a_nIndex > m_uSpaceCount)
	{
		Display();
		return;
	}

	//Traverse the children
	for (uint uChild = 0; uChild < m_uChildren; uChild++)
	{
		if (m_pChild[uChild])
		{
			m_pChild[uChild]->Display(a_nIndex);//recursive behavior
		}
	}

	//Once the specified index is found, render it
	if (m_uID == a_nIndex)
	{
		vector3 v3Center = m_pRigidBody->GetCenterGlobal();
		vector3 v3Size = m_pRigidBody->GetHalfWidth() * 2.0f;
		//render the wire cube
		m_pModelMngr->AddSpecialToRenderList(
			BTX::eBTX_SPECIALMODEL::CUBEWHITE,
			glm::translate(v3Center) * glm::scale(v3Size));
	}
}
void Space::Display()
{
	//Traverse the children
	for (uint uChild = 0; uChild < m_uChildren; uChild++)
	{
		if (m_pChild[uChild])
		{
			m_pChild[uChild]->Display(); //recursive behavior
		}
	}

	//Render the space
	vector3 v3Center = m_pRigidBody->GetCenterGlobal();
	vector3 v3Size = m_pRigidBody->GetHalfWidth() * 2.0f;
	//render the wire cube
	m_pModelMngr->AddSpecialToRenderList(
		BTX::eBTX_SPECIALMODEL::CUBEWHITE,
		glm::translate(v3Center) * glm::scale(v3Size));
}
Space::~Space() { Release(); };
//Accessors
vector3 Space::GetCenterGlobal(void) { return m_pRigidBody->GetCenterGlobal(); }
vector3 Space::GetMinGlobal(void) { return m_pRigidBody->GetMinGlobal(); }
vector3 Space::GetMaxGlobal(void) { return m_pRigidBody->GetMaxGlobal(); }
bool Space::IsLeaf(void) { return m_uChildren == 0; } //Is it childless?
uint Space::GetSpaceCount(void) { return m_uSpaceCount; }
#pragma endregion