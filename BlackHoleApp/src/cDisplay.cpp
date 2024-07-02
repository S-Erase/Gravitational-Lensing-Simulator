#include "cDisplay.h"
#include "cCtrlPanel.h"

#define WASD_W 0x01
#define WASD_A 0x02
#define WASD_S 0x04
#define WASD_D 0x08

glm::vec2 WASDtovec2(int WASDflag) {
	glm::vec2 dir(0.0f);
	//dir.x = (float)(bool)(WASDflag & WASD_D) - (float)(bool)(WASDflag & WASD_A);
	//dir.y = (float)(bool)(WASDflag & WASD_W) - (float)(bool)(WASDflag & WASD_S);
	if (WASDflag & WASD_W) dir.y++;
	if (WASDflag & WASD_A) dir.x--;
	if (WASDflag & WASD_S) dir.y--;
	if (WASDflag & WASD_D) dir.x++;

	if ((dir.x!=0) && (dir.y != 0)) {
		dir /= sqrtf(2);
	}
	return dir;
}

cDisplay* cDisplay::m_instance = nullptr;
cDisplay::cDisplay(wxWindow* parent) : wxGLCanvas(parent/*, wxID_ANY, nullptr, { 0,0 }, { 640,120 }*/), m_context(this)
{
	if (!m_instance)
		m_instance = this;

	GetSize(&m_Width, &m_Height);
	Bind(wxEVT_PAINT, &cDisplay::OnPaint, this);
	Bind(wxEVT_SIZE, &cDisplay::OnResize, this);
	Bind(wxEVT_LEFT_DOWN, &cDisplay::OnMouseDown, this);
	Bind(wxEVT_MOTION, &cDisplay::OnMouseMove, this);
	Bind(wxEVT_KEY_DOWN, &cDisplay::OnKeyDown, this);
	Bind(wxEVT_KEY_UP, &cDisplay::OnKeyUp, this);

	SetCurrent(m_context);
	InitializeGLEW();
	Setup();
}

cDisplay::~cDisplay()
{
	SetCurrent(m_context);
}

void cDisplay::InitializeGLEW()
{
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		const GLubyte* msg = glewGetErrorString(err);
		throw std::exception(reinterpret_cast<const char*>(msg));
	}
}

void cDisplay::Setup()
{
	float cubevertices[] = {
		-1.0f, -1.0f, -1.0f, //0
		 1.0f, -1.0f, -1.0f, //1
		-1.0f,  1.0f, -1.0f, //2
		 1.0f,  1.0f, -1.0f, //3
		-1.0f, -1.0f,  1.0f, //4
		 1.0f, -1.0f,  1.0f, //5
		-1.0f,  1.0f,  1.0f, //6
		 1.0f,  1.0f,  1.0f	 //7
	};
	unsigned int cubeindices[] = {
		0,1,2,1,2,3,
		0,1,4,1,4,5,
		0,2,4,2,4,6,
		7,6,5,6,5,4,
		7,6,3,6,3,2,
		7,5,3,5,3,1
	};
	m_CubeVBO = std::make_shared<cOpenGLStaticVBO>(sizeof(cubevertices), cubevertices);
	m_CubeIBO = std::make_shared<cOpenGLStaticIBO>(sizeof(cubeindices), cubeindices, GL_UNSIGNED_INT);
	m_CubeVAO = std::make_unique<cOpenGLVAO>();
	m_CubeVAO->SetLayout({ {3, GL_FLOAT} });
	m_CubeVAO->BindVertexBuffer(m_CubeVBO);

	m_CubeFlatShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_Flat.glsl");
	m_CubeFlatwPhotonSphereShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_FlatwPhotonSphere.glsl");
	m_CubeNGShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_NG.glsl");
	m_CubeGRShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_GR.glsl");
	m_CubeGRwChargeShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_GRwCharge.glsl");
	m_CubeConeShader = std::make_shared<cOpenGLShader>("src/assets/shaders/Cube_Shader_Cone.glsl");

	m_Cubemap = std::make_shared<cOpenGLCubemap>("src/assets/textures/defaultF.png","src/assets/textures/defaultB.png","src/assets/textures/defaultR.png",
		"src/assets/textures/defaultL.png","src/assets/textures/defaultU.png","src/assets/textures/defaultD.png");
	ViewMat = glm::mat4(1.0f);
	ProjMat = glm::perspective(glm::radians(45.0f), (float)m_Width/(float)m_Height, 0.1f,100.0f);
}

