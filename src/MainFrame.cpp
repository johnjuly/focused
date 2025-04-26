#include "MainFrame.h"
#include "RainAnimation.h"
#include "GlacierAnimation.h"
#include "FireAnimation.h"
#include "WindAnimation.h"
#include "OceanAnimation.h"
#include "CafeAnimation.h"

#include <wx/clrpicker.h> // 颜色选择器头文件
#include "SoundButton.h"
#include <wx/fontpicker.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>
#include <wx/colordlg.h>  // 颜色对话框
#include <wx/fontdlg.h>   // 字体对话框

#include <fstream>
#include <sstream>
#include <iomanip>



MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, wxString::FromUTF8("focused"), wxDefaultPosition, wxSize(800, 600)),
    timer(new wxTimer(this)), // 初始化计时器
    remainingSeconds(0),
    isRunning(false)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    this->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
    InitUI();
    LoadRecords();

    // 退出记录
    Bind(wxEVT_TOOL, [=](wxCommandEvent&) { Close(true); }, wxID_EXIT);

    // 统计记录
    Bind(wxEVT_TOOL, &MainFrame::OnShowStatistics, this, 2001);


}



void MainFrame::InitUI(){
    // 设置主窗口的sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    // 创建分割窗口
    wxSplitterWindow* splitter = new wxSplitterWindow(this);
    mainSizer->Add(splitter, 1, wxEXPAND);

    // 创建左侧声音面板
    wxScrolledWindow* soundPanel = new wxScrolledWindow(splitter);
    CreateSoundPanel(soundPanel);

    // 创建右侧面板
    rightPanel = new wxPanel(splitter);
    wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
    rightPanel->SetSizer(rightSizer);

    // 创建计时器面板
    wxPanel* timerPanel = new wxPanel(rightPanel);
    CreateTimerPanel(timerPanel);
    rightSizer->Add(timerPanel, 1, wxEXPAND | wxALL, 5);

    // 创建动画面板
    CreateAnimationPanel();
    rightSizer->Add(animPanel, 1, wxEXPAND | wxALL, 5);  // Give it a proportion of 1 to expand

    // 设置分割窗口
    splitter->SplitVertically(soundPanel, rightPanel, 300);
    splitter->SetMinimumPaneSize(200);

    // 创建工具栏
    m_toolbar = CreateToolBar(wxTB_HORIZONTAL | wxTB_TEXT);
    m_toolbar->SetToolBitmapSize(wxSize(16, 16));
    m_toolbar->SetThemeEnabled(false);

    // 添加工具栏按钮
    m_toolbar->AddTool(wxID_EXIT, wxString::FromUTF8("退出"), wxBitmap("exit.png", wxBITMAP_TYPE_PNG));
    m_toolbar->AddTool(2001, wxString::FromUTF8("统计"), wxBitmap("stats.png", wxBITMAP_TYPE_PNG));

    // 创建主题菜单
    wxMenu* themeMenu = new wxMenu;
    themeMenu->Append(ID_THEME_NORD_LIGHT, "Nord Light");
    themeMenu->Append(ID_THEME_NORD_DARK, "Nord Dark");
    themeMenu->Append(ID_THEME_CUSTOM, wxString::FromUTF8("自定义"));

    m_toolbar->AddTool(ID_THEME_MENU, wxString::FromUTF8("主题"), wxBitmap("theme.png", wxBITMAP_TYPE_PNG),
                      wxEmptyString, wxITEM_DROPDOWN);
    m_toolbar->SetDropdownMenu(ID_THEME_MENU, themeMenu);
    m_toolbar->Realize();

    // 绑定主题菜单事件
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ApplyNordLightTheme(); }, ID_THEME_NORD_LIGHT);
    Bind(wxEVT_MENU, [this](wxCommandEvent&) { ApplyNordDarkTheme(); }, ID_THEME_NORD_DARK);
    Bind(wxEVT_MENU, &MainFrame::OnCustomTheme, this, ID_THEME_CUSTOM);

    // 应用初始主题
    ApplyNordLightTheme();

    // Register for theme updates
    ThemeManager::Get().RegisterCallback([this]() {
        UpdateTheme();
    });
}

