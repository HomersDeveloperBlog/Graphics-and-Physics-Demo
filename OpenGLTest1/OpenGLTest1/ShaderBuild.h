//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#pragma once

#include <vector>
#include <string>

enum ShaderType : unsigned int 
{
    VERTEX_SHADER_INTERNAL, FRAGMENT_SHADER_INTERNAL, 
    INTERNAL_SUPPORTED_SHADER_COUNT
};
 
const GLint ShaderEnumInternalToOGL[INTERNAL_SUPPORTED_SHADER_COUNT] = 
{
    GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
};

struct OpenGLShaderFile
{
    ShaderType m_eShaderTypeInternal;
    std::string m_strShaderFileName;
};

struct OpenGLProgramManifest
{
    std::vector<OpenGLShaderFile> m_vectShaderFiles;
};

struct OpenGLShaderSource
{
    GLint m_glnShaderType;
    std::string m_strShaderSource;
};

struct OpenGLProgramSource
{
    std::vector<OpenGLShaderFile> m_vectShaderSources;
};

bool GetOpenGLError(
	const string & i_strFileName,
	int nLineNumber);
    
 OpenGLProgram BuildProgramFromManifest(
	const OpenGLProgramManifest & i_oManifest);
    
//%deprecated
void CompileShaders();