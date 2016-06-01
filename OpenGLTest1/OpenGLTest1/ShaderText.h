#pragma once

enum ShaderSource { eVertexShaderSource, eFragmentShaderSource, nShaderSourceCount };

static const char * astrShaderSources[nShaderSourceCount] =
{
	"#version 430 core                      \n"
	"			                            \n"
	"layout(location = 0) in vec4 vPosition;\n"
	"                                       \n"
	"void main()                            \n"
	"{                                      \n"
	"	gl_Position = vPosition;            \n"
	"}",

	"#version 430 core                      \n"
	"                                       \n"
	"out vec4 fColor;                       \n"
	"                                       \n"
	"void main()                            \n"
	"{                                      \n"
	"	fColor = vec4(0.0, 0.0, 1.0, 1.0);  \n"
	"}"
};