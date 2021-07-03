#ifndef __SHADER_H__
#define __SHADER_H__

#pragma once

#include <string>
#include <GLFW/glfw3.h>
#include "imgui.h"

const ImColor CLEAR = ImColor(115, 140, 153, 255);
// ====================================================================================================================





// ====================================================================================================================
class Shader {
public:
  static const GLenum cvTypeToGLFormat(int _type);
  Shader();
  Shader(const std::string& _vertexCode, const std::string& _fragmentCode);
  Shader(const char* _vertexCode, const char* _fragmentCode);
  void Initialize();
  void Use();
  void BindFragDataLocation(GLuint _colorNumber=0, const char * _name="outColor");
  void BindTexture(GLuint _texture, const GLenum _target=GL_TEXTURE_2D);
  void CreateTexture();
  
  void SetupTexture(const GLenum _texture, GLuint* _ptrTextures, GLsizei _width, GLsizei _height, const void* _ptrData, const GLint _internalFormat=GL_RED, const GLsizei _n=1, const GLenum _target=GL_TEXTURE_2D, const GLint _level=0, const GLint _border=0, const GLenum _type=GL_UNSIGNED_BYTE);

  void UpdateTexture(const GLenum _texture, GLuint* _ptrTextures, GLsizei _width, GLsizei _height, const void* _ptrData,  const GLint _internalFormat=GL_RED, const GLsizei _n=1, const GLenum _target=GL_TEXTURE_2D, const GLint _level=0, const GLint _border=0, const GLenum _type=GL_UNSIGNED_BYTE);

  void SetupRenderBuffer(GLuint* _ptrRenderBuffers, GLsizei _width, GLsizei _height, GLuint* _ptrFrameBuffers, GLuint* _ptrTextures);
  void UpdateRenderBuffer(GLuint* _ptrRenderBuffers, GLsizei _width, GLsizei _height, GLuint* _ptrFrameBuffers, GLuint* _ptrTextures, const ImVec4& _color=CLEAR);

  template<typename T> void setUniform(const std::string& _name, T _val);
  template<typename T> void setUniform(const std::string& _name, T _val1, T _val2);
  template<typename T> void setUniform(const std::string& _name, T _val1, T _val2, T _val3);

private:
  void CheckCompileErr();
  void CheckLinkingErr();
  void Compile();
  void Link();
  unsigned int m_programId, m_vertexId, m_fragmentId;
  std::string m_vertexCode;
  std::string m_fragmentCode;

};
// ====================================================================================================================





// ====================================================================================================================
class FileManager {
public:
  FileManager();
  ~FileManager();
  static std::string Read(const std::string& filename);
};
// ====================================================================================================================





// ====================================================================================================================
#endif //__SHADER_H__