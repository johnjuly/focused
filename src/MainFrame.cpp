#include "MainFrame.h"
#include "RainAnimation.h"
#include "GlacierAnimation.h"
#include "FireAnimation.h"

#include <wx/clrpicker.h> // ��ɫѡ��ؼ�ͷ�ļ�
#include "SoundButton.h"
#include <wx/fontpicker.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>  // ��ɫ�Ի���
#include <wx/fontdlg.h>   // ����Ի���


MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "focused",wxDefaultPosition, wxSize(800, 600)),
    timer(new wxTimer(this)), // ��ʼ����ʱ��
      remainingSeconds(0),
      isRunning(false)
{

    this->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);// �󶨼�ʱ���¼�
    InitUI();
}



void MainFrame::InitUI(){
    wxSplitterWindow* splitter = new wxSplitterWindow(this);


    // ����������
    wxScrolledWindow* soundPanel = new wxScrolledWindow(splitter);
    CreateSoundPanel(soundPanel);


    rightPanel = new wxPanel(splitter); // ����wxPanel��Ϊ����

     CreateAnimationPanel();
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    rightPanel->SetSizer(rightSizer);

// ������ʱ����壨������壩
wxPanel* timerPanel = new wxPanel(rightPanel);
CreateTimerPanel(timerPanel);

    // ���÷ָ�ڲ���
    splitter->SplitVertically(soundPanel, rightPanel, 300);
    splitter->SetMinimumPaneSize(200);

    // ���Ҳ�����С�仯�¼�
    /*rightPanel->Bind(wxEVT_SIZE, [=](wxSizeEvent& evt) {
        rainAnim->SetSize(evt.GetSize()); // �������������С
        evt.Skip();*/


// ��������
rightSizer->Add(timerPanel, 1, wxEXPAND); // ��ʱ�����ռ1��
rightSizer->Add(animPanel, 0, wxEXPAND);  // ��������̶��߶�



    // �ؼ������÷ָ�������
    splitter->SplitVertically(soundPanel, rightPanel, 300);
    splitter->SetMinimumPaneSize(200);



     // ������ô˺�����ʾ�ָ���
    splitter->SetSizer(new wxBoxSizer(wxHORIZONTAL));
    splitter->Show(true);


// �޸ĳߴ��¼���
/*
 animPanel->Bind(wxEVT_SIZE, [=](wxSizeEvent& evt) {
        rainAnim->SetSize(evt.GetSize());

    evt.Skip();

    });

*/
    this->SetMenuBar(createMenu());
}

// ����������ť���
void MainFrame::CreateSoundPanel(wxWindow* parent){
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ��������
    wxSlider* volumeSlider = new wxSlider(parent, wxID_ANY, 100, 0, 100);
    volumeSlider->Bind(wxEVT_SLIDER, [=](wxCommandEvent& e){
        float volume = e.GetInt() / 100.0f;
        for(auto btn : soundButtons){
            btn->SetVolume(volume);// ��������������ť����
        }
    });

    // ������ť����
    wxGridSizer* grid = new wxGridSizer(2, 5, 5);
    const std::vector<std::pair<wxString, std::string>> sounds = {
        {"����", "rain.wav"},
        {"���ȹ�", "cafe.wav"},
        {"����", "icebergs.wav"},
        {"����", "wind-in-forest.wav"},
        {"����", "waves.wav"},
        {"ľ��", "fire.wav"}
    };

    for(const auto& [name, path] : sounds){
        SoundButton* btn = new SoundButton(parent, name, path);
        btn->SetMinSize(wxSize(140, 80));
        btn->Bind(wxEVT_TOGGLEBUTTON, [=](wxCommandEvent& e){
            btn->ToggleSound();// �л�����/ֹͣ
           /* btn->SetBackgroundColour(
                e.IsChecked() ? wxColour(220, 240, 220) : *wxWHITE
            );
                */
             // ����궯������

        if(e.IsChecked()) {
        std::string animType = "";
        if(name == "����") animType = "rain";
        else if(name == "����") animType = "glacier";
        else if(name == "ľ��") animType = "fire";
        else if(name == "����") animType = "wind";

        if(!animType.empty()) {
            SwitchAnimation(animType);
        }
    } else {
        if(currentAnim) {
            currentAnim->Stop();
            currentAnim->Hide();
        }
    }



        });
        soundButtons.push_back(btn);
        grid->Add(btn, 1, wxEXPAND);
    }
     // ���ֿؼ�
    mainSizer->Add(volumeSlider, 0, wxEXPAND|wxALL, 10);
    mainSizer->Add(grid, 1, wxEXPAND|wxALL, 10);
    parent->SetSizer(mainSizer);
    dynamic_cast<wxScrolledWindow*>(parent)->SetScrollRate(10, 10); // ��ȷ
}

