/*
 * Types.hpp
 *
 *  Created on: 9 May 2016
 *      Author: sfriston
 */

#ifndef TYPES_HPP_
#define TYPES_HPP_

#include "ProjectOrion.h"
#include <vector>

/* We will typically use the oculus math library, but for components that shouldnt otherwise depend on it, this is a convenient
 * way to pass around quaternions */
struct quaternion
{
public:
	float s;
	float x;
	float y;
	float z;

	float* data()
	{
		return (float*)this;
	}
};

struct vector3
{
public:

	float x;
	float y;
	float z;

	vector3(){}
	vector3(float ix, float iy, float iz){ x = ix; y = iy; z = iz; }

	float* data()
	{
		return (float*)this;
	}

	vector3 normalised()
	{
		return vector3::normalise(*this);
	}

	static float norm(vector3 v)
	{
		float sum = v.x*v.x + v.y*v.y + v.z*v.z;
		return sqrt(sum);
	}

	static vector3 cross(vector3 a, vector3 b)
	{
		vector3 ans;
		ans.x = ((a.y*b.z) - (a.z*b.y));
		ans.y = ((a.z*b.x) - (a.x*b.z));
		ans.z = ((a.x*b.y) - (a.y*b.x));

		return ans;
	}

	static vector3 add(vector3 a, std::vector<float> b)
	{
		vector3 ans;
		ans.x = (a.x + b[0]);
		ans.y = (a.y + b[1]);
		ans.z = (a.z + b[2]);
		return ans;
	}

	static vector3 add(vector3 a, vector3 b)
	{
		vector3 ans;
		ans.x = (a.x + b.x);
		ans.y = (a.y + b.y);
		ans.z = (a.z + b.z);
		return ans;
	}

	static vector3 multiply(vector3 a, float b)
	{
		vector3 ans;
		ans.x = (a.x * b);
		ans.y = (a.y * b);
		ans.z = (a.z * b);
		return ans;
	}

	static vector3 divide(vector3 a, float b)
	{
		vector3 ans;
		ans.x = (a.x / b);
		ans.y = (a.y / b);
		ans.z = (a.z / b);
		return ans;
	}

	static vector3 normalise(vector3 a)
	{
		return vector3::divide(a, vector3::norm(a));
	}

	static vector3 one()
	{
		return vector3(1.0f, 1.0f, 1.0f);
	}

	static vector3 forward()
	{
		return vector3(0.0f, 0.0f, 1.0f);
	}

	static vector3 right()
	{
		return vector3(1.0f, 0.0f, 0.0f);
	}

	static vector3 left()
	{
		return vector3(-1.0f, 0.0f, 0.0f);
	}

	static vector3 up()
	{
		return vector3(0.0f, 1.0f, 0.0f);
	}

};

#endif /* TYPES_HPP_ */
