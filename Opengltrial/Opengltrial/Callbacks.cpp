
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

	Matrix4f vertextoworldtr;
	vertextoworldtr.identity();
	Matrix4f normaltr;
	normaltr.identity();

	//SET SHADER VARIABLES
	glUniformMatrix4fv(TrLocation, 1, GL_FALSE, transformation.get());
	glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
	glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());

	glUniform3fv(CameraPositionLoc, 1, Cam.position.get());
	glUniform3fv(CameraDirLoc, 1, Cam.target.get());
	

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
		glUniform1f(pLight.anglerestictionloc, pLight.anglerestriction);
		glUniform3f(pLight.lightAColorLoc, pLight.lightAColor.x(), pLight.lightAColor.y(), pLight.lightAColor.z());
		glUniform3f(pLight.lightDColorLoc, pLight.lightDColor.x(), pLight.lightDColor.y(), pLight.lightDColor.z());
		glUniform3f(pLight.lightSColorLoc, pLight.lightSColor.x(), pLight.lightSColor.y(), pLight.lightSColor.z());
		glUniform1f(pLight.lightAIntensityLoc, pLight.lightAIntensity);
		glUniform1f(pLight.lightDIntensityLoc, pLight.lightDIntensity);
		glUniform1f(pLight.lightSIntensityLoc, pLight.lightSIntensity);
	}


	// Enable the vertex attributes and set their format
	GLuint SamplerLoc = glGetUniformLocation(ShaderProgram, "sampler");

	// Set the uniform variable for the texture unit (texture unit 0)
	glUniform1i(SamplerLoc, 0);



	// Enable the vertex attributes and set their format
	GLuint bumpSamplerLoc = glGetUniformLocation(ShaderProgram, "bumpsampler");

	// Set the uniform variable for the texture unit (texture unit 1)
	glUniform1i(bumpSamplerLoc, 1);

	// Enable texture unit 1 and bind the texture to it
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bumptexture.TextureObject);

	// Bind the buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glEnableVertexAttribArray(0);        //LOCATION is first
	glEnableVertexAttribArray(1);        //texturecoor
	glEnableVertexAttribArray(2);        //normal is third
	glEnableVertexAttribArray(3);        //tangent is fourth
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(0));

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(float[3])+sizeof(float[2])));

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])+ sizeof(float[3])));

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(ModelOBJ::Vertex),
		reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));

	// Enable texture unit 0 and bind the texture to it
	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, maintexture[3].TextureObject);

	
	for (size_t i = 0; i < Model.getNumberOfMeshes(); i++)            // draw every mesh separately
	{
		auto && actmesh = Model.getMesh(i);
		int j = 0;
		for (j = 0; j < Model.getNumberOfMaterials(); j++)
		{
			if (&Model.getMaterial(j) == actmesh.pMaterial) {
				break;
			}
		}

		glUniform1ui(Bumploc, i==0);											// only first mesh will have applied bump on it
		glUniform3fv(MaterialAColorLoc,1, Model.getMaterial(j).ambient);
		glUniform3fv(MaterialDColorLoc,1, Model.getMaterial(j).diffuse);
		glUniform3fv(MaterialSColorLoc,1, Model.getMaterial(j).specular);
		glUniform1f(MaterialShineLoc, 20);										//shininess from blender is set to almost zero, which more or less destroy the picture so we set it manually
		
		glBindTexture(GL_TEXTURE_2D, maintexture[j].TextureObject);

		glDrawElements(
			GL_TRIANGLES,
			actmesh.triangleCount * 3,
			GL_UNSIGNED_INT,
			(void*)(actmesh.startIndex * sizeof(GLuint)));

	}

	//------------------------------------------------- draw gandalf gif--------------------------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, VBOT);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOT);

	glUniform1ui(Bumploc, false);
	// Enable texture unit 0 and bind the texture to it
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cinematexture[(int)(difference * cinematexture.size())].TextureObject);
	//cout << (int)(difference * cinematexture.size()) << endl;

	vertextoworldtr = Matrix4f::createTranslation(Vector3f{ 0.0, 0.0, -2.0 });
	glUniformMatrix4fv(PointTrLocation, 1, GL_FALSE, vertextoworldtr.get());
	glUniformMatrix4fv(NormalTrLocation, 1, GL_FALSE, normaltr.get());
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(MyVertex),
		reinterpret_cast<const GLvoid*>(0));

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(MyVertex),
		reinterpret_cast<const GLvoid*>(sizeof(float[3]) + sizeof(float[2])));

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(MyVertex),
		reinterpret_cast<const GLvoid*>(sizeof(Vector3f)));
	// Bind the buffers
	
	glDrawElements(
		GL_TRIANGLES,
		2 * 3,
		GL_UNSIGNED_INT,
		0);
		


	// Disable the vertex attributes (not necessary but recommended)
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);


	// Disable the shader program (not necessary but recommended)
	glUseProgram(0);

	// Lock the mouse at the center of the screen
	//glutWarpPointer(MouseX, MouseY);

	// Swap the frame buffers (off-screen rendering)
	glutSwapBuffers();
}

