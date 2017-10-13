
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

enum Shadertype {     // different types of shaders
	FragmentIllumination,
	Vertexillumination
};

enum Projections
{
	Orthogonal,
	Perspective
};


struct Light {
	Light() {
		Reset();
	}

	void Reset() {
		lightAColor = { 0.05f, 0.03f, 0.0f };
		lightDColor = { 0.5f, 0.4f, 0.3f };
		lightSColor = { 0.6f, 0.6f, 0.7f };
		lightAIntensity= 1.0f;
		lightDIntensity = 1.0f;
		lightSIntensity = 1.0f;
	}
	GLint lightAColorLoc = -1;
	GLint lightDColorLoc = -1;
	GLint lightSColorLoc = -1;
	GLint lightAIntensityLoc = -1;
	GLint lightDIntensityLoc = -1;
	GLint lightSIntensityLoc = -1;

	Vector3f lightAColor;
	Vector3f lightDColor;
	Vector3f lightSColor;
	float lightAIntensity = -1;
	float lightDIntensity = -1;
	float lightSIntensity = -1;
	
};

struct DirectionalLight : Light {
	DirectionalLight() {
		Reset();
	};

	void Reset() {
		Light::Reset();
		dLightDir = { 0.0f, -0.5f, -1.0f };
	}

	GLint dLightDirLoc=-1;
	Vector3f dLightDir;
};

struct HeadLight : Light {        //spot light mounted on head with restricted lightning angle and decreasing light intensity with distance
	HeadLight() {
		Reset();
	}
	void Reset() {
		Light::Reset();
		klinear = 0.01f;
		ksquared = 0.1f;
		anglerestriction = 10;
	}

	float klinear;
	float ksquared;
	int anglerestriction;

	GLint anglerestictionloc = -1;
	GLint klinearloc=-1;
	GLint ksquaredloc=-1;

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
	Matrix4f computeTransformToworldcoordinates();      //compute matrix that transform object to world coordinates
	Matrix4f computeNormalTransform();					//copute matrix that transform object'snormal to normals in wolrds coordinates

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
	Shadertype shadertype = FragmentIllumination;   //typeofshader currently used

	//Lights
	DirectionalLight dLight;
	HeadLight pLight;
	bool directional=true;
	

    // Vertex transformation
	Vector3f Translation;	// Translation
	float Scaling;			///< Scaling

	// Mouse interaction
	int MouseX, MouseY;		//The last position of the mouse
	int MouseButton;		//The last mouse button pressed or released
	Camera Cam;

	//Shader variables
	GLint TrLocation = -1;  //location of shader transformation variable
	//GLint PointTrLocation = -1;  //location of matrix trandofrming to wolrd coordinates
	//GLint NormalTrLocation = -1;  //location of matrix trandofrming normals to wolrd coordinates

	GLint DirectionalLoc = -1;
	GLint CameraPositionLoc = -1;
	GLint CameraDirLoc = -1;              //position of camera direction(target) variable in shader
	GLint MaterialAColorLoc = -1;
	GLint MaterialDColorLoc = -1;
	GLint MaterialSColorLoc = -1;
	GLint MaterialShineLoc = -1;
};
#endif // !kk