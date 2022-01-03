#include <glad/glad.h>

#include "shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
// ====================================================================================================================





// ====================================================================================================================
Shader::Shader(const std::string& _vertexCode, const std::string& _fragmentCode) {
  m_vertexCode = _vertexCode;
  m_fragmentCode = _fragmentCode;
}
// ====================================================================================================================





// ====================================================================================================================
Shader::Shader(const char* _vertexCode, const char* _fragmentCode) : Shader::Shader(std::string(_vertexCode), std::string(_fragmentCode)) {}
// ====================================================================================================================





// ====================================================================================================================
void Shader::Initialize() {
  Compile();
  Link();
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::Compile() {
  const char* vcode = m_vertexCode.c_str();
  m_vertexId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertexId, 1, &vcode, NULL);
  glCompileShader(m_vertexId);

  const char* fcode = m_fragmentCode.c_str();
  m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_fragmentId, 1, &fcode, NULL);
  glCompileShader(m_fragmentId);
  CheckCompileErr();
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::Link() {
  m_programId = glCreateProgram();
  glAttachShader(m_programId, m_vertexId);
  glAttachShader(m_programId, m_fragmentId);
  glLinkProgram(m_programId);
  CheckLinkingErr();
  glDeleteShader(m_vertexId);
  glDeleteShader(m_fragmentId);
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::Use() {
  glUseProgram(m_programId);
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::BindFragDataLocation(GLuint _colorNumber, const char * _name) {
  glBindFragDataLocation(m_programId, 0, "outColor");
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::BindTexture(GLuint _texture, const GLenum _target) {
  glBindTexture(_target, _texture); // bind default texture to a texturing target
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::CreateTexture() {

  // ---- Create Vertex Array Object ----------------------------------------------------------------------------------
  GLuint vertexArray;
  glGenVertexArrays(1, &vertexArray);
  glBindVertexArray(vertexArray);

  // ---- Create a Vertex Buffer Object and copy the vertex data to it ------------------------------------------------
  GLuint vertexBuffer;
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                    //  Position        Color                Texcoords
  float vertices[] = { -1.0f,  1.0f,    1.0f, 0.0f, 0.0f,    0.0f, 1.0f,   // Top-left
                        1.0f,  1.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f,   // Top-right
                        1.0f, -1.0f,    0.0f, 0.0f, 1.0f,    1.0f, 0.0f,   // Bottom-right
                       -1.0f, -1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 0.0f};  // Bottom-left

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // ---- Create an Element Buffer Object and copy the element data to it ---------------------------------------------
  GLuint elementBuffer;
  glGenBuffers(1, &elementBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);

  GLuint elements[] = {0, 1, 2,
                       2, 3, 0};

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
  // ------------------------------------------------------------------------------------------------------------------

  // Specify the layout of the vertex data
  GLint posAttrib = glGetAttribLocation(m_programId, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);

  GLint colAttrib = glGetAttribLocation(m_programId, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));

  GLint texAttrib = glGetAttribLocation(m_programId, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));
  // ------------------------------------------------------------------------------------------------------------------

}
// ====================================================================================================================





// ====================================================================================================================
void Shader::SetupTexture(const GLenum _texture, GLuint* _ptrTextures, GLsizei _width, GLsizei _height, const void* _ptrData, const GLint _internalFormat, const GLsizei _n, const GLenum _target, const GLint _level, const GLint _border, const GLenum _type) {
  glActiveTexture(_texture); 
  glGenTextures(_n, _ptrTextures); 
  glBindTexture(_target, *_ptrTextures); 
  glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(_target, 0, _internalFormat, _width, _height, _level, _internalFormat, _type, _ptrData);
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::UpdateTexture(const GLenum _texture, GLuint* _ptrTextures, GLsizei _width, GLsizei _height, const void* _ptrData, const GLint _internalFormat, const GLsizei _n, const GLenum _target, const GLint _level, const GLint _border, const GLenum _type) {
  glActiveTexture(_texture); 
  glBindTexture(_target, *_ptrTextures); 
  glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(_target, 0, _internalFormat, _width, _height, _level, _internalFormat, _type, _ptrData);
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::SetupRenderBuffer(GLuint* _ptrRenderBuffers, GLsizei _width, GLsizei _height, GLuint* _ptrFrameBuffers, GLuint* _ptrTextures) {
  glGenRenderbuffers(1, _ptrRenderBuffers); // generate renderbuffer object names
  glBindRenderbuffer(GL_RENDERBUFFER, *_ptrRenderBuffers); // bind a named renderbuffer object
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height); // establish data storage, format and dimensions of a renderbuffer object's image
  glBindRenderbuffer(GL_RENDERBUFFER, 0); // bind default renderbuffer object

  glGenFramebuffers(1, _ptrFrameBuffers); // generate framebuffer object names
  glBindFramebuffer(GL_FRAMEBUFFER, *_ptrFrameBuffers); // bind a named framebuffer to a framebuffer target
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *_ptrTextures, 0); // attach a texture image to a framebuffer object
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *_ptrRenderBuffers); // attach a renderbuffer object to a framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer to a framebuffer target

}
// ====================================================================================================================





// ====================================================================================================================
void Shader::UpdateRenderBuffer(GLuint* _ptrRenderBuffers, GLsizei _width, GLsizei _height, GLuint* _ptrFrameBuffers, GLuint* _ptrTextures, const ImVec4& _color) {
  glBindRenderbuffer(GL_RENDERBUFFER, *_ptrRenderBuffers); // bind a named renderbuffer object
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height); // establish data storage, format and dimensions of a renderbuffer object's image
  glBindRenderbuffer(GL_RENDERBUFFER, 0); // bind default renderbuffer object

  glBindFramebuffer(GL_FRAMEBUFFER, *_ptrTextures); // bind a named framebuffer to a framebuffer target
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *_ptrTextures, 0); // attach a texture image to a framebuffer object
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *_ptrRenderBuffers); // attach a renderbuffer object to a framebuffer object
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind default framebuffer to a framebuffer target

  glBindFramebuffer(GL_FRAMEBUFFER, *_ptrTextures); // switch to off screen framebuffer
  glViewport(0, 0, _width, _height);
  glClearColor(_color.x, _color.y, _color.z, _color.w);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // switch back to screen framebuffer
}
// ====================================================================================================================





