/*--------------------------------------------------------------------------------------------------
Made by: Alberto Bobadilla
on: 2024/01/23
--------------------------------------------------------------------------------------------------*/
#ifndef __MYQUEUE_H_
#define __MYQUEUE_H_
#include <iostream>
#include "Alberto.h"
#define uint unsigned int
template <typename T>
class MyQueue
{
public:
	MyQueue()
	{
		//Constructor
		Init();
	}
	MyQueue(MyQueue const& other)
	{
		//Copy constructor
		stackArray = new T[other.stackArraySize];
		stackArraySize = other.stackArraySize;
		stackArrayCapacity = other.stackArrayCapacity;

		for (int i = 0; i < stackArraySize; i++)
		{
			stackArray[i] = other.stackArray[i];
		}

	}
	MyQueue& operator=(MyQueue const& other)
	{
		//Copy Assignment Operator

		if (this != &other)
		{
			stackArraySize = other.stackArraySize;
			stackArrayCapacity = other.stackArrayCapacity;

			for (int i = 0; i < stackArraySize; i++)
			{
				stackArray[i] = other.stackArray[i];
			}
		}
		return *this;
	}
	~MyQueue()
	{
		//Destructor
		Release();
	}
	bool IsEmpty()
	{
		//If the size of the array is greater than zero return false else true
		if (stackArraySize > 0)
		{
			return false;
		}
		return true;
	}
	//Refresher on dynamic arrays
	//https://www.youtube.com/watch?v=ryRf4Jh_YC0&list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb&index=92&ab_channel=TheCherno
	void Push(T a_fInput)
	{
		//Push
		//Check if the size of the array is greater than or equal to the capacity and if so increase the capacity
		if (stackArraySize >= stackArrayCapacity) {
			ReAlloc(stackArrayCapacity * 2);
		}
		
		//Add new item to array and update size
		stackArray[stackArraySize] = a_fInput;
		stackArraySize++;

		//Sort
		//Sort array items after adding the newest one and updating the array size
		Sort(stackArray, stackArraySize);
	}
	//Basis for my sort
	//https://www.youtube.com/watch?v=0w2l1Vzcf9U&ab_channel=VinodPillai
	void Sort(T array[], unsigned int size)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = i + 1; j < size; j++)
			{
				if (array[i] > array[j])
				{
					std::swap(array[i], array[j]);
				}
			}
		}
	}
	void Pop()
	{
		//Take from the front
		if (stackArraySize > 0)
		{
			//To remove from the front take the last element of the array and move it to the front
			//This removes the popped item and keeps last item that is now out of bounds after the size is lowered
			//Sorting afterwards arranges the remaining items to appear as though the front item was removed
			stackArraySize--;
			stackArray[0] = stackArray[stackArraySize];
			Sort(stackArray, stackArraySize);
		}
	}
	void Print()
	{
		//Print the content
		if (!IsEmpty())
		{
			std::cout << "Entries: ";
			std::cout << GetSize();
			std::cout << " -> ";
			for (unsigned int i = 0; i < stackArraySize; i++)
			{
				if (i == stackArraySize - 1)
				{
					std::cout << stackArray[i];
				}
				//Add comma for all items except the last
				else
				{
					std::cout << stackArray[i];
					std::cout << ", ";
				}
			}
		}
		else
		{
			std::cout << "Queue is Empty";
		}
	}
	uint GetSize()
	{
		return stackArraySize;
	}
	friend std::ostream& operator<<(std::ostream& os, MyQueue& other)
	{
		os << "This is a line" << std::endl;
		return os;
	}
private:
	void ReAlloc(unsigned int newCapacity)
	{
		// Allocate new memory
		// Move elements from old array to new
		// Delete
		
		T* newBlock = new T[newCapacity];

		if (newCapacity < stackArraySize) {
			stackArraySize = newCapacity;
		}
		
		//Make a deep copy of the original array to the expanded array
		for (unsigned int i = 0; i < stackArraySize; i++) {
			newBlock[i] = stackArray[i];
		}

		delete[] stackArray;
		stackArray = newBlock;
		stackArrayCapacity = newCapacity;
	}
	void Init()
	{
		//Setup variables 
		T* stackArray = nullptr;

		unsigned int stackArraySize = 0;
		unsigned int stackArrayCapacity = 0;

		ReAlloc(1);
	}
	void Release()
	{
		delete[] stackArray;
	}
	void Swap(T &firstVal, T &secondVal)
	{
		T temp = firstVal;
		firstVal = secondVal;
		secondVal = temp;
	}

private: 
	// Variable Decleration
	T* stackArray = nullptr;

	unsigned int stackArraySize = 0;
	unsigned int stackArrayCapacity = 0;
};
#endif //__MYQUEUE_H_
