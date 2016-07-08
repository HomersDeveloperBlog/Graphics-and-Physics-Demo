//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#pragma once

#include <vector>
#include <string>

#include "GLBase.h"
#include "Program.h"

//%deprecated
void CompileShaders();

enum ShaderType
{
    VERTEX_SHADER_INTERNAL, FRAGMENT_SHADER_INTERNAL, 
    INTERNAL_SUPPORTED_SHADER_COUNT
};

struct OpenGLShaderFile
{
	OpenGLShaderFile(    
		const ShaderType & i_eShaderTypeInternal,
		const std::string & i_strShaderFileName)
		: m_eShaderTypeInternal(i_eShaderTypeInternal),
		m_strShaderFileName(i_strShaderFileName) {}

    ShaderType m_eShaderTypeInternal;
    const std::string m_strShaderFileName;
};

struct OpenGLProgramManifest
{
    std::vector<OpenGLShaderFile> m_vectShaderFiles;
};

struct OpenGLShaderSource
{
    OpenGLShaderSource(const OpenGLShaderFile & oShaderFile);

    static GLint ConvertInternalShaderEnumToOGL(
        ShaderType i_eShaderType);

    GLint m_glnShaderType;
    const std::string m_strShaderSource;
};

class OpenGLProgramSource
{
public:
    OpenGLProgramSource(const OpenGLProgramManifest & i_oManifest);
    
    OpenGLProgram BuildProgram();
    
private:
    std::vector<OpenGLShaderSource> m_vectShaderSources;
};