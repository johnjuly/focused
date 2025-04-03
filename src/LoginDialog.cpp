// LoginDialog.cpp
#include "LoginDialog.h"
#include "Database.h"

LoginDialog::LoginDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "用户登录", wxDefaultPosition, wxSize(350, 200)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 现代输入框样式
    usernameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_CENTER);
    passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD|wxTE_CENTER);
    usernameCtrl->SetHint("用户名");
    passwordCtrl->SetHint("密码");

    // 扁平化登录按钮
    wxButton* loginBtn = new wxButton(this, wxID_OK, "登 录");
    loginBtn->SetBackgroundColour(Style::PRIMARY);
    loginBtn->SetForegroundColour(*wxWHITE);

    // 布局管理
    mainSizer->Add(usernameCtrl, 0, wxALIGN_CENTER|wxTOP, 20);
    mainSizer->Add(passwordCtrl, 0, wxALIGN_CENTER|wxTOP, 10);
    mainSizer->Add(loginBtn, 0, wxALIGN_CENTER|wxTOP, 15);

    // 视觉优化
    SetBackgroundColour(Style::MAIN_BG);
    loginBtn->Bind(wxEVT_BUTTON, &LoginDialog::OnLogin, this);
    SetSizerAndFit(mainSizer);
}

void LoginDialog::OnLogin(wxCommandEvent& event) {
    wxString user = usernameCtrl->GetValue().Trim();
    wxString pass = passwordCtrl->GetValue().Trim();

    if (user.empty() || pass.empty()) {
        wxMessageBox("用户名和密码不能为空", "输入错误", wxOK|wxICON_WARNING);
        return;
    }

    if (DatabaseManager::GetInstance().ValidateUser(user, pass)) {
        username = user;
        // 获取用户ID（需在DatabaseManager添加方法）
        userId = DatabaseManager::GetInstance().GetUserId(user.ToStdString());
        EndModal(wxID_OK);
    } else {
        wxMessageBox("用户名或密码错误", "登录失败", wxOK|wxICON_ERROR);
    }
}
