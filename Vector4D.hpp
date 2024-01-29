#pragma once
#include <cmath>

struct Vector4D {
	float x;
	float y;
	float z;
	float w;

	Vector4D() {};

	Vector4D(float _x, float _y, float _z, float _w) { 
		x = _x; 
		y = _y; 
		z = _z; 
		w = _w; 
	}

	float operator[](int i) const;
	float& operator[](int i);

	float length() {
		return std::sqrt(x*x + y*y + z*z + w*w);
	}

	float distance(const Vector4D& o) {
		return std::sqrt(std::pow(x - o.x, 2) + pow(y - o.y, 2) + pow(z - o.z, 2) + pow(w - o.w, 2));
	}

	Vector4D vscale(const Vector4D& s) {
		return Vector4D(x*s.x, y*s.y, z*s.z, w*s.w);
	}

	Vector4D scale(float s) {
		return Vector4D(x*s, y*s, z*s, w*s);
	}

	Vector4D normalize() {
		float l = length();
		return Vector4D(x / l, y / l, z / l, w / l);
	}

	Vector4D add(const Vector4D& o) {
		return Vector4D(x + o.x, y + o.y, z + o.z, w + o.w);
	}

	Vector4D sub(const Vector4D& o) {
		return Vector4D(x - o.x, y - o.y, z - o.z, w - o.w);
	}

	Vector4D clone() {
		return Vector4D(x, y, z, w);
	}
};