// 创建声音面板按钮
void MainFrame::CreateSoundPanel(wxWindow* parent){
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 声音
    wxSlider* volumeSlider = new wxSlider(parent, wxID_ANY, 100, 0, 100);
    volumeSlider->Bind(wxEVT_SLIDER, [=](wxCommandEvent& e){
        float volume = e.GetInt() / 100.0f;
        for(auto btn : soundButtons){
            btn->SetVolume(volume);
        }
    });

    // 声音按钮
    wxGridSizer* grid = new wxGridSizer(2, 5, 5);
    const std::vector<std::pair<wxString, std::string>> sounds = {
        {wxString::FromUTF8("雨声"), "rain.wav"},
        {wxString::FromUTF8("咖啡馆"), "cafe.wav"},
        {wxString::FromUTF8("冰川"), "icebergs.wav"},
        {wxString::FromUTF8("森林"), "wind-in-forest.wav"},
        {wxString::FromUTF8("海浪"), "waves.wav"},
        {wxString::FromUTF8("木火"), "fire.wav"}
    };

    for(const auto& [name, path] : sounds){
        SoundButton* btn = new SoundButton(parent, name, path);
        btn->SetMinSize(wxSize(140, 80));
        btn->Bind(wxEVT_TOGGLEBUTTON, [=](wxCommandEvent& e){
            btn->ToggleSound();

            if(e.IsChecked()) {
                std::string animType = "";
                if(name == wxString::FromUTF8("雨声")) animType = "rain";
                else if(name == wxString::FromUTF8("冰川")) animType = "glacier";
                else if(name == wxString::FromUTF8("木火")) animType = "fire";
                else if(name == wxString::FromUTF8("森林")) animType = "wind";
                else if(name == wxString::FromUTF8("海浪")) animType = "ocean";
                else if(name == wxString::FromUTF8("咖啡馆")) animType = "cafe";

                // 停止当前动画
                if(currentAnim) {
                    currentAnim->Stop();
                    currentAnim->Hide();
                }

                // 启动新动画
                if(!animType.empty()) {
                    auto it = animMap.find(animType);
                    if(it != animMap.end()) {
                        currentAnim = it->second;
                        currentAnim->SetSize(animPanel->GetClientSize());
                        currentAnim->Show();
                        currentAnim->Start();
                        animPanel->Layout();
                    }
                }
            } else {
                // 停止动画
                if(currentAnim) {
                    currentAnim->Stop();
                    currentAnim->Hide();
                    currentAnim = nullptr;
                }
            }

            // 刷新布局
            rightPanel->Layout();
        });
        soundButtons.push_back(btn);
        grid->Add(btn, 1, wxEXPAND);
    }

    mainSizer->Add(volumeSlider, 0, wxEXPAND|wxALL, 10);
    mainSizer->Add(grid, 1, wxEXPAND|wxALL, 10);
    parent->SetSizer(mainSizer);
    dynamic_cast<wxScrolledWindow*>(parent)->SetScrollRate(10, 10);
}

