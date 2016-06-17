//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#pragma once

#include <vector>
#include <string>

bool GetOpenGLError( //%relocate?
	const string & i_strFileName,
	int nLineNumber);
    
//%deprecated
void CompileShaders();

enum ShaderType
{
    VERTEX_SHADER_INTERNAL, FRAGMENT_SHADER_INTERNAL, 
    INTERNAL_SUPPORTED_SHADER_COUNT
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

class OpenGLShaderSource
{
public:
    OpenGLShaderSource(const OpenGLShaderFile & oShaderFile);

    static GLint ConvertInternalShaderEnumToOGL(
	    ShaderType i_eShaderType);
        
private:
    GLint m_glnShaderType;
    std::string m_strShaderSource;
};

class OpenGLProgramSource
{
public:
    OpenGLProgramSource(const OpenGLProgramManifest & i_oManifest)
    
    OpenGLProgram BuildProgramFromSource();
    
private:
    std::vector<OpenGLShaderFile> m_vectShaderSources;
};