void cDisplay::OnPaint(wxPaintEvent& event)
{
	SetCurrent(m_context);

	// set background to black
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// draw the graphics
	ProjMat = glm::perspective(glm::radians(90.0f), (float)m_Width / (float)m_Height, 0.1f, 100.0f);
	ViewMat = glm::rotate(glm::mat4(1.0f), glm::radians(cam_theta[1]), glm::vec3(-1.0f, 0.0f, 0.0f));
	ViewMat = glm::rotate(ViewMat, glm::radians(cam_theta[0]), glm::vec3(0.0f, 1.0f, 0.0f));
	PVMMat = ProjMat * ViewMat;
	switch (cCtrlPanel::GetInstance()->GetGravitySetting()) {
	case Gravity_Mode::Flat:
		m_CubeShader = m_CubeFlatwPhotonSphereShader;
		break;
	case Gravity_Mode::NG:
		m_CubeShader = m_CubeNGShader;
		break;
	case Gravity_Mode::GR:
		if(cCtrlPanel::GetInstance()->GetSquaredCharge() == 0)
			m_CubeShader = m_CubeGRShader;
		else
			m_CubeShader = m_CubeGRwChargeShader;
		break;
	case Gravity_Mode::Cone:
		m_CubeShader = m_CubeConeShader;
		break;
	}

	m_CubeShader->SetUniformMat4("ViewMat", ViewMat);
	m_CubeShader->SetUniformMat4("PVMMat", PVMMat);
	m_CubeShader->SetUniformFloat3("cam_position", m_position);
	m_CubeShader->SetUniformSamplerCube("u_texture", 0);
	if (cCtrlPanel::GetInstance()->GetGravitySetting() != Gravity_Mode::Cone) {
		m_CubeShader->SetUniformFloat("r_s", cCtrlPanel::GetInstance()->GetSchwarzschildRadius());
	}
	else {
		m_CubeShader->SetUniformFloat("steepness", cCtrlPanel::GetInstance()->GetConeSteepness());
	}
	if (cCtrlPanel::GetInstance()->GetGravitySetting() == Gravity_Mode::GR && cCtrlPanel::GetInstance()->GetSquaredCharge() != 0) {
		m_CubeShader->SetUniformFloat("r_Q2", cCtrlPanel::GetInstance()->GetSquaredCharge());
	}
	m_Cubemap->Bind();
	m_CubeVAO->DrawElements(m_CubeIBO, GL_TRIANGLES, m_CubeIBO->GetCount());
	// and display
	glFlush();
	SwapBuffers();
	event.Skip();
}

void cDisplay::MoveCamera() {
	glm::vec3 w_vec = { sin(glm::radians(cam_theta[0])) * cos(glm::radians(cam_theta[1])),sin(glm::radians(cam_theta[1])),-cos(glm::radians(cam_theta[0])) * cos(glm::radians(cam_theta[1])) };
	glm::vec3 d_vec = { cos(glm::radians(cam_theta[0])) ,0.0f,sin(glm::radians(cam_theta[0])) };
	glm::vec2 dir = WASDtovec2(WASDflag);
	m_position += (d_vec*dir.x + w_vec*dir.y)*0.5f;
}

void cDisplay::SetCameratoFace(GLenum target)
{
	switch (target) {
	case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		cam_theta[0] = 90;
		cam_theta[1] = 0;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		cam_theta[0] = -90;
		cam_theta[1] = 0;
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		cam_theta[0] = 180;
		cam_theta[1] = 90;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		cam_theta[0] = 180;
		cam_theta[1] = -90;
		break;
	case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		cam_theta[0] = 180;
		cam_theta[1] = 0;
		break;
	case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
		cam_theta[0] = 0;
		cam_theta[1] = 0;
		break;
	}
}

void cDisplay::SetCameratoCenter()
{
	cam_theta[0] = 180 / M_PI * atan2(-m_position.x, m_position.z);
	cam_theta[1] = 180 / M_PI * atan2(-m_position.y, sqrt(m_position.x * m_position.x + m_position.z * m_position.z));
}

void cDisplay::OnResize(wxSizeEvent& event)
{
	GetSize(&m_Width, &m_Height);
	glViewport(0, 0, m_Width, m_Height);
	event.Skip();
}

void cDisplay::OnMouseDown(wxMouseEvent& event)
{
	initialmousepos = event.GetPosition();
	init_cam_theta = cam_theta;
	event.Skip();
}
void cDisplay::OnMouseMove(wxMouseEvent& event)
{
	if (event.Dragging()) {
		wxPoint mousediff = event.GetPosition() - initialmousepos;
		cam_theta[0] = init_cam_theta[0] - mousediff.x;
		cam_theta[1] = init_cam_theta[1] + mousediff.y;
		mod(cam_theta[0], 360.0f);
		clamp(cam_theta[1], -90.0f, 90.0f);
	}
	event.Skip();
}

void cDisplay::OnKeyDown(wxKeyEvent& event)
{
	switch (event.GetUnicodeKey()) {
	case L'W':
		WASDflag |= WASD_W;
		break;
	case L'A':
		WASDflag |= WASD_A;
		break;
	case L'S':
		WASDflag |= WASD_S;
		break;
	case L'D':
		WASDflag |= WASD_D;
		break;
	}
	//cCtrlPanel::GetInstance()->display_position->SetLabel(wxString::Format("WASD flags: %i", WASDflag));
	event.Skip();
}
void cDisplay::OnKeyUp(wxKeyEvent& event)
{
	switch (event.GetUnicodeKey()) {
	case L'W':
		WASDflag &= ~WASD_W;
		break;
	case L'A':
		WASDflag &= ~WASD_A;
		break;
	case L'S':
		WASDflag &= ~WASD_S;
		break;
	case L'D':
		WASDflag &= ~WASD_D;
		break;
	}
	//cCtrlPanel::GetInstance()->display_position->SetLabel(wxString::Format("WASD flags: %i", WASDflag));
	event.Skip();
}

RenderTimer::RenderTimer(cDisplay* pane) : m_pane(pane)
{
}

void RenderTimer::Notify()
{
	m_pane->Refresh();
	m_pane->MoveCamera();
}

void RenderTimer::start()
{
	wxTimer::Start(10);
}
