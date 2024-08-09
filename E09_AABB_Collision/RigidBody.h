/*----------------------------------------------
Programmer: Alberto Bobadilla (labigm@gmail.com)
Date: 2023/10
----------------------------------------------*/
#ifndef __MYRIGIDBODY_H_
#define __MYRIGIDBODY_H_

#include "BTX\BTX.h"
#include "BTX\TypeDefs.h"

//System Class
class RigidBody
{
	BTX::ModelManager* m_pMeshMngr = nullptr; //for displaying the Rigid Body

	bool m_bVisibleAABB = true; //Visibility
	bool m_bVisibleBS = false; //Visibility

	float m_fRadius = 0.0f; //Radius

	vector3 m_v3ColorAABB = C_WHITE; //Color
	vector3 m_v3ColorBS = C_WHITE; //Color

	vector3 m_v3Center = vector3(0.0f); //center point in local space
	vector3 m_v3CenterG = vector3(0.0f); //center point in global space

	vector3 m_v3MinL = vector3(0.0f); //minimum coordinate in local space
	vector3 m_v3MaxL = vector3(0.0f); //maximum coordinate in local space

	vector3 m_v3MinG = vector3(0.0f); //minimum coordinate in global space
	vector3 m_v3MaxG = vector3(0.0f); //maximum coordinate in global space

	vector3 m_v3HalfWidth = vector3(0.0f); //half the size of all sides

	matrix4 m_m4ToWorld = IDENTITY_M4; //Matrix that will take us from local to world coordinate

public:
	/*
	Usage: Constructor
	Arguments: ---
	Output: class object instance
	*/
	RigidBody(std::vector<vector3> a_pointList);
	/*
	Usage: Copy Constructor
	Arguments: class object to copy
	Output: class object instance
	*/
	RigidBody(RigidBody const& other);
	/*
	Usage: Copy Assignment Operator
	Arguments: class object to copy
	Output: ---
	*/
	RigidBody& operator=(RigidBody const& other);
	/*
	Usage: Destructor
	Arguments: ---
	Output: ---
	*/
	~RigidBody(void);
	/*
	Usage: Changes object contents for other object's
	Arguments: other -> object to swap content from
	Output: ---
	*/
	void Swap(RigidBody& other);

	/*
	USAGE: Will render the Rigid Body at the model to world matrix
	ARGUMENTS: ---
	OUTPUT: ---
	*/
	void AddToRenderList(void);

	/*
	USAGE: Tells if the object is colliding with the incoming one
	ARGUMENTS: ---
	OUTPUT: are they colliding?
	*/
	bool IsColliding(RigidBody* const other);

#pragma region Accessors
	/*
	Usage: Gets radius
	Arguments: ---
	Output: radius
	*/
	float GetRadius(void);
	/*
	Usage: Sets the color of the AABB
	Arguments: vector3 a_v3Color -> color
	Output: ---
	*/
	void SetColorAABB(vector3 a_v3Color);
	/*
	Usage: Sets the color of the BS
	Arguments: vector3 a_v3Color -> color
	Output: ---
	*/
	void SetColorBS(vector3 a_v3Color);
	/*
	Usage: Gets center in local space
	Arguments: ---
	Output: center
	*/
	vector3 GetCenterLocal(void);
	/*
	Usage: Gets minimum vector in local space
	Arguments: ---
	Output: min vector
	*/
	vector3 GetMinLocal(void);
	/*
	Usage: Gets maximum vector in local space
	Arguments: ---
	Output: max vector
	*/
	vector3 GetMaxLocal(void);
	/*
	Usage: Gets center in global space
	Arguments: ---
	Output: center
	*/
	vector3 GetCenterGlobal(void);
	/*
	Usage: Gets minimum vector in local space
	Arguments: ---
	Output: min vector
	*/
	vector3 GetMinGlobal(void);
	/*
	Usage: Gets max vector in global space
	Arguments: ---
	Output: max vector
	*/
	vector3 GetMaxGlobal(void);
	/*
	Usage: Gets the size of the model divided by 2
	Arguments: ---
	Output: halfwidth vector
	*/
	vector3 GetHalfWidth(void);
	/*
	Usage: Gets Model to World matrix
	Arguments: ---
	Output: model to world matrix
	*/
	matrix4 GetModelMatrix(void);
	/*
	Usage: Sets Model to World matrix
	Arguments: Model to World matrix
	Output: ---
	*/
	void SetModelMatrix(matrix4 a_m4ModelMatrix);
#pragma endregion
	
private:
	/*
	Usage: Deallocates member fields
	Arguments: ---
	Output: ---
	*/
	void Release(void);
	/*
	Usage: Allocates member fields
	Arguments: ---
	Output: ---
	*/
	void Init(void);
};//class


#endif //__MYRIGIDBODY_H_

/*
USAGE:
ARGUMENTS: ---
OUTPUT: ---
*/