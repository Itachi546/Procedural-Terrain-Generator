#include "ogl.h"

#include <string>
#include <fstream>
#include "debugdraw.h"

/*****************************************************************************************************************************************/
// Shader

static int EndWith(const char* s, const char* part)
{
	return (strstr(s, part) - s) == (strlen(s) - strlen(part));
}

/*****************************************************************************************************************************************/

static GLenum GetShaderTypeFromFile(const char* filename)
{
	if (EndWith(filename, ".vert"))
		return GL_VERTEX_SHADER;
	if (EndWith(filename, ".frag"))
		return GL_FRAGMENT_SHADER;
	if (EndWith(filename, ".geom"))
		return GL_GEOMETRY_SHADER;
	if (EndWith(filename, ".comp"))
		return GL_COMPUTE_SHADER;
	if (EndWith(filename, ".tesc"))
		return GL_TESS_CONTROL_SHADER;
	if (EndWith(filename, ".tese"))
		return GL_TESS_EVALUATION_SHADER;

	assert(false);
	return 0;

}

/*****************************************************************************************************************************************/

static std::string ReadShaderFile(const char* filename)
{

	std::ifstream inFile(filename);
	if (!inFile)
	{
		printf("Failed to load file: %s\n", filename);
		assert(0);
	}
	return std::string{
		(std::istreambuf_iterator<char>(inFile)),
		std::istreambuf_iterator<char>()
	};
}

/*****************************************************************************************************************************************/

GLShader::GLShader(const char* filename) :
	GLShader(GetShaderTypeFromFile(filename),
		ReadShaderFile(filename).c_str())
{
}

GLShader::GLShader(GLenum type, const char* shaderCode) : 
	type_(type),
	handle_(glCreateShader(type_))
{
	glShaderSource(handle_, 1, &shaderCode, nullptr);
	glCompileShader(handle_);

	char buffer[8192];
	GLsizei length = 0;
	glGetShaderInfoLog(handle_, sizeof(buffer), &length, buffer);
	if (length)
	{
		printf("%s\n", buffer);
		printf("%s\n", shaderCode);
		assert(0);
	}
}

/*****************************************************************************************************************************************/

// Shader Program

static
void printProgramInfoLog(GLuint handle_)
{
	char buffer[8192];
	GLsizei length = 0;

	glGetProgramInfoLog(handle_, sizeof(buffer), &length, buffer);
	if (length > 0)
	{
		printf("%s\n", buffer);
		assert(false);
	}
}

/*****************************************************************************************************************************************/

GLProgram::GLProgram(GLShader a, GLShader b) :
	handle_(glCreateProgram())
{
	glAttachShader(handle_, a.getHandle());
	glAttachShader(handle_, b.getHandle());
	glLinkProgram(handle_);

	printProgramInfoLog(handle_);
}

GLProgram::GLProgram(GLShader a, GLShader b, GLShader c) :
	handle_(glCreateProgram())
{
	glAttachShader(handle_, a.getHandle());
	glAttachShader(handle_, b.getHandle());
	glAttachShader(handle_, c.getHandle());
	glLinkProgram(handle_);

	printProgramInfoLog(handle_);
}


void GLProgram::setTexture(std::string name, int binding, unsigned int textureId)
{
	setInt("name", binding);
	glActiveTexture(GL_TEXTURE0 + binding);
	glBindTexture(GL_TEXTURE_2D, textureId);
}

void GLProgram::setInt(std::string name, int val)
{
	glUniform1i(glGetUniformLocation(handle_, name.c_str()), val);
}

void GLProgram::setFloat(std::string name, float val)
{
	glUniform1f(glGetUniformLocation(handle_, name.c_str()), val);
}

void GLProgram::setVec2(std::string name, float x, float y)
{
	glUniform2f(glGetUniformLocation(handle_, name.c_str()), x, y);
}

/*****************************************************************************************************************************************/

GLComputeProgram::GLComputeProgram(GLShader shader) :
	handle_(glCreateProgram())
{
	glAttachShader(handle_, shader.getHandle());
	glLinkProgram(handle_);

	printProgramInfoLog(handle_);
}

void GLComputeProgram::setTexture(int binding, uint32_t textureId, GLenum access, GLenum format)
{
	glBindImageTexture(binding, textureId, 0, GL_FALSE, 0, access, format);
}

void GLComputeProgram::setFloat(std::string name, float val)
{
	glUniform1f(glGetUniformLocation(handle_, name.c_str()), val);
}

void GLComputeProgram::dispatch(uint32_t workGroupX, uint32_t workGroupY, uint32_t workGroupZ) const
{
	glDispatchCompute(workGroupX, workGroupY, workGroupZ);
}

/*****************************************************************************************************************************************/
// Buffer
GLBuffer::GLBuffer(const void* data, unsigned int size, GLbitfield flags)
{
	glCreateBuffers(1, &handle_);
	glNamedBufferStorage(handle_, size, data, flags);
}

/*****************************************************************************************************************************************/

