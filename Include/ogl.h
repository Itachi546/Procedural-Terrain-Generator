#ifndef OGL_H
#define OGL_H

#include "geometry/vertex_data.h"

#include <assert.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <glad/glad.h>

/*****************************************************************************************************************************************/
// Helper Structs

struct DrawElementsIndirectCommand {
	GLuint count_;
	GLuint instanceCount_;
	GLuint firstIndex_;
	GLuint baseVertex_;
	GLuint baseInstance_;
};

extern float gOGLVersion;

/*************************************************************************************************************************************************/
// Shader

class GLShader
{
public:

	explicit GLShader(const char* filename);

	GLShader(GLenum type, const char* shaderCode);

	inline GLenum getType() { return type_; }

	inline GLuint getHandle() { return handle_; }

	~GLShader() { glDeleteShader(handle_); }

private:
	GLenum        type_;
	GLuint        handle_;
};

/*************************************************************************************************************************************************/

class GLProgram
{
public:

	GLProgram(GLShader a, GLShader b);

	GLProgram(GLShader a, GLShader b, GLShader c);

	~GLProgram() { glDeleteProgram(handle_); }

	void useProgram() const { glUseProgram(handle_); }

	void setTexture(std::string name, int binding, unsigned int textureId);

	void setInt(std::string name, int val);

	void setFloat(std::string name, float val);

	void setVec2(std::string name, float x, float y);

protected:

	GLuint        handle_;
};

/*************************************************************************************************************************************************/

class GLComputeProgram
{
public:

	GLComputeProgram(GLShader shader);

	void setTexture(int binding, uint32_t textureId, GLenum access, GLenum format);

	void setFloat(std::string name, float val);

	void dispatch(uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) const;

	void useProgram() const { glUseProgram(handle_); }

private:
	GLuint       handle_;
};

/*************************************************************************************************************************************************/
// Buffer

class GLBuffer
{
public:

	GLBuffer() = default;

	explicit GLBuffer(const void* data, unsigned int size, GLbitfield flags);

	unsigned int getHandle() const { return handle_; }

	~GLBuffer() { glDeleteBuffers(1, &handle_); }

private:
	unsigned int         handle_;
};

/*************************************************************************************************************************************************/
// Mesh

class GLMesh 
{

public:

	GLMesh(const MeshData& meshData);

	void draw() const;

	unsigned int getMeshCount() { return meshCount_; }

	virtual ~GLMesh() { glDeleteVertexArrays(1, &vao_); }

	DrawElementsIndirectCommand* commands = nullptr;

private:
	uint32_t                vao_;
	uint32_t                numIndices_;

	GLBuffer                bufferVertices_;
	GLBuffer                bufferIndices_;
	GLBuffer                bufferIndirect_;

	unsigned int            meshCount_;
	std::vector<uint8_t>    drawCommands;
};

#endif