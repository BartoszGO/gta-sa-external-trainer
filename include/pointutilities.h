#pragma once

struct vec2 {
	float x, y;
};

struct vec3 {
	float x, y, z;
};

vec3 RotatePoint(float cx, float cy, float angle, vec3 p);