// ������ʱ�����
void MainFrame::CreateTimerPanel(wxWindow* parent){

     wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);



    timeDisplay = new wxStaticText(parent, wxID_ANY, "25:00",
                                wxDefaultPosition, wxDefaultSize,
                                wxALIGN_CENTER);// ʱ����ʾ�ؼ�

    timeDisplay->SetFont({48, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, "Arial"});
     timeDisplay->SetForegroundColour({30, 144, 255});

    // ��ʼ��ʱ������ؼ�
    wxSpinCtrl* timeInput = new wxSpinCtrl(parent, wxID_ANY, "25",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSP_ARROW_KEYS, 1, 120);

         //��ʼ����ť��ʹ�ó�Ա����
     startBtn = new wxButton(parent, wxID_ANY, "��ʼ");
     stopBtn = new wxButton(parent, wxID_ANY, "ֹͣ");



    //��ʱ�������¼�
    timeInput->Bind(wxEVT_SPINCTRL, [=](wxCommandEvent& e){
                    int minutes =e.GetInt();
                    remainingSeconds = minutes * 60;//����ת��Ϊ��
        timeDisplay->SetLabel(wxString::Format("%02d:00", minutes));
    });



    //�󶨿�ʼ��ť�¼�
    startBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        if(!isRunning){
                    timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);

        // ���°��¼�
        timer->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
             remainingSeconds = timeInput->GetValue() * 60;
            isRunning = true;
            timer->Start(1000);//������ʱ�������1��
            timeDisplay->SetLabel(wxString::Format("%02d:00", remainingSeconds / 60));
        }
    });




    //��ֹͣ��ť�¼�
    stopBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        timer->Stop();
        isRunning = false;
        remainingSeconds = 0;
        timeDisplay->SetLabel("00:00");
    });



    //���ֿؼ�


    /* sizer->Add(timeDisplay, 1, wxEXPAND|wxALL, 20);
    sizer->Add(timeInput, 0, wxEXPAND|wxALL, 10);
    sizer->Add(startBtn, 0, wxEXPAND|wxALL, 5);
    sizer->Add(stopBtn, 0, wxEXPAND|wxALL, 5);
    parent->SetSizer(sizer);*/
     sizer->AddStretchSpacer();
    sizer->Add(timeDisplay, 0, wxALIGN_CENTER|wxBOTTOM, 20);
    sizer->Add(timeInput, 0, wxALIGN_CENTER|wxBOTTOM, 10);
    sizer->Add(startBtn, 0, wxALIGN_CENTER|wxBOTTOM, 5);
    sizer->Add(stopBtn, 0, wxALIGN_CENTER);
    sizer->AddStretchSpacer();

    parent->SetSizer(sizer);
}


void MainFrame::CreateAnimationPanel() {
    animPanel = new wxPanel(rightPanel);
    animPanel->SetMinSize(wxSize(-1, 200));

    // ��ʼ�����ж���
    animMap["rain"] = new RainAnimation(animPanel);
    animMap["glacier"] = new GlacierAnimation(animPanel);
    animMap["fire"] = new FireAnimation(animPanel);
    /*animMap["wind"] = new WindAnimation(animPanel);*/

    // ��ʼ�������ж���
    for(auto& [key, anim] : animMap) {
        anim->Hide();
    }

}
// ��ʱ���¼�����ÿ�����ʣ��ʱ��
void MainFrame::OnTimerTick(wxTimerEvent&){
    if(remainingSeconds > 0){
        remainingSeconds--;

        //����ʱ����ʾ
        int mins = remainingSeconds / 60;
        int secs = remainingSeconds % 60;
        timeDisplay->SetLabel(wxString::Format("%02d:%02d", mins, secs));

        //ǿ��ˢ�½���
        timeDisplay->Refresh();

    }else {if (isRunning&& remainingSeconds == 0){



        timer->Stop();
        timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
        wxMessageDialog dialog(nullptr, "��Ϣһ�°�(�������", "��ʾ", wxOK | wxICON_NONE);
         dialog.ShowModal();


        isRunning = false;
         remainingSeconds = -1;
         timeDisplay->SetLabel("00:00");}}


}

wxMenuBar* MainFrame::createMenu() {
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->SetBackgroundColour(ThemeManager::Get().GetCurrentTheme().cardBg);

    // �ļ��˵�
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "�˳�(&Q)");



    // �û��˵�
    wxMenu* userMenu = new wxMenu;
    userMenu->Append(2001, "רעͳ��(&S)");

    menuBar->Append(fileMenu, "�ļ�(&F)");

    menuBar->Append(userMenu, "�û�(&U)");


    wxMenu* themeMenu = new wxMenu;


    themeMenu->Append(ID_THEME_LIGHT, "Nord Light");
