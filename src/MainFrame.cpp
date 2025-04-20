#include "MainFrame.h"
#include "RainAnimation.h"
#include "GlacierAnimation.h"
#include "FireAnimation.h"

#include <wx/clrpicker.h> // 颜色选择控件头文件
#include "SoundButton.h"
#include <wx/fontpicker.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>  // 颜色对话框
#include <wx/fontdlg.h>   // 字体对话框


MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, "focused",wxDefaultPosition, wxSize(800, 600)),
    timer(new wxTimer(this)), // 初始化计时器
      remainingSeconds(0),
      isRunning(false)
{

    this->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);// 绑定计时器事件
    InitUI();
}



void MainFrame::InitUI(){
    wxSplitterWindow* splitter = new wxSplitterWindow(this);


    // 左侧声音面板
    wxScrolledWindow* soundPanel = new wxScrolledWindow(splitter);
    CreateSoundPanel(soundPanel);


    rightPanel = new wxPanel(splitter); // 改用wxPanel作为容器

     CreateAnimationPanel();
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    rightPanel->SetSizer(rightSizer);

// 创建计时器面板（独立面板）
wxPanel* timerPanel = new wxPanel(rightPanel);
CreateTimerPanel(timerPanel);

    // 设置分割窗口布局
    splitter->SplitVertically(soundPanel, rightPanel, 300);
    splitter->SetMinimumPaneSize(200);

    // 绑定右侧面板大小变化事件
    /*rightPanel->Bind(wxEVT_SIZE, [=](wxSizeEvent& evt) {
        rainAnim->SetSize(evt.GetSize()); // 动画层跟随面板大小
        evt.Skip();*/


// 布局设置
rightSizer->Add(timerPanel, 1, wxEXPAND); // 计时器面板占1份
rightSizer->Add(animPanel, 0, wxEXPAND);  // 动画区域固定高度



    // 关键：设置分割器布局
    splitter->SplitVertically(soundPanel, rightPanel, 300);
    splitter->SetMinimumPaneSize(200);



     // 必须调用此函数显示分割器
    splitter->SetSizer(new wxBoxSizer(wxHORIZONTAL));
    splitter->Show(true);


// 修改尺寸事件绑定
/*
 animPanel->Bind(wxEVT_SIZE, [=](wxSizeEvent& evt) {
        rainAnim->SetSize(evt.GetSize());

    evt.Skip();

    });

*/
    this->SetMenuBar(createMenu());
}