// ====================================================================================================================
template<>
void Shader::setUniform<int>(const std::string& _name, int _val) {
  glUniform1i(glGetUniformLocation(m_programId, _name.c_str()), _val);
}





template<>
void Shader::setUniform<bool>(const std::string& _name, bool _val) {
  glUniform1i(glGetUniformLocation(m_programId, _name.c_str()), _val);
}





template<>
void Shader::setUniform<GLuint>(const std::string& _name, GLuint _val) {
  glUniform1i(glGetUniformLocation(m_programId, _name.c_str()), _val);
}





template<>
void Shader::setUniform<float>(const std::string& _name, float _val) {
  glUniform1f(glGetUniformLocation(m_programId, _name.c_str()), _val);
}





template<>
void Shader::setUniform<float>(const std::string& _name, float _val1, float _val2) {
  glUniform2f(glGetUniformLocation(m_programId, _name.c_str()), _val1, _val2);
}





template<>
void Shader::setUniform<float>(const std::string& _name, float _val1, float _val2, float _val3) {
  glUniform3f(glGetUniformLocation(m_programId, _name.c_str()), _val1, _val2, _val3);
}





template<>
void Shader::setUniform<float*>(const std::string& _name, float* _val) {
  glUniformMatrix4fv(glGetUniformLocation(m_programId, _name.c_str()), 1, GL_FALSE, _val);
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::CheckCompileErr() {
  int success;
  char infoLog[1024];
  glGetShaderiv(m_vertexId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_vertexId, 1024, NULL, infoLog);
    std::cout << "Error compiling Vertex Shader:\n" << infoLog << std::endl;
  }
  glGetShaderiv(m_fragmentId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(m_fragmentId, 1024, NULL, infoLog);
    std::cout << "Error compiling Fragment Shader:\n" << infoLog << std::endl;
  }
}
// ====================================================================================================================





// ====================================================================================================================
void Shader::CheckLinkingErr() {
  int success;
  char infoLog[1024];
  glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(m_programId, 1024, NULL, infoLog);
    std::cout << "Error Linking Shader Program:\n" << infoLog << std::endl;
  }
}
// ====================================================================================================================





// ====================================================================================================================
std::string Shader::ReadFile(const std::string& filename) {
  std::ifstream file;
  file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
  std::stringstream file_stream;
  try {
    file.open(filename.c_str());
    file_stream << file.rdbuf();
    file.close();
  } catch (std::ifstream::failure& failure) {
    std::cout << "Error reading Shader File!" << std::endl;
  }
  return file_stream.str();
}