themeMenu->Append(ID_THEME_DARK, "Nord Dark");

themeMenu->AppendSeparator();
themeMenu->Append(ID_THEME_CUSTOM, "�Զ�������...");
 menuBar->Append(themeMenu, "����(&T)");

// �󶨲˵��¼�
Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    ApplyNordLightTheme();
}, ID_THEME_LIGHT);

Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    ApplyNordDarkTheme();
}, ID_THEME_DARK);

Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    wxDialog dlg(this, wxID_ANY, "�Զ�������");
    wxPanel* panel = new wxPanel(&dlg);

    // ��ɫѡ��ؼ�
    wxColourPickerCtrl* bgPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().mainBg);
    wxColourPickerCtrl* textPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().textPrimary);

    // ����
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "������ɫ:"), 0, wxALL, 5);
    sizer->Add(bgPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "�ı���ɫ:"), 0, wxALL, 5);
    sizer->Add(textPicker, 0, wxEXPAND | wxALL, 5);

    // ȷ�ϰ�ť
    wxButton* btnOK = new wxButton(panel, wxID_OK, "ȷ��");
    sizer->Add(btnOK, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(sizer);
    if (dlg.ShowModal() == wxID_OK) {
        ThemeConfig customTheme;
        customTheme.mainBg = bgPicker->GetColour();
        customTheme.textPrimary = textPicker->GetColour();
        ThemeManager::Get().SetTheme(customTheme);
         OnThemeChanged();
    }
}, ID_THEME_CUSTOM);

    // �¼���
    Bind(wxEVT_MENU, [=](wxCommandEvent&) {
        wxColourDialog dlg(this);
        if(dlg.ShowModal() == wxID_OK) {
            ApplyColorToAll(dlg.GetColourData().GetColour());
        }
    }, 1001);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) {
        wxFontDialog dlg(this);
        if(dlg.ShowModal() == wxID_OK) {
            ApplyFontToDisplay(dlg.GetFontData().GetChosenFont());
        }
    }, 1002);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) {
        ApplyColorToAll(ThemeManager::Get().GetCurrentTheme().cardBg);
        ApplyFontToDisplay(ThemeManager::Get().GetCurrentTheme().timeFont);
    }, 1003);

    Bind(wxEVT_MENU, &MainFrame::OnShowStatistics, this, 2001);

    Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    Close(true); // �ر�������
}, wxID_EXIT);

    return menuBar;


}
void MainFrame::SetColorRecursive(wxWindow* window, wxColour color) {
    if (!window) return;

    window->SetBackgroundColour(color);
    window->Refresh();

    wxWindowList children = window->GetChildren();
    for (wxWindow* child : children) {
        SetColorRecursive(child, color);  // �ݹ������Ӵ�����ɫ
    }
}

// ʵ����ɫӦ�ù���
// MainFrame.cpp
void MainFrame::ApplyColorToAll(wxColour color) {

    std::function<void(wxWindow*)> SetColorRecursive;

    SetColorRecursive = [&](wxWindow* win) {
        win->SetBackgroundColour(color);
        win->Refresh();

        // �ݹ鴦���ӿؼ�
        wxWindowList& children = win->GetChildren();
        for (wxWindow* child : children) {
            SetColorRecursive(child);
        }
    };

    // �������ڿ�ʼӦ��
    SetColorRecursive(this);

    // ����ؼ�����
    if(timeDisplay) {
        timeDisplay->SetBackgroundColour(color.ChangeLightness(95));
    }
    currentBgColor = color;
}

// ʵ������Ӧ�ù���
void MainFrame::ApplyFontToDisplay(wxFont font) {
    timeDisplay->SetFont(font);
    currentTimeFont = font;
    Layout(); // �Զ���������
}


void MainFrame::ApplyThemeToWindow(wxWindow* window, const ThemeConfig& theme) {
    if (!window) return;


     if(window == this) {
        window->SetBackgroundColour(theme.mainBg.ChangeLightness(105));
    } else {
        window->SetBackgroundColour(theme.cardBg);
    }

    window->SetForegroundColour(theme.textPrimary);

    // ����ؼ�����
    if(auto btn = dynamic_cast<wxButton*>(window)){
        btn->SetBackgroundColour(theme.frost[0]); // nord7
        btn->SetForegroundColour(theme.polar_night[0]);
    }

    if(auto st = dynamic_cast<wxStaticText*>(window)) {
        st->SetForegroundColour(theme.frost[3]); // nord10
    }
    // �ݹ鴦���ӿؼ�
    wxWindowList& children = window->GetChildren();
    for (wxWindow* child : children) {
        ApplyThemeToWindow(child, theme);
    }
}