// 创建声音按钮面板
void MainFrame::CreateSoundPanel(wxWindow* parent){
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 音量控制
    wxSlider* volumeSlider = new wxSlider(parent, wxID_ANY, 100, 0, 100);
    volumeSlider->Bind(wxEVT_SLIDER, [=](wxCommandEvent& e){
        float volume = e.GetInt() / 100.0f;
        for(auto btn : soundButtons){
            btn->SetVolume(volume);// 调整所有声音按钮音量
        }
    });

    // 声音按钮网格
    wxGridSizer* grid = new wxGridSizer(2, 5, 5);
    const std::vector<std::pair<wxString, std::string>> sounds = {
        {"雨声", "rain.wav"},
        {"咖啡馆", "cafe.wav"},
        {"冰川", "icebergs.wav"},
        {"风声", "wind-in-forest.wav"},
        {"海浪", "waves.wav"},
        {"木火", "fire.wav"}
    };

    for(const auto& [name, path] : sounds){
        SoundButton* btn = new SoundButton(parent, name, path);
        btn->SetMinSize(wxSize(140, 80));
        btn->Bind(wxEVT_TOGGLEBUTTON, [=](wxCommandEvent& e){
            btn->ToggleSound();// 切换播放/停止
           /* btn->SetBackgroundColour(
                e.IsChecked() ? wxColour(220, 240, 220) : *wxWHITE
            );
                */
             // 添加雨动画控制

        if(e.IsChecked()) {
        std::string animType = "";
        if(name == "雨声") animType = "rain";
        else if(name == "冰川") animType = "glacier";
        else if(name == "木火") animType = "fire";
        else if(name == "风声") animType = "wind";

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
     // 布局控件
    mainSizer->Add(volumeSlider, 0, wxEXPAND|wxALL, 10);
    mainSizer->Add(grid, 1, wxEXPAND|wxALL, 10);
    parent->SetSizer(mainSizer);
    dynamic_cast<wxScrolledWindow*>(parent)->SetScrollRate(10, 10); // 正确
}

// 创建计时器面板
void MainFrame::CreateTimerPanel(wxWindow* parent){

     wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);



    timeDisplay = new wxStaticText(parent, wxID_ANY, "25:00",
                                wxDefaultPosition, wxDefaultSize,
                                wxALIGN_CENTER);// 时间显示控件

    timeDisplay->SetFont({48, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, "Arial"});
     timeDisplay->SetForegroundColour({30, 144, 255});

    // 初始化时间输入控件
    wxSpinCtrl* timeInput = new wxSpinCtrl(parent, wxID_ANY, "25",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSP_ARROW_KEYS, 1, 120);

         //初始化按钮，使用成员变量
     startBtn = new wxButton(parent, wxID_ANY, "开始");
     stopBtn = new wxButton(parent, wxID_ANY, "停止");



    //绑定时间输入事件
    timeInput->Bind(wxEVT_SPINCTRL, [=](wxCommandEvent& e){
                    int minutes =e.GetInt();
                    remainingSeconds = minutes * 60;//分钟转化为秒
        timeDisplay->SetLabel(wxString::Format("%02d:00", minutes));
    });



    //绑定开始按钮事件
    startBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        if(!isRunning){
                    timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);

        // 重新绑定事件
        timer->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
             remainingSeconds = timeInput->GetValue() * 60;
            isRunning = true;
            timer->Start(1000);//启动计时器，间隔1秒
            timeDisplay->SetLabel(wxString::Format("%02d:00", remainingSeconds / 60));
        }
    });




    //绑定停止按钮事件
    stopBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        timer->Stop();
        isRunning = false;
        remainingSeconds = 0;
        timeDisplay->SetLabel("00:00");
    });



    //布局控件


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

    // 初始化所有动画
    animMap["rain"] = new RainAnimation(animPanel);
    animMap["glacier"] = new GlacierAnimation(animPanel);
    animMap["fire"] = new FireAnimation(animPanel);
    /*animMap["wind"] = new WindAnimation(animPanel);*/

    // 初始隐藏所有动画
    for(auto& [key, anim] : animMap) {
        anim->Hide();
    }

}
// 计时器事件处理：每秒更新剩余时间
void MainFrame::OnTimerTick(wxTimerEvent&){
    if(remainingSeconds > 0){
        remainingSeconds--;

        //更新时间显示
        int mins = remainingSeconds / 60;
        int secs = remainingSeconds % 60;
        timeDisplay->SetLabel(wxString::Format("%02d:%02d", mins, secs));

        //强制刷新界面
        timeDisplay->Refresh();

    }else {if (isRunning&& remainingSeconds == 0){



        timer->Stop();
        timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
        wxMessageDialog dialog(nullptr, "休息一下吧(￣︶￣）", "提示", wxOK | wxICON_NONE);
         dialog.ShowModal();


        isRunning = false;
         remainingSeconds = -1;
         timeDisplay->SetLabel("00:00");}}


}

wxMenuBar* MainFrame::createMenu() {
    wxMenuBar* menuBar = new wxMenuBar();
    menuBar->SetBackgroundColour(ThemeManager::Get().GetCurrentTheme().cardBg);

    // 文件菜单
    wxMenu* fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, "退出(&Q)");



    // 用户菜单
    wxMenu* userMenu = new wxMenu;
    userMenu->Append(2001, "专注统计(&S)");

    menuBar->Append(fileMenu, "文件(&F)");

    menuBar->Append(userMenu, "用户(&U)");


    wxMenu* themeMenu = new wxMenu;


    themeMenu->Append(ID_THEME_LIGHT, "Nord Light");
themeMenu->Append(ID_THEME_DARK, "Nord Dark");

themeMenu->AppendSeparator();
themeMenu->Append(ID_THEME_CUSTOM, "自定义主题...");
 menuBar->Append(themeMenu, "主题(&T)");

// 绑定菜单事件
Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    ApplyNordLightTheme();
}, ID_THEME_LIGHT);

Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    ApplyNordDarkTheme();
}, ID_THEME_DARK);

