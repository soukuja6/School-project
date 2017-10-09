
#include "Header.h"
using namespace std;


// ************************************************************************************************
// *** OpenGL callbacks implementation ************************************************************

// Called whenever the scene has to be drawn
void Programm::display() {
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	glViewport(0, 0, width, height);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Enable the shader program
	assert(ShaderProgram != 0);
	glUseProgram(ShaderProgram);

	// Set the camera transformation
	Cam.ar = (1.0f * width) / height;
	Matrix4f transformation = Cam.computeCameraTransform();

	//SET SHADER VARIABLES
	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, transformation.get());
	glUniform3fv(CameraPositionLoc, 1, Cam.position.get());
	glUniform3f(MaterialAColorLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(MaterialDColorLoc, 1.0f, 0.8f, 0.8f);
	glUniform3f(MaterialSColorLoc, 0.5f, 0.5f, 0.5f);
	glUniform1f(MaterialShineLoc, 20.0f);

	if (directional) {  //if the directional light is enabled
		glUniform1ui(DirectionalLoc, true);
		glUniform3fv(dLight.dLightDirLoc,1, dLight.dLightDir.get());
		//glUniform3f(DLight.DLightDirLoc, 0.5f, 0.5f, 0.5f);
		glUniform3fv(dLight.lightAColorLoc, 1,dLight.lightAColor.get());
		glUniform3fv(dLight.lightDColorLoc, 1,dLight.lightDColor.get());
		glUniform3fv(dLight.lightSColorLoc, 1,dLight.lightSColor.get());
		glUniform1f(dLight.lightAIntensityLoc, dLight.lightAIntensity);
		glUniform1f(dLight.lightDIntensityLoc, dLight.lightDIntensity);
		glUniform1f(dLight.lightSIntensityLoc, dLight.lightSIntensity);
	}
	else {     //otherwise it is head mounted light
		glUniform1ui(DirectionalLoc, false);
		glUniform1f(pLight.klinearloc, pLight.klinear);
		glUniform1f(pLight.ksquaredloc, pLight.ksquared);
		glUniform3f(pLight.lightAColorLoc, pLight.lightAColor.x(), pLight.lightAColor.y(), pLight.lightAColor.z());
		glUniform3f(pLight.lightDColorLoc, pLight.lightDColor.x(), pLight.lightDColor.y(), pLight.lightDColor.z());
		glUniform3f(pLight.lightSColorLoc, pLight.lightSColor.x(), pLight.lightSColor.y(), pLight.lightSColor.z());
		glUniform1f(pLight.lightAIntensityLoc, pLight.lightAIntensity);
		glUniform1f(pLight.lightDIntensityLoc, pLight.lightDIntensity);
		glUniform1f(pLight.lightSIntensityLoc, pLight.lightSIntensity);
	}


	// Enable the vertex attributes and set their format
	glEnableVertexAttribArray(0);        //LOCATION is first
	glEnableVertexAttribArray(2);        //normal is third
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(0));

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(float[3])+sizeof(float[2])));

	/*
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2,	GL_FLOAT, GL_FALSE,
	sizeof(ModelOBJ::Vertex),
	reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));*/

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	// Draw the elements on the GPU
	glDrawElements(
		GL_TRIANGLES,
		Model.getNumberOfIndices(),
		GL_UNSIGNED_INT,
		0);

	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);


	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Lock the mouse at the center of the screen
	glutWarpPointer(MouseX, MouseY);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

// Called at regular intervals (can be used for animations)
void Programm::idle() {
}


// Called whenever a keyboard button is pressed (only ASCII characters)
void Programm::keyboard(unsigned char key, int x, int y) {
	Vector3f right;

	switch (tolower(key)) {
		// --- camera movements ---
	case 'w':
		Cam.position += Cam.target * 0.5f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.5f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.5f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.5f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.5f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.5f;
		break;
	case 'r': // Reset camera status
		Cam.Reset();
		break;

	case 'i': //change perspective
		if (Cam.projection == Projections::Perspective) {
			Cam.projection = Projections::Orthogonal;
		}
		else
			Cam.projection = Projections::Perspective;
		break;
		// --- utilities ---

	case 'p': // change to wireframe rendering
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'l': // change light option
		directional = !directional;
		break;

	case 'o': // change to polygon rendering
		glPolygonMode(GL_FRONT, GL_FILL);
		break;

	case 'g': // show the current OpenGL version
		cout << "OpenGL version " << glGetString(GL_VERSION) << endl;
		break;
	case 'q':  // terminate the application
		exit(0);
		break;
	}
	// redraw
	glutPostRedisplay();
}

// Called whenever a special keyboard button is pressed
void Programm::special(int key, int x, int y) {
	Vector3f right;

	switch (key) {
		// --- camera movements ---
	case GLUT_KEY_PAGE_UP:	// Increase field of view
		Cam.fov = min(Cam.fov + 1.f, 179.f);
		break;
	case GLUT_KEY_PAGE_DOWN:	// Decrease field of view
		Cam.fov = max(Cam.fov - 1.f, 1.f);
		break;

		// --- utilities ---
	case GLUT_KEY_F5:	// Reload shaders
		cout << "Re-loading shaders..." << endl;

		if (LoadShaders("shader.v.glsl", "shader.f.glsl")) {
			cout << "> done." << endl;
		}
		else {
			cout << "> not possible." << endl;
		}
		break;
	}
	// redraw
	glutPostRedisplay();
}

// Called whenever a mouse event occur (press or release)
void Programm::mouse(int button, int state, int x, int y) {
	// Store the current mouse status
	MouseButton = button;

	// Instead of updating the mouse position, lock it at the center of the screen
	MouseX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	MouseY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	glutWarpPointer(MouseX, MouseY);
}

// Called whenever the mouse is moving while a button is pressed
void Programm::motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Cam.position += Cam.target * 0.015f * (MouseY - y);
		Cam.position += Cam.target.cross(Cam.up) * 0.003f * (x - MouseX);
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Cam.zoom = std::max(0.001f, Cam.zoom + 0.0153f * (y - MouseY));
	}
	if (MouseButton == GLUT_LEFT_BUTTON) {
		Matrix4f ry, rr;

		// "horizontal" rotation
		ry.rotate(0.05f * (MouseX - x), Vector3f(0, 1, 0));
		Cam.target = ry * Cam.target;
		Cam.up = ry * Cam.up;

		// "vertical" rotation
		rr.rotate(0.05f * (MouseY - y), Cam.target.cross(Cam.up));
		Cam.up = rr * Cam.up;
		Cam.target = rr * Cam.target;
	}

	// Redraw the scene
	glutPostRedisplay();
}
