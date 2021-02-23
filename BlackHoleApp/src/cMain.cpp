#include "cMain.h"
#include "cDisplay.h"
#include "cCtrlPanel.h"

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Gravitational Lensing Simulator", wxDefaultPosition, wxSize(960,720))
{
	m_menu = new wxMenuBar;

	m_panel = new wxPanel(this);

	wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);
	m_display = new cDisplay(m_panel);
	vbox->Add(m_display,1, wxEXPAND);
	vbox->Add(new cCtrlPanel(m_panel));
	m_panel->SetSizer(vbox);

	m_timer = new RenderTimer(m_display);
	Show();
	m_timer->start();
}
cMain::~cMain()
{
	delete m_timer;
}