Bind(wxEVT_MENU, [=](wxCommandEvent&) {
    wxDialog dlg(this, wxID_ANY, "自定义主题");
    wxPanel* panel = new wxPanel(&dlg);

    // 颜色选择控件
    wxColourPickerCtrl* bgPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().mainBg);
    wxColourPickerCtrl* textPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().textPrimary);

    // 布局
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "背景颜色:"), 0, wxALL, 5);
    sizer->Add(bgPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "文本颜色:"), 0, wxALL, 5);
    sizer->Add(textPicker, 0, wxEXPAND | wxALL, 5);

    // 确认按钮
    wxButton* btnOK = new wxButton(panel, wxID_OK, "确定");
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

    // 事件绑定
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
    Close(true); // 关闭主窗口
}, wxID_EXIT);

    return menuBar;


}
void MainFrame::SetColorRecursive(wxWindow* window, wxColour color) {
    if (!window) return;

    window->SetBackgroundColour(color);
    window->Refresh();

    wxWindowList children = window->GetChildren();
    for (wxWindow* child : children) {
        SetColorRecursive(child, color);  // 递归设置子窗口颜色
    }
}

// 实现颜色应用功能
// MainFrame.cpp
void MainFrame::ApplyColorToAll(wxColour color) {

    std::function<void(wxWindow*)> SetColorRecursive;

    SetColorRecursive = [&](wxWindow* win) {
        win->SetBackgroundColour(color);
        win->Refresh();

        // 递归处理子控件
        wxWindowList& children = win->GetChildren();
        for (wxWindow* child : children) {
            SetColorRecursive(child);
        }
    };

    // 从主窗口开始应用
    SetColorRecursive(this);

    // 特殊控件处理
    if(timeDisplay) {
        timeDisplay->SetBackgroundColour(color.ChangeLightness(95));
    }
    currentBgColor = color;
}

// 实现字体应用功能
void MainFrame::ApplyFontToDisplay(wxFont font) {
    timeDisplay->SetFont(font);
    currentTimeFont = font;
    Layout(); // 自动调整布局
}


void MainFrame::ApplyThemeToWindow(wxWindow* window, const ThemeConfig& theme) {
    if (!window) return;


     if(window == this) {
        window->SetBackgroundColour(theme.mainBg.ChangeLightness(105));
    } else {
        window->SetBackgroundColour(theme.cardBg);
    }

    window->SetForegroundColour(theme.textPrimary);

    // 特殊控件处理
    if(auto btn = dynamic_cast<wxButton*>(window)){
        btn->SetBackgroundColour(theme.frost[0]); // nord7
        btn->SetForegroundColour(theme.polar_night[0]);
    }

    if(auto st = dynamic_cast<wxStaticText*>(window)) {
        st->SetForegroundColour(theme.frost[3]); // nord10
    }
    // 递归处理子控件
    wxWindowList& children = window->GetChildren();
    for (wxWindow* child : children) {
        ApplyThemeToWindow(child, theme);
    }
}

// MainFrame.cpp（自定义主题对话框）
void MainFrame::OnCustomTheme(wxCommandEvent&) {
    wxDialog dlg(this, wxID_ANY, "自定义主题");
    wxPanel* panel = new wxPanel(&dlg);

    // 颜色选择控件
    wxColourPickerCtrl* bgColorPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().mainBg);
    wxColourPickerCtrl* textColorPicker = new wxColourPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().textPrimary);

    // 字体选择控件
    wxFontPickerCtrl* titleFontPicker = new wxFontPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().titleFont);
    wxFontPickerCtrl* timeFontPicker = new wxFontPickerCtrl(panel, wxID_ANY,
        ThemeManager::Get().GetCurrentTheme().timeFont);

    // 布局
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "背景颜色:"), 0, wxALL, 5);
    sizer->Add(bgColorPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "文本颜色:"), 0, wxALL, 5);
    sizer->Add(textColorPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "标题字体:"), 0, wxALL, 5);
    sizer->Add(titleFontPicker, 0, wxEXPAND | wxALL, 5);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "时间字体:"), 0, wxALL, 5);
    sizer->Add(timeFontPicker, 0, wxEXPAND | wxALL, 5);

    wxButton* btnOK = new wxButton(panel, wxID_OK, "确定");
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
// 主题变更事件处理
void MainFrame::OnThemeChanged() {
    const auto& theme = ThemeManager::Get().GetCurrentTheme();
    ApplyThemeToWindow(this, theme);
    Refresh(); // 强制重绘界面

     // 特殊控件更新
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
        "本周专注时长：%.1f小时\n本月累计：%.1f小时",
        12.5,  // 示例数据，可替换为实际统计逻辑
        48.3
    );
    wxMessageBox(stats, "专注统计", wxOK | wxICON_INFORMATION);
}

void MainFrame::ApplyNordLightTheme() {
    ThemeConfig theme;
    // 北极光配色
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
    // 极夜配色
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
    // 停止当前动画
    if(currentAnim) {
        currentAnim->Stop();
        currentAnim->Hide();
    }

    // 启动新动画
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