// 创建计时器面板
void MainFrame::CreateTimerPanel(wxWindow* parent){
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    timeDisplay = new wxStaticText(parent, wxID_ANY, "25:00",
                                wxDefaultPosition, wxDefaultSize,
                                wxALIGN_CENTER);

    timeDisplay->SetFont({48, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL, false, "Arial"});
    timeDisplay->SetForegroundColour({30, 144, 255});

    wxSpinCtrl* timeInput = new wxSpinCtrl(parent, wxID_ANY, "25",
                                        wxDefaultPosition, wxDefaultSize,
                                        wxSP_ARROW_KEYS, 1, 120);
  //初始化按钮，使用成员变量
     startBtn = new wxButton(parent, wxID_ANY, wxString::FromUTF8("开始"));
     stopBtn = new wxButton(parent, wxID_ANY, wxString::FromUTF8("停止"));
    //timeInput event
    timeInput->Bind(wxEVT_SPINCTRL, [=](wxCommandEvent& e){
                    int minutes =e.GetInt();
                    remainingSeconds = minutes * 60;//转换为秒
        timeDisplay->SetLabel(wxString::Format("%02d:00", minutes));
    });

    //startBtn event
    startBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        if(!isRunning){
                    timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);

        // 绑定事件
        timer->Bind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
             remainingSeconds = timeInput->GetValue() * 60;
            isRunning = true;
            timer->Start(1000);//计时器开始，每秒一次
            timeDisplay->SetLabel(wxString::Format("%02d:00", remainingSeconds / 60));
        }
    });

    //stopBtn event
    stopBtn->Bind(wxEVT_BUTTON, [=](wxCommandEvent&){
        timer->Stop();
        isRunning = false;

        int used = timeInput->GetValue() * 60 - remainingSeconds;
        if (used > 0) {
            FocusRecord rec;
            rec.startTime = std::chrono::system_clock::now() - std::chrono::seconds(used);
            rec.durationSec = used;
            focusRecords.push_back(rec);
            SaveRecords();
        }


        remainingSeconds = 0;
        timeDisplay->SetLabel("00:00");
    });



    sizer->AddStretchSpacer(1);
    sizer->Add(timeDisplay, 0, wxALIGN_CENTER|wxBOTTOM, 20);
    sizer->Add(timeInput, 0, wxALIGN_CENTER|wxBOTTOM, 10);
    sizer->Add(startBtn, 0, wxALIGN_CENTER|wxBOTTOM, 5);
    sizer->Add(stopBtn, 0, wxALIGN_CENTER);
    sizer->AddStretchSpacer(1);

    parent->SetSizer(sizer);


}


void MainFrame::CreateAnimationPanel() {
    // 创建动画面板
    animPanel = new wxPanel(rightPanel);




    // 创建动画面板的sizer
    wxBoxSizer* animSizer = new wxBoxSizer(wxVERTICAL);
    animPanel->SetSizer(animSizer);

    // 初始化动画
    animMap["rain"] = new RainAnimation(animPanel);
    animMap["glacier"] = new GlacierAnimation(animPanel);
    animMap["fire"] = new FireAnimation(animPanel);
    animMap["wind"] = new WindAnimation(animPanel);
    animMap["ocean"] = new OceanAnimation(animPanel);
    animMap["cafe"] = new CafeAnimation(animPanel);

    // 设置所有动画的大小和sizer
    for(auto& [key, anim] : animMap) {
        anim->Hide();
        anim->SetSize(animPanel->GetClientSize());
        animSizer->Add(anim, 1, wxEXPAND | wxALL, 0);
    }

    // 绑定大小变化事件
    animPanel->Bind(wxEVT_SIZE, [this](wxSizeEvent& evt) {
        wxSize size = evt.GetSize();
        for(auto& [key, anim] : animMap) {
            anim->SetSize(size);
        }
        if(currentAnim) {
            currentAnim->Refresh();
        }
        evt.Skip();
    });
}

// 计时器事件，每秒更新剩余时间
void MainFrame::OnTimerTick(wxTimerEvent&){
    if(remainingSeconds > 0){
        remainingSeconds--;

        //当前时间显示
        int mins = remainingSeconds / 60;
        int secs = remainingSeconds % 60;
        timeDisplay->SetLabel(wxString::Format("%02d:%02d", mins, secs));

        //强制刷新
        timeDisplay->Refresh();

    }else {if (isRunning&& remainingSeconds == 0){



        timer->Stop();
        timer->Unbind(wxEVT_TIMER, &MainFrame::OnTimerTick, this);
         wxMessageDialog dialog(nullptr, wxString::FromUTF8("休息一下吧(￣︶￣）"), wxString::FromUTF8("提示"), wxOK | wxICON_NONE);
         dialog.ShowModal();


        isRunning = false;
          RecordSession();
         remainingSeconds = -1;
         timeDisplay->SetLabel("00:00");}}


}