// Called at regular intervals (can be used for animations)
void Programm::idle() {
	clock_t now = clock();
	difference += (double)(now- lasttime)/CLOCKS_PER_SEC /5;
	if (difference >= 1.0)
		difference -= floor(difference);

	//cout << difference << endl;
	lasttime = now;
	glutPostRedisplay();
}


// Called whenever a keyboard button is pressed (only ASCII characters)
void Programm::keyboard(unsigned char key, int x, int y) {
	Vector3f right;

	switch (tolower(key)) {
		// --- camera movements ---
	case 'w':
		Cam.position += Cam.target * 0.1f;
		break;
	case 'a':
		right = Cam.target.cross(Cam.up);
		Cam.position -= right * 0.1f;
		break;
	case 's':
		Cam.position -= Cam.target * 0.1f;
		break;
	case 'd':
		right = Cam.target.cross(Cam.up);
		Cam.position += right * 0.1f;
		break;
	case 'c':
		Cam.position -= Cam.up * 0.1f;
		break;
	case ' ':
		Cam.position += Cam.up * 0.1f;
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

	case 'k': //change shader
		if (shadertype == Shadertype::FragmentIllumination) {
			if (LoadShaders("shader.v.glsl", "shader.f.glsl")) {
				shadertype = Shadertype::Vertexillumination;
				cout << "> done." << endl;
			}
			else {
				cout << "> not possible." << endl;
			}
		}
		else {
			if (LoadShaders("Vertexshader.glsl", "Fragmentshader.glsl")) {
				cout << "> done." << endl;
				shadertype = Shadertype::FragmentIllumination;
			}
			else {
				cout << "> not possible." << endl;
			}
			
		}break;
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
	//MouseX = glutGet(GLUT_WINDOW_WIDTH) / 2;
	//MouseY = glutGet(GLUT_WINDOW_HEIGHT) / 2;
	//glutWarpPointer(MouseX, MouseY);aaa

	MouseX = x;
	MouseY = y;
}

// Called whenever the mouse is moving while a button is pressed
void Programm::motion(int x, int y) {
	if (MouseButton == GLUT_RIGHT_BUTTON) {
		Cam.position += Cam.target * 0.015f * (MouseY - y);
		Cam.position += Cam.target.cross(Cam.up) * 0.0015f * (x - MouseX);
		MouseX = x; // Store the current mouse position
		MouseY = y;
	}
	if (MouseButton == GLUT_MIDDLE_BUTTON) {
		Cam.zoom = std::max(0.001f, Cam.zoom + 0.0153f * (y - MouseY));
		MouseX = x; // Store the current mouse position
		MouseY = y;
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

		MouseX = x; // Store the current mouse position
		MouseY = y;
	}

	// Redraw the scene
	glutPostRedisplay();
}
