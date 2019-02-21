/*
 * sphere.cpp
 *
 * Using method described:
 * http://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c
 * and various other places.
 *
 * From university's example code.
 */

#ifndef SPHERE_H
#define SPHERE_H

class Sphere {
public:

	/**
	 * Generates a new sphere mesh with given radius, vertical sub-divisions and horizontal
	 * sub-divisions. Produces vertex positions, normal vectors, and element
	 * indices available for use in arrays.
	 * More sub-divisions generates a smoother sphere.
	 * It is a good idea to have vertical and horizontal divisions equal.
	 * @param radius, radius of sphere in generic units
	 * @param vertDiv, (min 1), number of vertical divisions (bands)
	 * @param horzDiv, (min 4), number of horizontal divisions (slices)
	 */
	Sphere(bool negNorms = false, float radius = 1.0f, int vertDiv = 16, int horzDiv = 16);
	~Sphere();

	float				*vertices;		// Vertex position (x,y,z)
	float				*normals;		// Normal vector (x,y,z)
	float				*texcoords;		// Tex coords s, t
	unsigned int		*indices;		// Element indices

	// Counts of array elements
	int					vertCount;
	int					normCount;
	int					texCount;
	int					indCount;

};

#endif