void MainFrame::SetColorRecursive(wxWindow* window, wxColour color) {
    if (!window) return;

    window->SetBackgroundColour(color);
    window->Refresh();

    wxWindowList children = window->GetChildren();
    for (wxWindow* child : children) {
        SetColorRecursive(child, color);  // 递归设置颜色
    }
}

// 实现颜色应用函数
// MainFrame.cpp
void MainFrame::ApplyColorToAll(wxColour color) {

    std::function<void(wxWindow*)> SetColorRecursive;

    SetColorRecursive = [&](wxWindow* win) {
        win->SetBackgroundColour(color);
        win->Refresh();

        // 递归遍历子部件
        wxWindowList& children = win->GetChildren();
        for (wxWindow* child : children) {
            SetColorRecursive(child);
        }
    };

    // 从当前部件开始应用
    SetColorRecursive(this);

    // 关键部件
    if(timeDisplay) {
        timeDisplay->SetBackgroundColour(color.ChangeLightness(95));
    }
    currentBgColor = color;
}

// 实现字体应用函数
void MainFrame::ApplyFontToDisplay(wxFont font) {
    timeDisplay->SetFont(font);
    currentTimeFont = font;
    Layout(); // 自动布局
}


void MainFrame::ApplyThemeToWindow(wxWindow* window, const ThemeConfig& theme) {
    if (!window) return;


     if(window == this) {
        window->SetBackgroundColour(theme.mainBg.ChangeLightness(105));
    } else {
        window->SetBackgroundColour(theme.cardBg);
    }

    window->SetForegroundColour(theme.textPrimary);

    // 关键部件
    if(auto btn = dynamic_cast<wxButton*>(window)){
        btn->SetBackgroundColour(theme.frost[0]); // nord7
        btn->SetForegroundColour(theme.polar_night[0]);
    }

    if(auto st = dynamic_cast<wxStaticText*>(window)) {
        st->SetForegroundColour(theme.frost[3]); // nord10
    }
    // 递归遍历子部件
    wxWindowList& children = window->GetChildren();
    for (wxWindow* child : children) {
        ApplyThemeToWindow(child, theme);
    }
}

