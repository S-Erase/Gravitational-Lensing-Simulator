#include "cPCH.h"
#include "cOpenGLObjects.h"
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

cOpenGLDynamicVBO::cOpenGLDynamicVBO(size_t size) : m_Capacity(size) {
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_DRAW);
}
void cOpenGLDynamicVBO::InsertData(size_t size, const void* data) {
	SE_ASSERT(size <= m_Capacity, "Vertex Buffer Overflow!");
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	m_Size = size;
}

cOpenGLStaticVBO::cOpenGLStaticVBO(size_t size, const void* data) {
	m_Size = size;
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

static GLenum ShaderTypeFromString(const std::string& type) {
	if (type == "vertex") return GL_VERTEX_SHADER;
	if (type == "geometry") return GL_GEOMETRY_SHADER;
	if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

	SE_ASSERT(false, "Unknown shader type!");
		return 0;
}
cOpenGLShader::cOpenGLShader(const std::string& filepath)
{
	//Reads shader code
	std::string srccode;
	std::ifstream in(filepath, std::ios::in | std::ios::binary);
	SE_ASSERT(in, wxString::Format("'%s' could not be read!", filepath));
	in.seekg(0, std::ios::end);
	srccode.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&srccode[0], srccode.size());
	in.close();

	//Seperates code for different shaders
	std::unordered_map<GLenum, std::string> shaderSources;

	const char* typeToken = "#type";
	size_t typeTokenLength = strlen(typeToken);
	size_t pos = srccode.find(typeToken, 0);
	while (pos != std::string::npos)
	{
		size_t eol = srccode.find_first_of("\r\n", pos);
		SE_ASSERT(eol != std::string::npos, "Syntax error");
		size_t begin = pos + typeTokenLength + 1;
		std::string type = srccode.substr(begin, eol - begin);
		SE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");

		size_t nextLinePos = srccode.find_first_not_of("\r\n", eol);
		pos = srccode.find(typeToken, nextLinePos);
		shaderSources[ShaderTypeFromString(type)] =
			srccode.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? srccode.size() - 1 : nextLinePos));
	}

	//Creates Shader
	GLuint program = glCreateProgram();
	SE_ASSERT(shaderSources.size() <= 3, "Does not support more than three shaders");
	GLuint glShaderIDs[3];
	size_t glShaderIDCount = 0;
	for (auto& kv : shaderSources) {
		GLenum type = kv.first;
		const GLchar* source = kv.second.c_str();

		GLuint shader = glCreateShader(type);
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint isCompiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(shader);
			for (size_t sh = 0; sh < glShaderIDCount; sh++) {
				glDeleteShader(glShaderIDs[sh]);
			}

			char* infoLogCh = new char[36 + maxLength];
			switch (type) {
			case GL_VERTEX_SHADER:
				sprintf(infoLogCh, "%s: %s", "Vertex shader failed to compile", infoLog.data());
				break;
			case GL_GEOMETRY_SHADER:
				sprintf(infoLogCh, "%s: %s", "Geometry shader failed to compile", infoLog.data());
				break;
			case GL_FRAGMENT_SHADER:
				sprintf(infoLogCh, "%s: %s", "Fragment shader failed to compile", infoLog.data());
				break;
			}
			SE_ASSERT(false, infoLogCh);
			delete[] infoLogCh;
			break;
		}

		glAttachShader(program, shader);
		glShaderIDs[glShaderIDCount++] = shader;
	}
	glLinkProgram(program);
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

		glDeleteProgram(program);
		for (size_t sh = 0; sh < glShaderIDCount; sh++) {
			glDeleteShader(glShaderIDs[sh]);
		}

		char* infoLogCh = new char[25 + maxLength];
		sprintf(infoLogCh, "Failed to link shaders: %s", infoLog.data());
		SE_ASSERT(false, infoLogCh);
		delete[] infoLogCh;
		return;
	}

	for (size_t sh = 0; sh < glShaderIDCount; sh++) {
		glDetachShader(program, glShaderIDs[sh]);
	}
	m_ShaderID = program;
}

static int glSizeof(GLenum type) {
	switch (type) {
	case GL_BYTE: return 1;
	case GL_UNSIGNED_BYTE: return 1;
	case GL_SHORT: return 2;
	case GL_UNSIGNED_SHORT: return 2;
	case GL_INT: return 4;
	case GL_UNSIGNED_INT: return 4;
	case GL_FIXED: return 4;
	case GL_HALF_FLOAT: return 2;
	case GL_FLOAT: return 4;
	case GL_DOUBLE: return 8;
	default: return 0;
	}
}

