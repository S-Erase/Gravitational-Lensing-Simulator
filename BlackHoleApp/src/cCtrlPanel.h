#pragma once
#include "wx/wx.h"

class cCtrlPanel : public wxPanel
{
public:
	static cCtrlPanel* GetInstance() { return m_Instance; }
	cCtrlPanel(wxWindow* parent);
	~cCtrlPanel();

	inline float GetSchwarzschildRadius() const { return schwar_radius; }
	inline float GetSquaredCharge() const { return electric_charge*electric_charge; }
	inline int GetGravitySetting() const { return gravity_mode; }

	wxStaticText* display_size = nullptr;
	wxStaticText* display_camera = nullptr;
	wxStaticText* display_position = nullptr;
private:
	static cCtrlPanel* m_Instance;

	wxButton* frontfilectrl = nullptr;
	wxButton* backfilectrl = nullptr;
	wxButton* rightfilectrl = nullptr;
	wxButton* leftfilectrl = nullptr;
	wxButton* upfilectrl = nullptr;
	wxButton* downfilectrl = nullptr;
	wxButton* netfilectrl = nullptr;

	wxSlider* radius_slider = nullptr;
	float schwar_radius = 3.0f;

	wxSlider* charge_slider = nullptr;
	float electric_charge = 0.0f;
	wxButton* resetCharge = nullptr;

	wxRadioButton* flatmode = nullptr;
	wxRadioButton* NGmode = nullptr;
	wxRadioButton* GRmode = nullptr;
	int gravity_mode = 0;

	wxButton* frontcamctrl = nullptr;
	wxButton* backcamctrl = nullptr;
	wxButton* rightcamctrl = nullptr;
	wxButton* leftcamctrl = nullptr;
	wxButton* upcamctrl = nullptr;
	wxButton* downcamctrl = nullptr;
	wxButton* centercamctrl = nullptr;
private:
	void OnLoadFace(wxCommandEvent& event);
	void OnLoadNet(wxCommandEvent& event);
	void OnRadiusSlider(wxCommandEvent& event);
	void OnChargeSlider(wxCommandEvent& event);
	void OnResetCharge(wxCommandEvent& event);
	void OnGravitySettings(wxCommandEvent& event);
	void OnFaceCamera(wxCommandEvent& event);
	void OnCenterCamera(wxCommandEvent& event);
};