// MainFrame.cpp自动应用主题窗口
void MainFrame::OnCustomTheme(wxCommandEvent&) {
    // 创建自定义主题对话框
    wxDialog dlg(this, wxID_ANY, wxString::FromUTF8("自定义主题"));
    wxPanel* panel = new wxPanel(&dlg);

    // 获取当前主题
    const auto& currentTheme = ThemeManager::Get().GetCurrentTheme();

    // 创建颜色选择器
    auto* bgPicker = new wxColourPickerCtrl(panel, wxID_ANY, currentTheme.mainBg,
        wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE | wxCLRP_SHOW_LABEL);
    auto* cardPicker = new wxColourPickerCtrl(panel, wxID_ANY, currentTheme.cardBg,
        wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE | wxCLRP_SHOW_LABEL);
    auto* primaryPicker = new wxColourPickerCtrl(panel, wxID_ANY, currentTheme.primary,
        wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE | wxCLRP_SHOW_LABEL);
    auto* textPicker = new wxColourPickerCtrl(panel, wxID_ANY, currentTheme.textPrimary,
        wxDefaultPosition, wxDefaultSize, wxCLRP_DEFAULT_STYLE | wxCLRP_SHOW_LABEL);

    // 创建布局
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // 添加颜色选择器
    auto addColorPicker = [&](const wxString& label, wxColourPickerCtrl* picker) {
        wxBoxSizer* row = new wxBoxSizer(wxHORIZONTAL);
        row->Add(new wxStaticText(panel, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        row->Add(picker, 1, wxEXPAND);
        sizer->Add(row, 0, wxEXPAND | wxALL, 5);
    };

    addColorPicker(wxString::FromUTF8("主背景颜色:"), bgPicker);
    addColorPicker(wxString::FromUTF8("卡片背景:"), cardPicker);
    addColorPicker(wxString::FromUTF8("主题色:"), primaryPicker);
    addColorPicker(wxString::FromUTF8("文本颜色:"), textPicker);

    // 添加按钮
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(panel, wxID_OK, wxString::FromUTF8("确定"));
    wxButton* cancelBtn = new wxButton(panel, wxID_CANCEL, wxString::FromUTF8("取消"));
    btnSizer->Add(okBtn, 0, wxRIGHT, 5);
    btnSizer->Add(cancelBtn, 0);
    sizer->Add(btnSizer, 0, wxALIGN_CENTER | wxALL, 10);

    // 设置面板布局
    panel->SetSizer(sizer);
    sizer->Fit(panel);

    // 设置对话框大小
    dlg.SetSize(400, -1);
    dlg.Center();

    // 显示对话框并处理结果
    if (dlg.ShowModal() == wxID_OK) {
        ThemeConfig newTheme = currentTheme;
        newTheme.mainBg = bgPicker->GetColour();
        newTheme.cardBg = cardPicker->GetColour();
        newTheme.primary = primaryPicker->GetColour();
        newTheme.textPrimary = textPicker->GetColour();

        // 应用新主题
        ThemeManager::Get().SetTheme(newTheme);
        UpdateTheme();
    }
}

// 事件
void MainFrame::OnThemeChanged() {
    const auto& theme = ThemeManager::Get().GetCurrentTheme();

    // 应用主题到所有窗口组件
    ApplyThemeToWindow(this, theme);

    // 特别处理工具栏
    if (m_toolbar) {
        m_toolbar->SetBackgroundColour(theme.mainBg);
        m_toolbar->Refresh();
    }

    // 特别处理右侧面板
    if (rightPanel) {
        rightPanel->SetBackgroundColour(theme.cardBg);
    }

    // 特别处理动画面板
    if (animPanel) {
        animPanel->SetBackgroundColour(theme.mainBg);
        for(auto& [key, anim] : animMap) {
            anim->UpdateTheme();
        }
    }

    // 刷新整个窗口
    Refresh();
    Update();
    Layout();
}

void MainFrame::UpdateTheme() {
    const auto& theme = ThemeManager::Get().GetCurrentTheme();

    // Apply theme to main window
    SetBackgroundColour(theme.mainBg);

    // Apply theme to toolbar
    if (m_toolbar) {
        m_toolbar->SetBackgroundColour(theme.mainBg);
        m_toolbar->Refresh();
    }

    // Apply theme to right panel
    if (rightPanel) {
        rightPanel->SetBackgroundColour(theme.cardBg);
        rightPanel->Refresh();
    }

    // Apply theme to timer display
    if (timeDisplay) {
        timeDisplay->SetBackgroundColour(theme.cardBg);
        timeDisplay->SetForegroundColour(theme.frost[3]);  // nord10
        timeDisplay->SetFont(theme.timeFont);
    }

    // Apply theme to buttons
    if (startBtn) {
        startBtn->SetBackgroundColour(theme.frost[0]);  // nord7
        startBtn->SetForegroundColour(theme.polar_night[0]);
        startBtn->SetFont(theme.titleFont);
    }
    if (stopBtn) {
        stopBtn->SetBackgroundColour(theme.frost[0]);  // nord7
        stopBtn->SetForegroundColour(theme.polar_night[0]);
        stopBtn->SetFont(theme.titleFont);
    }

    // Apply theme to sound buttons
    for (auto* btn : soundButtons) {
        if (btn) {
            btn->UpdateTheme();
        }
    }

    // Apply theme to animation panel and all animations
    if (animPanel) {
        animPanel->SetBackgroundColour(theme.mainBg);
        for (const auto& pair : animMap) {
            if (pair.second) {
                pair.second->UpdateTheme();
            }
        }
    }

    // Refresh everything
    Refresh();
    Update();
    Layout();
}

// 1) 读取 CSV 记录 focusRecords
void MainFrame::LoadRecords() {
    std::ifstream ifs(dataFilePath.mb_str());
    if (!ifs.is_open()) return;  // 文件不存在时直接返回

    focusRecords.clear();
    std::string line;
    // 读取标题行
    std::getline(ifs, line);

    while (std::getline(ifs, line)) {
        std::istringstream ss(line);
        std::string timeStr;
        int dur;
        if (std::getline(ss, timeStr, ',') && ss >> dur) {
            std::tm tm = {};
            std::istringstream ts(timeStr);
            ts >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
            if (!ts.fail()) {
                auto tt = std::mktime(&tm);
                FocusRecord rec;
                rec.startTime = std::chrono::system_clock::from_time_t(tt);
                rec.durationSec = dur;
                focusRecords.push_back(rec);
            }
        }
    }
}

// 2) 在当前目录下写入 CSV
void MainFrame::SaveRecords() {
    std::ofstream ofs(dataFilePath.mb_str(), std::ios::trunc);
    ofs << "start_time,duration\n";
    for (auto& r : focusRecords) {
        auto tt = std::chrono::system_clock::to_time_t(r.startTime);
        std::tm* tm = std::localtime(&tt);
        ofs << std::put_time(tm, "%Y-%m-%dT%H:%M:%S")
            << ',' << r.durationSec << "\n";
    }
}



// 3) 每个专注时间段记录，记录在 focusRecords
void MainFrame::RecordSession() {
    FocusRecord rec;
    // 开始时间 = 之前时间 - 使用时间
    rec.startTime = std::chrono::system_clock::now() - std::chrono::seconds(rec.durationSec =
                         (timer->IsRunning() ? (timeInput->GetValue()*60 - remainingSeconds) : remainingSeconds));
    focusRecords.push_back(rec);
    SaveRecords();
}



void MainFrame::OnShowStatistics(wxCommandEvent&) {
    using namespace std::chrono;
    auto now = system_clock::now();

    // 创建统计对话框
    wxDialog dlg(this, wxID_ANY, wxString::FromUTF8("专注统计"), wxDefaultPosition, wxSize(600, 400));
    wxPanel* panel = new wxPanel(&dlg);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // 计算统计数据
    std::vector<int> dailyHours(7, 0);  // 最近7天
    std::vector<int> monthlyHours(30, 0);  // 最近30天

    int64_t weekSec = 0, monthSec = 0;
    auto today_start = now - hours(now.time_since_epoch().count() % (24*3600));

    for (const auto& record : focusRecords) {
        auto secs = record.durationSec;
        auto days_ago = duration_cast<hours>(now - record.startTime).count() / 24;

        if (days_ago < 7) {
            dailyHours[days_ago] += secs / 3600;
            weekSec += secs;
        }
        if (days_ago < 30) {
            monthlyHours[days_ago] += secs / 3600;
            monthSec += secs;
        }
    }

    // 创建图表面板
    wxPanel* chartPanel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(550, 200));
    chartPanel->SetBackgroundColour(*wxWHITE);

    // 绘制图表
    chartPanel->Bind(wxEVT_PAINT, [chartPanel, dailyHours](wxPaintEvent& evt) {
        wxPaintDC dc(chartPanel);
        wxSize size = dc.GetSize();

        // 设置背景色
        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        // 设置坐标轴颜色
        dc.SetPen(*wxBLACK_PEN);

        // 绘制坐标轴
        int margin = 40;
        int chartWidth = size.GetWidth() - 2 * margin;
        int chartHeight = size.GetHeight() - 2 * margin;

        // 绘制X轴和Y轴
        dc.DrawLine(margin, size.GetHeight() - margin,
                   size.GetWidth() - margin, size.GetHeight() - margin);
        dc.DrawLine(margin, margin, margin, size.GetHeight() - margin);

        // 绘制柱状图
        int barWidth = chartWidth / 7;
        int maxHours = 24;

        for (int i = 0; i < 7; i++) {
            int x = margin + i * barWidth;
            int barHeight = (dailyHours[i] * chartHeight) / maxHours;
            int y = size.GetHeight() - margin - barHeight;

            // 设置柱状图颜色
            dc.SetBrush(*wxBLUE_BRUSH);
            dc.DrawRectangle(x, y, barWidth - 10, barHeight);

            // 绘制日期标签
            wxString day;
            switch(i) {
                case 0: day = "Mon"; break;
                case 1: day = "Tue"; break;
                case 2: day = "Wed"; break;
                case 3: day = "Thu"; break;
                case 4: day = "Fri"; break;
                case 5: day = "Sat"; break;
                case 6: day = "Sun"; break;
            }
            dc.DrawText(day, x + (barWidth - 10) / 2 - 15, size.GetHeight() - margin + 5);
        }

        // 绘制Y轴刻度
        for (int i = 0; i <= maxHours; i += 4) {
            int y = size.GetHeight() - margin - (i * chartHeight) / maxHours;
            dc.DrawLine(margin - 5, y, margin, y);
            dc.DrawText(wxString::Format("%d", i), margin - 35, y - 10);
        }
    });

    // 添加统计文本
    double weekH = weekSec / 3600.0, monthH = monthSec / 3600.0;
    wxString stats = wxString::Format(
        wxString::FromUTF8("本周专注时间: %.1f 小时\n本月专注时间: %.1f 小时"),
        weekH, monthH
    );
    wxStaticText* statsText = new wxStaticText(panel, wxID_ANY, stats);
    statsText->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // 布局
    mainSizer->Add(statsText, 0, wxALL | wxALIGN_CENTER, 20);
    mainSizer->Add(chartPanel, 1, wxEXPAND | wxALL, 10);

    wxButton* btnClose = new wxButton(panel, wxID_OK, wxString::FromUTF8("关闭"));
    mainSizer->Add(btnClose, 0, wxALIGN_CENTER | wxALL, 10);

    panel->SetSizer(mainSizer);
    dlg.ShowModal();
}

void MainFrame::ApplyNordLightTheme() {
    ThemeConfig theme;
    theme.mainBg = theme.snow_storm[2];  // nord6
    theme.cardBg = theme.snow_storm[1];  // nord5
    theme.primary = theme.frost[1];      // nord8
    theme.textPrimary = theme.polar_night[0]; // nord0
    theme.textSecondary = theme.polar_night[1]; // nord1

    ThemeManager::Get().SetTheme(theme);
    m_toolbar->SetBackgroundColour(theme.mainBg);
    m_toolbar->Refresh();
}

void MainFrame::ApplyNordDarkTheme() {
    ThemeConfig theme;
    theme.mainBg = theme.polar_night[0];  // nord0
    theme.cardBg = theme.polar_night[1];  // nord1
    theme.primary = theme.frost[1];       // nord8
    theme.textPrimary = theme.snow_storm[2]; // nord6
    theme.textSecondary = theme.snow_storm[1]; // nord5

    ThemeManager::Get().SetTheme(theme);
    m_toolbar->SetBackgroundColour(theme.mainBg);
    m_toolbar->Refresh();
}

void MainFrame::SwitchAnimation(const std::string& animType) {
    // 停止当前动画
    if(currentAnim) {
        currentAnim->Stop();
        currentAnim->Hide();
        currentAnim = nullptr;
    }

    // 启动新动画
    auto it = animMap.find(animType);
    if(it != animMap.end()) {
        currentAnim = it->second;
        currentAnim->SetSize(animPanel->GetClientSize());
        currentAnim->Show();
        currentAnim->Start();

        // 更新布局
        animPanel->Layout();
        rightPanel->Layout();
    }
}

void MainFrame::OnClose(wxCloseEvent& event){
    for(auto btn : soundButtons){
        if(btn->GetValue()) btn->ToggleSound();
    }
    event.Skip();
}

void MainFrame::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    event.Skip();
}

