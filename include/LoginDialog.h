#pragma once
#include <wx/wx.h>

class LoginDialog : public wxDialog {
public:
    LoginDialog(wxWindow* parent);
    const wxString& GetUsername() const { return username; }
    int GetUserId() const { return userId; }

private:
    void OnLogin(wxCommandEvent& event);
    wxTextCtrl *usernameCtrl, *passwordCtrl;
    wxString username;
    int userId = -1;
};
