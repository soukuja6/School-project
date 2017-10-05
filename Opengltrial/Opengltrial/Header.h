
#ifndef kk
#define kk



#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <fstream>
#include <string>
#include <Algorithm>

#include "Matrix4.h"
#include "model_obj.h"
#include "Vector3.h"
#include "Shaders.h"


enum Projections
{
	Orthogonal,
	Perspective
};


struct Camera {
	Vector3f position;	// the position of the camera
	Vector3f target;	// the direction the camera is looking at
	Vector3f up;		// the up vector of the camera

	float fov;			// camera field of view
	float ar;			// camera aspect ratio

	float zNear, zFar;	// depth of the near and far plane

	float zoom;			// an additional scaling parameter

	//Compute camera transformation matrix
	Matrix4f computeCameraTransform();

	//Reset camera to default settings
	void Reset();

	Projections projection=Perspective;


};

class Programm {
public:
	Programm() {
		Cam.Reset();
	}


	// ************************************************************************************************
	// *** OpenGL callbacks definition ************************************************************

	void display();
	void keyboard(unsigned char key, int x, int y);
	void idle();
	void special(int key, int x, int y);
	void mouse(int, int, int, int);
	void motion(int, int);
	

	// *****************Othrer method definition************************

	

	///<summary>
	/// Initialize buffer objects
	///</summary>
	bool InitMesh(std::string location);

	///<summary>
	///load shaders
	///<summary>
	bool LoadShaders(std::string vertexshader, std::string fragmentshader);
	


	// 3D model
	ModelOBJ Model;		// A 3D model
	GLuint VBO=0;	// A vertex buffer object
	GLuint IBO=0;	// An index buffer object

						
	GLuint ShaderProgram=0;	// A shader program

    // Vertex transformation
	Vector3f Translation;	// Translation
	float Scaling;			///< Scaling

	// Mouse interaction
	int MouseX, MouseY;		//The last position of the mouse
	int MouseButton;		//The last mouse button pressed or released
	Camera Cam;
	GLint TrLocation = -1;  //location of shader transformation variable
};
#endif // !kk