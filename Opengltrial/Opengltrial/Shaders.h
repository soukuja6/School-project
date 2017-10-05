#pragma once
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

class Shaderloader
{
public:
	bool Initshaders(const char* vertexShaderFilename,
		const char* fragmentShaderFilename);
	GLuint shader;                        //actual shader
	GLint TrLocation = -1;                //location of transformation variable in shader
private:
	std::string readTextFile(const std::string& pathAndFileName);
};