cOpenGLStaticIBO::cOpenGLStaticIBO(size_t size, const void* data, GLenum type) : m_Type(type) {
	glGenBuffers(1, &m_BufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	switch (type) {
	case GL_UNSIGNED_INT:
		m_Count = size / sizeof(GLuint);
		break;
	case GL_UNSIGNED_SHORT:
		m_Count = size / sizeof(GLushort);
		break;
	case GL_UNSIGNED_BYTE:
		m_Count = size / sizeof(GLubyte);
		break;
	default:
		SE_ASSERT(false, "Invalid Element Buffer Type!");
	}
}

cOpenGLVAO::cOpenGLVAO()
{
	glGenVertexArrays(1, &m_ArrayID);
}
cOpenGLVAO::~cOpenGLVAO()
{
	glDeleteVertexArrays(1, &m_ArrayID);
}
void cOpenGLVAO::SetLayout(std::initializer_list<cOpenGLVertexAttribute> layout)
{
	int stride = 0, offset = 0, index = 0;
	for (auto& att : layout) {
		stride += att.size*glSizeof(att.type);
	}
	m_Stride = stride;
	
	glBindVertexArray(m_ArrayID);
	for (auto& att : layout) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index++, att.size, att.type, false, stride, (const void*)offset);
		offset += att.size * glSizeof(att.type);
	}
}

void cOpenGLVAO::BindVertexBuffer(const std::shared_ptr<cOpenGLVBO>& vbo)
{
	glBindVertexArray(m_ArrayID);
	vbo->Bind();
	m_vboRef = vbo;
}
void cOpenGLVAO::DrawElements(std::shared_ptr<cOpenGLStaticIBO>& ibo, GLenum mode, size_t count)
{
	glBindVertexArray(m_ArrayID);
	SE_ASSERT(m_vboRef, "Vertex Array Object needs a Vertex Buffer Object!");
	SE_ASSERT(count <= ibo->GetCount(), "Index Buffer Overflow!");
	ibo->Bind();
	glDrawElements(mode, count, ibo->GetType(), NULL);
}
void cOpenGLVAO::DrawArrays(GLenum mode) {
	glBindVertexArray(m_ArrayID);
	SE_ASSERT(m_vboRef, "Vertex Array Object needs a Vertex Buffer Object!");
	size_t DrawCount = m_vboRef->GetSize() / m_Stride;
	glDrawArrays(mode, 0, DrawCount);
}

