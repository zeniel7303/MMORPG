#pragma once

#pragma once
#include <iostream>

#define SIZE 100

//√‚√≥ :
//https://gist.github.com/xstherrera1987/3196485

class CircBuf
{
private:
	char* data[SIZE];
	int front;
	int end;
	int count;

public:
	CircBuf()
	{
		front = 0;
		end = 0;
		count = 0;

		for (int i = 0; i < SIZE; i++)
		{
			data[i] = {};
		}
	}
	~CircBuf()
	{
		//delete data;
	}

	bool IsEmpty()
	{
		if (front == end)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool IsFull()
	{
		int temp = (end + 1) % SIZE;

		if (temp == front)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Add(char* _ptr)
	{
		if (IsFull()) {
			Remove();
			Add(_ptr);
		}
		else {
			count++;
			end = (end + 1) % SIZE;
			data[end] = _ptr;
		}
	}

	char* Remove()
	{
		if (!IsEmpty()) {
			count--;

			front = (front + 1) % SIZE;
			return data[front];
		}
	}

	int GetCount() { return count; }
	int GetFront() { return front; }
	int GetEnd() { return end; }

	char* GetBuffer(int num) { return data[num]; }
	char* GetFrontBuffer() { return data[front]; }
	char* GetEndBuffer() { return data[end]; }
};