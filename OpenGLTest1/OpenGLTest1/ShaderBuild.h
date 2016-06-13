//ShaderBuild.cpp - Functions for compiling, linking, and using GLSL programs

#pragma once

#include <vector>
#include <string>

using namespace std;

//%deprecated
void CompileShaders();

#define OGL_INVALID_SHADER_HANDLE static_cast<GLint>(0)
#define OGL_INVALID_SHADER_HANDLE static_cast<GLint>(0)

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
    string m_strShaderFileName;
};

struct OpenGLProgramManifest
{
    vector<OpenGLShaderFile> m_vectShaderFiles;
};

struct OpenGLShaderSource
{
    GLint m_glnShaderType;
    string m_strShaderSource;
};

struct OpenGLProgramSource
{
    vector<OpenGLShaderFile> m_vectShaderSources;
};

// struct OpenGLShaderHandle
// {
//     ~OpenGLShaderHandle()
    
//     GLint m_nShaderHandle;
// }

//Automatic layout seems best, but you must be prepared for the compiler to eliminate inputs. 
//Layout index is basically padded to float vec4's, with each row of a matrix indexed separately.
//Layout requested via int GetAttribLocation(uint program, const char *name);