cOpenGLTexture::cOpenGLTexture(const char* filepath)
{
	unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_Channels, 0);

	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	GLint internalformat = 0;
	GLint format = 0;
	switch (m_Channels) {
	case 4:
		internalformat = GL_RGBA8;
		format = GL_RGBA;
		break;
	case 3:
		internalformat = GL_RGB8;
		format = GL_RGB;
		break;
	case 2:
		internalformat = GL_RG8;
		format = GL_RG;
		break;
	case 1:
		internalformat = GL_R8;
		format = GL_RED;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internalformat, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

struct stbi_imagedata {
	unsigned char* data = nullptr;
	int width = 0, height = 0, channels = 0;

	stbi_imagedata() {}
	stbi_imagedata(const char* filepath) { data = stbi_load(filepath, &width, &height, &channels, 0);}
	~stbi_imagedata() {
		if (data) stbi_image_free(data);
	}
	stbi_imagedata(const stbi_imagedata& other) {
		width = other.width;
		height = other.height;
		channels = other.channels;

		data = (unsigned char*)malloc(width * height * channels);
		memcpy(data, other.data, width * height * channels);
	}
	stbi_imagedata(stbi_imagedata&& other) {
		width = other.width;
		height = other.height;
		channels = other.channels;

		data = other.data;
		other.data = nullptr;
	}
	stbi_imagedata operator=(const stbi_imagedata& other) {
		if (this != &other) {
			width = other.width;
			height = other.height;
			channels = other.channels;

			if (data) stbi_image_free(data);

			data = (unsigned char*)malloc(width * height * channels);
			memcpy(data, other.data, width * height * channels);
		}
		
		return *this;
	}
	stbi_imagedata operator=(stbi_imagedata&& other) {
		width = other.width;
		height = other.height;
		channels = other.channels;

		if (data) stbi_image_free(data);

		data = other.data;
		other.data = nullptr;

		return *this;
	}

	void load(const char* filepath) {
		if (data) stbi_image_free(data);
		data = stbi_load(filepath, &width, &height, &channels, 0);
	}
};

void stbi_flip_horizontally(stbi_imagedata& image)
{
	unsigned char tmp;
	for (int y = 0; y < image.height; y++) {
		for (int x = 0; x < image.width / 2; x++) {
			for (int n = 0; n < image.channels; n++) {
				tmp = image.data[(y * image.width + x) * image.channels + n];
				image.data[(y * image.width + x) * image.channels + n] = image.data[(y * image.width + (image.width - 1 - x)) * image.channels + n];
				image.data[(y * image.width + (image.width - 1 - x)) * image.channels + n] = tmp;
			}
		}
	}
}

cOpenGLCubemap::cOpenGLCubemap(const char* frontfilepath, const char* backfilepath, const char* rightfilepath,
	const char* leftfilepath, const char* upfilepath, const char* downfilepath)
{
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	const char* filepaths[6] = { rightfilepath, leftfilepath, upfilepath, downfilepath, backfilepath, frontfilepath};
	GLint format = 0;
	stbi_imagedata inimage;
	unsigned char *outdata;
	for (int i = 0; i < 6; i++) {
		inimage.load(filepaths[i]);
		stbi_flip_horizontally(inimage);
		outdata = (unsigned char*)malloc(m_Size * m_Size * inimage.channels);
		stbir_resize_uint8_srgb(inimage.data, inimage.width, inimage.height, 0, outdata, m_Size, m_Size, 0, inimage.channels, STBIR_ALPHA_CHANNEL_NONE, 0);

		switch (inimage.channels) {
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_Size, m_Size, 0, format, GL_UNSIGNED_BYTE, outdata);
		free(outdata);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void cOpenGLCubemap::SetTexture(GLenum target, const char* filepath)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	GLint format = 0;
	stbi_imagedata inimage(filepath);
	unsigned char* outdata = (unsigned char*)stbi__malloc(m_Size * m_Size * inimage.channels);
	stbi_flip_horizontally(inimage);
	stbir_resize_uint8_srgb(inimage.data, inimage.width, inimage.height, 0, outdata, m_Size, m_Size, 0, inimage.channels, STBIR_ALPHA_CHANNEL_NONE, 0);

	switch (inimage.channels) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}
	glTexImage2D(target, 0, GL_RGBA8, m_Size, m_Size, 0, format, GL_UNSIGNED_BYTE, outdata);
	free(outdata);
}

stbi_imagedata stbi_crop(stbi_imagedata inimage, int x0, int y0, int x1, int y1)
{
	stbi_imagedata outimage;
	outimage.data = (unsigned char*)stbi__malloc((x1 - x0) * (y1 - y0) * inimage.channels);
	for (int y = y0; y < y1; y++) {
		for (int x = x0; x < x1; x++) {
			for (int n = 0; n < inimage.channels; n++) {
				outimage.data[((y-y0) * (x1 - x0) + (x-x0)) * inimage.channels + n] = inimage.data[(y * inimage.width + x) * inimage.channels + n];
			}
		}
	}
	outimage.width = x1 - x0;
	outimage.height = y1 - y0;
	outimage.channels = inimage.channels;

	return outimage;
}

void cOpenGLCubemap::UseNet(const char* filepath)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	stbi_imagedata inimage(filepath);
	GLint format = 0;
	switch (inimage.channels) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}
	int faces[6][4] = { 
		{0 * inimage.width / 4,1 * inimage.height / 3,1 * inimage.width / 4,2 * inimage.height / 3}, //Right Face
		{2 * inimage.width / 4,1 * inimage.height / 3,3 * inimage.width / 4,2 * inimage.height / 3}, //Left Face
		{1 * inimage.width / 4,0 * inimage.height / 3,2 * inimage.width / 4,1 * inimage.height / 3}, //Up Face
		{1 * inimage.width / 4,2 * inimage.height / 3,2 * inimage.width / 4,3 * inimage.height / 3}, //Down Face
		{1 * inimage.width / 4,1 * inimage.height / 3,2 * inimage.width / 4,2 * inimage.height / 3}, //Back Face
		{3 * inimage.width / 4,1 * inimage.height / 3,4 * inimage.width / 4,2 * inimage.height / 3}, //Front Face
	};
	stbi_imagedata croppedimage;
	unsigned char* outdata = (unsigned char*)stbi__malloc(m_Size * m_Size * inimage.channels);
	for (int i = 0; i < 6; i++) {
		croppedimage = stbi_crop(inimage, faces[i][0], faces[i][1], faces[i][2], faces[i][3]);

		stbi_flip_horizontally(croppedimage);
		stbir_resize_uint8_srgb(croppedimage.data, faces[i][2] - faces[i][0], faces[i][3] - faces[i][1], 0, outdata, m_Size, m_Size, 0, croppedimage.channels, STBIR_ALPHA_CHANNEL_NONE, 0);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, m_Size, m_Size, 0, format, GL_UNSIGNED_BYTE, outdata);
	}
	free(outdata);
}