#include "MainFrame.h"


#include "SoundButton.h"

#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>

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

    // 右侧计时面板
    wxPanel* timerPanel = new wxPanel(splitter);
    CreateTimerPanel(timerPanel);

    splitter->SplitVertically(soundPanel, timerPanel, 300);
    splitter->SetMinimumPaneSize(200);


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
            btn->SetBackgroundColour(
                e.IsChecked() ? wxColour(220, 240, 220) : *wxWHITE
            );
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

    // 初始化时间显示控件


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
     sizer->Add(timeDisplay, 1, wxEXPAND|wxALL, 20);
    sizer->Add(timeInput, 0, wxEXPAND|wxALL, 10);
    sizer->Add(startBtn, 0, wxEXPAND|wxALL, 5);
    sizer->Add(stopBtn, 0, wxEXPAND|wxALL, 5);
    parent->SetSizer(sizer);
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

wxMenuBar *MainFrame::createMenu()
{
    wxMenuBar *menuBar = new wxMenuBar(wxMB_DOCKABLE);
    menuBar->SetBackgroundColour(wxColor(174, 238, 240));



    wxMenu *toolsMenu = new wxMenu;
    menuBar->Append(toolsMenu, wxT("Tools"));

    wxMenu *setMenu = new wxMenu;
    menuBar->Append(setMenu, wxT("Settings"));
    wxMenu *adminMenu = new wxMenu;
    menuBar->Append(adminMenu, wxT("Admin"));
    wxMenu *helpMenu = new wxMenu;
    menuBar->Append(helpMenu, wxT("Help"));

    return menuBar;
}


void MainFrame::OnClose(wxCloseEvent& event){
    for(auto btn : soundButtons){
        if(btn->GetValue()) btn->ToggleSound();
    }
    event.Skip();
}
