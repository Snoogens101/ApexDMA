#pragma once
#include "Vector3D.hpp"

struct Matrix3x4 {
public:
	float matrix[3][4];

	Vector3D GetPosition() const {
		return Vector3D(matrix[0][3], matrix[1][3], matrix[2][3]);
	}

	Vector3D GetPosition2() const {
		return Vector3D(matrix[0][3], matrix[1][3], matrix[2][3]);
	}
};

struct ViewMatrix {
public:
	float matrix[4][4];

	Vector3D Transform(const Vector3D vector) const {
		Vector3D transformed;

		transformed.x = vector.y * matrix[0][1] + vector.x * matrix[0][0] + vector.z * matrix[0][2] + matrix[0][3];
	    transformed.y = vector.y * matrix[1][1] + vector.x * matrix[1][0] + vector.z * matrix[1][2] + matrix[1][3];
	    transformed.z = vector.y * matrix[3][1] + vector.x * matrix[3][0] + vector.z * matrix[3][2] + matrix[3][3];

		return transformed;
	}
};