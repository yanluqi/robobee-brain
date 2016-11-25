#include "include/shader.h"

using namespace std;

Shader::Shader()
{
	
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{	
	m_program = LoadShaders(vertexPath.c_str(), fragmentPath.c_str());

	m_uniforms[MODEL] = glGetUniformLocation(m_program, "V");
	m_uniforms[VIEW] = glGetUniformLocation(m_program, "M");
	m_uniforms[MVP] = glGetUniformLocation(m_program, "MVP");
	m_uniforms[GSAMPLER] = glGetUniformLocation(m_program, "gSampler");
	m_uniforms[LIGHTPOS] = glGetUniformLocation(m_program, "LightPosition_worldspace");
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
}

void Shader::SetShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	if(m_program)
		glDeleteProgram(m_program);

	m_program = LoadShaders(vertexPath.c_str(), fragmentPath.c_str());

	m_uniforms[MVP] = glGetUniformLocation(m_program, "MVP");
	m_uniforms[GSAMPLER] = glGetUniformLocation(m_program, "gSampler");
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

void Shader::Update(Transform& State, Camera* Cam, Light* Lamp)
{
	glm::mat4 MPV_matrix = Cam->GetVPMat() * State.GetModel();
	glm::mat4 Model_matrix = State.GetModel();
	glm::mat4 View_matrix = Cam->GetVMat();
	glm::vec3 lightPos = Lamp->GetLightPos();

    glUniformMatrix4fv(m_uniforms[MODEL], 1, GL_FALSE, &Model_matrix[0][0]);
    glUniformMatrix4fv(m_uniforms[VIEW], 1, GL_FALSE, &View_matrix[0][0]);
    glUniformMatrix4fv(m_uniforms[MVP], 1, GL_FALSE, &MPV_matrix[0][0]);
    glUniform3f(m_uniforms[LIGHTPOS], lightPos.x, lightPos.y, lightPos.z);
}


// Load Shaders' files

GLuint Shader::LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open()){
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}
