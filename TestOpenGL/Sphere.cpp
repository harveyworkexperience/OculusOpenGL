
/*
 * sphere.cpp
 *
 * Using method described:
 * http://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c
 * and various other places.
 */

#include <math.h>

#include "Sphere.h"

#define M_PI		3.14159265358979323846264338327950288
#define M_PI_2		1.57079632679489661923

Sphere::Sphere(bool negNorms, float radius, int vertDiv, int horzDiv)
{
	// Force minimum values
	if (vertDiv < 1) vertDiv = 1;
	if (horzDiv < 4) horzDiv = 4;

	vertDiv = vertDiv + 2;	// Add 2 for top and bottom-cap 'divisions'
	float V = 1.0 / (float)(vertDiv - 1);
	float H = 1.0 / (float)(horzDiv - 1);
	int v, h;

	vertices = new float[vertDiv*horzDiv * 3];
	normals = new float[vertDiv*horzDiv * 3];
	texcoords = new float[vertDiv*horzDiv * 2];
	indices = new unsigned int[vertDiv*horzDiv * 6];

	vertCount = 0;
	normCount = 0;
	indCount = 0;
	texCount = 0;

	// Construct vertex, normal and tex-coords
	for (v = 0; v < vertDiv; v++) {
		for (h = 0; h < horzDiv; h++) {
			float x = cos(2 * M_PI * h * H) * sin(M_PI * v * V);
			float y = sin(-M_PI_2 + M_PI * v * V);
			float z = sin(2 * M_PI * h * H) * sin(M_PI * v * V);

			vertices[vertCount++] = x * radius;
			vertices[vertCount++] = y * radius;
			vertices[vertCount++] = z * radius;

			float symbol = 1.0f;
			if (negNorms)
				symbol = -1.0f;

			normals[normCount++] = x * symbol;
			normals[normCount++] = y * symbol;
			normals[normCount++] = z * symbol;

			texcoords[texCount++] = 1.0 - (h * H);
			texcoords[texCount++] = 1.0 - (v * V);
		}
	}

	// Construct indices
	// Start with special case of bottom sub-division where all vertices meet
	v = 0;
	for (h = 0; h < horzDiv - 1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v + 1) * horzDiv + h;
		indices[indCount++] = (v + 1) * horzDiv + (h + 1);
	}

	// Middle divisions
	for (v = 1; v < vertDiv - 2; v++) {
		for (h = 0; h < horzDiv - 1; h++) {

			indices[indCount++] = v * horzDiv + h;
			indices[indCount++] = (v + 1) * horzDiv + (h + 1);
			indices[indCount++] = v * horzDiv + (h + 1);

			indices[indCount++] = v * horzDiv + h;
			indices[indCount++] = (v + 1) * horzDiv + h;
			indices[indCount++] = (v + 1) * horzDiv + (h + 1);
		}
	}

	// Cap off the top of the sphere
	v = vertDiv - 2;
	for (h = 0; h < horzDiv - 1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v + 1) * horzDiv + (h + 1);
		indices[indCount++] = v * horzDiv + (h + 1);
	}

}

Sphere::~Sphere() {
	delete(vertices);
	delete(normals);
	delete(texcoords);
	delete(indices);
}