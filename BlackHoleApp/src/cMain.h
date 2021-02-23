#pragma once
#include "wx/wx.h"
#include "cDisplay.h"

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();
private:
	wxPanel* m_panel = nullptr;
	wxMenuBar* m_menu = nullptr;

	cDisplay* m_display = nullptr;
	RenderTimer* m_timer = nullptr;
};

