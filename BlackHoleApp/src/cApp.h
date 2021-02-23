#pragma once
#include "wx/wx.h"
#include "cMain.h"

class cApp : public wxApp
{
public:
	cApp();
	~cApp();
	virtual bool OnInit();
public:
	cMain* m_main = nullptr;
};

