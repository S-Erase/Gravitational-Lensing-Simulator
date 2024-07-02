#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include "wx/wx.h"
#include "wx/glcanvas.h"
#include <wx/timer.h>
#include "cOpenGLObjects.h"
#include <array>

template<typename T>
void clamp(T& x, T lo, T hi) {
    if (x > hi) x = hi;
    if (x < lo) x = lo;
}
template<typename T>
void mod(T& x, T modulo) {
    x -= modulo * floor(x / modulo);
}

class cDisplay : public wxGLCanvas
{
public:
	cDisplay(wxWindow* parent);
	~cDisplay();
    static cDisplay* GetInstance() { return m_instance; }

    inline void SetFace(GLenum target, const char* filepath) { m_Cubemap->SetTexture(target, filepath); }
    inline void UseNet(const char* filepath) { m_Cubemap->UseNet(filepath); }

    void InitializeGLEW();
    void Setup();
    void OnPaint(wxPaintEvent& event);
    void MoveCamera();
    void SetCameratoFace(GLenum target);
    void SetCameratoCenter();
private:
    static cDisplay* m_instance;
	wxGLContext m_context;

    int m_Width, m_Height;

    wxPoint initialmousepos;

    std::shared_ptr<cOpenGLStaticVBO> m_CubeVBO;
    std::shared_ptr<cOpenGLStaticIBO> m_CubeIBO;
    std::unique_ptr<cOpenGLVAO> m_CubeVAO;
    std::shared_ptr<cOpenGLShader> m_CubeShader;
    std::shared_ptr<cOpenGLShader> m_CubeFlatShader;
    std::shared_ptr<cOpenGLShader> m_CubeFlatwPhotonSphereShader;
    std::shared_ptr<cOpenGLShader> m_CubeNGShader;
    std::shared_ptr<cOpenGLShader> m_CubeGRShader;
    std::shared_ptr<cOpenGLShader> m_CubeGRwChargeShader;
    std::shared_ptr<cOpenGLShader> m_CubeConeShader;
    std::shared_ptr<cOpenGLCubemap> m_Cubemap;

    std::array<float, 2> init_cam_theta = { 0.0f, 0.0f };
    std::array<float, 2> cam_theta = { 0.0f, 0.0f };
    glm::vec3 m_position = { 0.0f, 0.0f, 10.0f };
    glm::mat4 ProjMat, ViewMat, PVMMat;

    int WASDflag = 0;
private:
    void OnResize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
};

class RenderTimer : public wxTimer
{
    cDisplay* m_pane;
public:
    RenderTimer(cDisplay* pane);
    void Notify();
    void start();
};