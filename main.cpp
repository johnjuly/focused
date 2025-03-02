#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/image.h>

class MyApp : public wxApp {
public:
    virtual bool OnInit(){
        //���������ڲ��󶨹ر��¼�
         wxInitAllImageHandlers();
        MainFrame *frame = new MainFrame();
        wxIcon icon;
        icon.LoadFile("focused.png", wxBITMAP_TYPE_PNG);
        frame->Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose,frame);
        frame->SetIcon(icon);
        frame->Show();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
