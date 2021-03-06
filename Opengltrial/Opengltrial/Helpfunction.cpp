
#include "Header.h"
using namespace std;


// Initialize buffer objects
bool Programm::InitMesh(string location) {
	// Load the OBJ model
	if (!Model.import("building\\building.obj")) {
		cerr << "Error: cannot load model." << endl;
		return false;
	}

	// Notice that normals may not be stored in the model
	// This issue will be dealt with in the next lecture

	// VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		Model.getNumberOfVertices() * sizeof(ModelOBJ::Vertex),
		Model.getVertexBuffer(),
		GL_STATIC_DRAW);

	// IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * Model.getNumberOfTriangles() * sizeof(int),
		Model.getIndexBuffer(),
		GL_STATIC_DRAW);


	return true;
} /* initBuffers() */

bool Programm::LoadShaders(std::string vertexshader, std::string fragmentshader)
{
	Shaderloader loader;

	if (!loader.Initshaders(vertexshader.data(), fragmentshader.data()))
		return false;

	ShaderProgram = loader.shader;
	TrLocation = loader.TrLocation;
	return true;
}


// Return the transformation matrix corresponding to the specified camera
Matrix4f Camera::computeCameraTransform() {
	// camera rotation
	Vector3f t = target.getNormalized();
	Vector3f u = up.getNormalized();
	Vector3f r = t.cross(u);
	Matrix4f camR(r.x(), r.y(), r.z(), 0.f,			//axis x
		u.x(), u.y(), u.z(), 0.f,					// axis y 
		-t.x(), -t.y(), -t.z(), 0.f,               // axis z:minus because we are rotating object
		0.f, 0.f, 0.f, 1.f);

	// camera translation
	Matrix4f camT = Matrix4f::createTranslation(-position);     // minus because we are translation object and not camera

																// perspective projection
	Matrix4f prj;
	if (projection == Projections::Perspective)
		prj = Matrix4f::createPerspectivePrj(fov, ar, zNear, zFar);
	else prj = Matrix4f(1.f, 0.f, 0.f, 0.f,                          //orthogonal projection, just get rid of z axis
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 0.f,
		0.f, 0.f, 0.f, 1.f);

	// scaling due to zooming
	Matrix4f camZoom = Matrix4f::createScaling(zoom, zoom, 1.f);

	// Final transformation. Notice the multiplication order
	// First vertices are moved in camera space
	// Then the perspective projection puts them in clip space
	// And a final zooming factor is applied in clip space
	return camZoom * prj * camR  * camT;

} /* computeCameraTransform() */



void Camera::Reset()
{
	position.set(0.f, 0.f, 0.f);
	target.set(0.f, 0.f, -1.f);
	up.set(0.f, 1.f, 0.f);
	fov = 30.f;
	ar = 1.f;  // will be correctly initialized in the "display()" method
	zNear = 0.1f;
	zFar = 100.f;
	zoom = 1.f;
}