// MainFrame.cpp���Զ�������Ի���
void MainFrame::OnCustomTheme(wxCommandEvent&) {
    wxDialog dlg(this, wxID_ANY, "�Զ�������");
    wxPanel* panel = new wxPanel(&dlg);

    // ��ɫѡ��ؼ�
    wxColourPickerCtrl* bgColorPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().mainBg);
    wxColourPickerCtrl* textColorPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().textPrimary);

    // ����ѡ��ؼ�
    wxFontPickerCtrl* titleFontPicker = new wxFontPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().titleFont);
    wxFontPickerCtrl* timeFontPicker = new wxFontPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().timeFont);

    // ����
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "������ɫ:"), 0, wxALL, 5);
    sizer->Add(bgColorPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "�ı���ɫ:"), 0, wxALL, 5);
    sizer->Add(textColorPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "��������:"), 0, wxALL, 5);
    sizer->Add(titleFontPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "ʱ������:"), 0, wxALL, 5);
    sizer->Add(timeFontPicker, 0, wxEXPAND | wxALL, 5);

    wxButton* btnOK = new wxButton(panel, wxID_OK, "ȷ��");
    sizer->Add(btnOK, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(sizer);
    if (dlg.ShowModal() == wxID_OK) {
        ThemeConfig customTheme;
        customTheme.mainBg = bgColorPicker->GetColour();
        customTheme.textPrimary = textColorPicker->GetColour();
        customTheme.titleFont = titleFontPicker->GetSelectedFont();
        customTheme.timeFont = timeFontPicker->GetSelectedFont();
        ThemeManager::Get().SetTheme(customTheme);
         OnThemeChanged();
    }
}
// �������¼�����
void MainFrame::OnThemeChanged() {
    const auto& theme = ThemeManager::Get().GetCurrentTheme();
    ApplyThemeToWindow(this, theme);
    Refresh(); // ǿ���ػ����

     // ����ؼ�����
    if(timeDisplay) {
        timeDisplay->SetForegroundColour(theme.textPrimary);
        timeDisplay->SetBackgroundColour(theme.mainBg);
    }
    rightPanel->SetBackgroundColour(theme.cardBg);

    Refresh();
    Update();
    Layout();

}

void MainFrame::OnShowStatistics(wxCommandEvent& event) {
    wxString stats = wxString::Format(
        "����רעʱ����%.1fСʱ\n�����ۼƣ�%.1fСʱ",
        12.5,  // ʾ�����ݣ����滻Ϊʵ��ͳ���߼�
        48.3
    );
    wxMessageBox(stats, "רעͳ��", wxOK | wxICON_INFORMATION);
}

void MainFrame::ApplyNordLightTheme() {
    ThemeConfig theme;
    // ��������ɫ
    theme.mainBg = theme.snow_storm[2];   // nord6
    theme.cardBg = theme.snow_storm[1];   // nord5
    theme.textPrimary = theme.polar_night[0]; // nord0
    theme.timeFont = wxFont(24, wxFONTFAMILY_SWISS,
                          wxFONTSTYLE_NORMAL,
                          wxFONTWEIGHT_BOLD);

    ThemeManager::Get().SetTheme(theme);
    OnThemeChanged();
}

void MainFrame::ApplyNordDarkTheme() {
    ThemeConfig theme;
    // ��ҹ��ɫ
    theme.mainBg = theme.polar_night[0];  // nord0
    theme.cardBg = theme.polar_night[1];  // nord1
    theme.textPrimary = theme.snow_storm[0]; // nord4
    theme.timeFont = wxFont(36, wxFONTFAMILY_SWISS,
                          wxFONTSTYLE_ITALIC,
                          wxFONTWEIGHT_BOLD);

    ThemeManager::Get().SetTheme(theme);
    OnThemeChanged();
}



void MainFrame::SwitchAnimation(const std::string& animType) {
    // ֹͣ��ǰ����
    if(currentAnim) {
        currentAnim->Stop();
        currentAnim->Hide();
    }

    // �����¶���
    if(animMap.find(animType) != animMap.end()) {
        currentAnim = animMap[animType];
        currentAnim->SetSize(animPanel->GetClientSize());
        currentAnim->Show();
        currentAnim->Start();
        animPanel->Layout();
    }
}

void MainFrame::OnClose(wxCloseEvent& event){
    for(auto btn : soundButtons){
        if(btn->GetValue()) btn->ToggleSound();
    }
    event.Skip();
}