GLMesh::GLMesh(const MeshData& meshData) :

	bufferVertices_((void*)meshData.vertices.data(), static_cast<uint32_t>(meshData.vertices.size()) * sizeof(float), 0),

	bufferIndices_((void*)meshData.indices.data(), static_cast<uint32_t>(meshData.indices.size()) * sizeof(uint32_t), 0),

	bufferIndirect_(nullptr, meshData.meshCount * sizeof(DrawElementsIndirectCommand) + sizeof(GLsizei), GL_DYNAMIC_STORAGE_BIT),

	numIndices_(static_cast<uint32_t>(meshData.indices.size())),
	boundingBoxes_(meshData.boundingBox),
	meshCount_(meshData.meshCount)
{
	glCreateVertexArrays(1, &vao_);

	glVertexArrayElementBuffer(vao_, bufferIndices_.getHandle());

	glVertexArrayVertexBuffer(vao_, 0, bufferVertices_.getHandle(), 0, meshData.getTotalStride());

	for (const AttributeLayout& attribute : meshData.attributeLayout)
	{
		glEnableVertexArrayAttrib(vao_, attribute.attributeIndex);
		glVertexArrayAttribFormat(vao_, attribute.attributeIndex, attribute.sizeInCount, GL_FLOAT, GL_FALSE, attribute.stride);
		glVertexArrayAttribBinding(vao_, attribute.attributeIndex, 0);
	}

	uint32_t numCommand = meshData.meshCount;
	drawCommands.resize(numCommand * sizeof(DrawElementsIndirectCommand) + sizeof(GLsizei));

	GLsizei drawCount = numCommand;
	memcpy_s(drawCommands.data(), sizeof(GLsizei), &drawCount, sizeof(GLsizei));

	commands = reinterpret_cast<DrawElementsIndirectCommand*>(drawCommands.data() + sizeof(GLsizei));
	for (uint32_t i = 0; i < numCommand; ++i)
	{
		commands[i].count_ = meshData.meshes[i].indexCount;
		commands[i].firstIndex_ = meshData.meshes[i].indexOffset;
		commands[i].baseVertex_ = meshData.meshes[i].vertexOffset / meshData.getTotalAttributeCount();
	}

	glBindVertexArray(vao_);
}

/*****************************************************************************************************************************************/

void GLMesh::draw() const
{
	GLsizei baseInstance = 0;

	for (uint32_t i = 0; i < meshCount_; ++i)
	{
		commands[i].baseInstance_ = baseInstance;
		baseInstance += commands[i].instanceCount_;
	}

	glNamedBufferSubData(bufferIndirect_.getHandle(), 0, drawCommands.size(), drawCommands.data());

	glBindVertexArray(vao_);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, bufferIndirect_.getHandle());

	glBindBuffer(GL_PARAMETER_BUFFER, bufferIndirect_.getHandle());

	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)sizeof(GLsizei), meshCount_, 0);
}

/*****************************************************************************************************************************************/
static GLenum GetTextureFilter(TextureFilter filter)
{
	switch (filter)
	{
	case TextureFilter::Linear:
		return GL_LINEAR;
	case TextureFilter::Nearest:
		return GL_NEAREST;
	case TextureFilter::LinearMipmap:
		return GL_LINEAR_MIPMAP_LINEAR;
	default:
		return GL_LINEAR;
	}
}

static GLenum GetTextureWrap(TextureWrap wrap)
{
	switch (wrap)
	{
	case TextureWrap::Repeat:
		return GL_REPEAT;
	case TextureWrap::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	default:
		return GL_CLAMP_TO_EDGE;
	}
}

static TextureFormatInfo GetTextureFormatInfo(TextureFormat format)
{
	TextureFormatInfo result = {};
	switch (format)
	{
	case TextureFormat::R16F:
		result.format = GL_RED;
		result.internalFormat = GL_R16F;
		result.type = GL_FLOAT;
		break;
	case TextureFormat::RGB8:
		result.format = GL_RGB;
		result.internalFormat =  GL_RGB8;
		result.type = GL_UNSIGNED_BYTE;
		break;
	case TextureFormat::RGBA8:
		result.format = GL_RGBA;
		result.internalFormat = GL_RGBA8;
		result.type = GL_UNSIGNED_BYTE;
		break;
	default:
		result.format = GL_RGB;
		result.internalFormat = GL_RGB8;
		result.type = GL_UNSIGNED_BYTE;
		break;
	}

	return result;
}

/*****************************************************************************************************************************************/

GLTexture::GLTexture(const void* data, const TextureParams& params) : 
	width_(params.width),
	height_(params.height),
	formatInfo_(GetTextureFormatInfo(params.format))
{
	glCreateTextures(GL_TEXTURE_2D, 1, &handle_);
	glTextureParameteri(handle_, GL_TEXTURE_MIN_FILTER, GetTextureFilter(params.minFilter));
	glTextureParameteri(handle_, GL_TEXTURE_MAG_FILTER, GetTextureFilter(params.magFilter));
	glTextureParameteri(handle_, GL_TEXTURE_WRAP_S, GetTextureWrap(params.wrapS));
	glTextureParameteri(handle_, GL_TEXTURE_WRAP_T, GetTextureWrap(params.wrapT));

	glTextureStorage2D(handle_, 1, formatInfo_.internalFormat, params.width, params.height);
	if(data)
		glTextureSubImage2D(handle_, 0, 0, 0, params.width, params.height, formatInfo_.format, formatInfo_.type, data);
}

/*****************************************************************************************************************************************/