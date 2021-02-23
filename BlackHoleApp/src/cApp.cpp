#include "cApp.h"

IMPLEMENT_APP(cApp)
cApp::cApp()
{
}

cApp::~cApp()
{
}

bool cApp::OnInit()
{
    m_main = new cMain();
    m_main->Show();
    return true;
}
