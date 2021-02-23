#include "cCtrlPanel.h"
#include "cDisplay.h"
#include <wx/numdlg.h>
#include <wx/msgdlg.h> 

enum {R_Face, L_Face, U_Face, D_Face, B_Face, F_Face, Net_Face, F_Grav, NG_Grav, GR_Grav};

cCtrlPanel* cCtrlPanel::m_Instance = nullptr;
cCtrlPanel::cCtrlPanel(wxWindow* parent) : wxPanel(parent)
{
	if (!m_Instance)
		m_Instance = this;

	wxStaticBoxSizer* stbox0 = new wxStaticBoxSizer(wxVERTICAL, this, "Skybox faces");

	frontfilectrl	= new wxButton(this, F_Face, "Load Front Face");
	backfilectrl	= new wxButton(this, B_Face, "Load Back Face");
	rightfilectrl	= new wxButton(this, R_Face, "Load Right Face");
	leftfilectrl	= new wxButton(this, L_Face, "Load Left Face");
	upfilectrl		= new wxButton(this, U_Face, "Load Upward Face");
	downfilectrl	= new wxButton(this, D_Face, "Load Downward Face");

	netfilectrl = new wxButton(this, Net_Face, "Load Cube Net");

	stbox0->Add(frontfilectrl);
	stbox0->Add(backfilectrl);
	stbox0->Add(rightfilectrl);
	stbox0->Add(leftfilectrl);
	stbox0->Add(upfilectrl);
	stbox0->Add(downfilectrl);
	stbox0->Add(netfilectrl);

	wxStaticBoxSizer* stbox1 = new wxStaticBoxSizer(wxVERTICAL, this, "Gravity");
	wxBoxSizer* hbox1 = new wxBoxSizer(wxHORIZONTAL);

	hbox1->Add(new wxStaticText(this, wxID_ANY, "Schwarzschild radius"));
	radius_slider = new wxSlider(this, wxID_ANY, 300,0,1000);
	hbox1->Add(radius_slider);
	stbox1->Add(hbox1);

	flatmode =	new wxRadioButton(this, F_Grav, "No Gravity", wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	NGmode =	new wxRadioButton(this, NG_Grav, "Newtonian Gravity");
	GRmode =	new wxRadioButton(this, GR_Grav, "General Relativity");
	stbox1->Add(flatmode);
	stbox1->Add(NGmode);
	stbox1->Add(GRmode);

	display_size = new wxStaticText(this, wxID_ANY, "Screen Size: ----,----");
	display_camera = new wxStaticText(this, wxID_ANY, "Camera Angle: ----,----");
	display_position = new wxStaticText(this, wxID_ANY, "Position: ----,----,----");
	stbox1->Add(display_size);
	stbox1->Add(display_camera);
	stbox1->Add(display_position);

	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, F_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, B_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, R_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, L_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, U_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadFace, this, D_Face);
	Bind(wxEVT_BUTTON, &cCtrlPanel::OnLoadNet , this, Net_Face);
	Bind(wxEVT_SLIDER, &cCtrlPanel::OnRadiusSlider, this, radius_slider->GetId());
	Bind(wxEVT_RADIOBUTTON, &cCtrlPanel::OnGravitySettings, this, flatmode->GetId());
	Bind(wxEVT_RADIOBUTTON, &cCtrlPanel::OnGravitySettings, this, NGmode->GetId());
	Bind(wxEVT_RADIOBUTTON, &cCtrlPanel::OnGravitySettings, this, GRmode->GetId());
	Bind(wxEVT_SIZE, &cCtrlPanel::OnResize, this);

	wxBoxSizer* hbox = new wxBoxSizer(wxHORIZONTAL);
	hbox->Add(stbox0);
	hbox->Add(stbox1);

	SetSizer(hbox);
}

cCtrlPanel::~cCtrlPanel()
{
}

void cCtrlPanel::OnLoadFace(wxCommandEvent& event) {
	wxString message;
	switch (event.GetId()) {
	case F_Face:
		message = "Open Front Face Texture";
		break;
	case B_Face:
		message = "Open Back Face Texture";
		break;
	case R_Face:
		message = "Open Right Face Texture";
		break;
	case L_Face:
		message = "Open Left Face Texture";
		break;
	case U_Face:
		message = "Open Upward Face Texture";
		break;
	case D_Face:
		message = "Open Downward Face Texture";
		break;
	}
	wxString wildcard = "PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg";
	wxString filepath = wxFileSelector(message, wxEmptyString, wxEmptyString, wxEmptyString, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	cDisplay::GetInstance()->SetFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + event.GetId()-R_Face, static_cast<const char*>(filepath));
	event.Skip();
}
void cCtrlPanel::OnLoadNet(wxCommandEvent& event) {
	wxString wildcard = "PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg";
	wxString filepath = wxFileSelector("Open Net Texture", wxEmptyString, wxEmptyString, wxEmptyString, wildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	cDisplay::GetInstance()->UseNet(static_cast<const char*>(filepath));
	event.Skip();
}
void cCtrlPanel::OnRadiusSlider(wxCommandEvent& event) {
	schwar_radius = (float)radius_slider->GetValue() / 100.0f;
	event.Skip();
}
void cCtrlPanel::OnGravitySettings(wxCommandEvent& event) {
	gravity_mode = event.GetId() - F_Grav;
	event.Skip();
}

void cCtrlPanel::OnResize(wxSizeEvent& event) {
	int x, y;
	cDisplay::GetInstance()->GetSize(&x, &y);
	display_size->SetLabel(wxString::Format("Screen Size: %i, %i", x,y));
	event.Skip();
}