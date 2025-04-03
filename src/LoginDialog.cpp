// LoginDialog.cpp
#include "LoginDialog.h"
#include "Database.h"

LoginDialog::LoginDialog(wxWindow* parent)
    : wxDialog(parent, wxID_ANY, "�û���¼", wxDefaultPosition, wxSize(350, 200)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // �ִ��������ʽ
    usernameCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_CENTER);
    passwordCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1), wxTE_PASSWORD|wxTE_CENTER);
    usernameCtrl->SetHint("�û���");
    passwordCtrl->SetHint("����");

    // ��ƽ����¼��ť
    wxButton* loginBtn = new wxButton(this, wxID_OK, "�� ¼");
    loginBtn->SetBackgroundColour(Style::PRIMARY);
    loginBtn->SetForegroundColour(*wxWHITE);

    // ���ֹ���
    mainSizer->Add(usernameCtrl, 0, wxALIGN_CENTER|wxTOP, 20);
    mainSizer->Add(passwordCtrl, 0, wxALIGN_CENTER|wxTOP, 10);
    mainSizer->Add(loginBtn, 0, wxALIGN_CENTER|wxTOP, 15);

    // �Ӿ��Ż�
    SetBackgroundColour(Style::MAIN_BG);
    loginBtn->Bind(wxEVT_BUTTON, &LoginDialog::OnLogin, this);
    SetSizerAndFit(mainSizer);
}

void LoginDialog::OnLogin(wxCommandEvent& event) {
    wxString user = usernameCtrl->GetValue().Trim();
    wxString pass = passwordCtrl->GetValue().Trim();

    if (user.empty() || pass.empty()) {
        wxMessageBox("�û��������벻��Ϊ��", "�������", wxOK|wxICON_WARNING);
        return;
    }

    if (DatabaseManager::GetInstance().ValidateUser(user, pass)) {
        username = user;
        // ��ȡ�û�ID������DatabaseManager��ӷ�����
        userId = DatabaseManager::GetInstance().GetUserId(user.ToStdString());
        EndModal(wxID_OK);
    } else {
        wxMessageBox("�û������������", "��¼ʧ��", wxOK|wxICON_ERROR);
    }
}
