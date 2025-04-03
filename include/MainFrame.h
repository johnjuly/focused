#pragma once
#include <wx/wx.h>
#include <vector>
#include <wx/timer.h>
#include <wx/spinctrl.h>
#include "StyleConfig.h"
#include "BubblePanel.h"
#include "SoundButton.h"

class MainFrame : public wxFrame {
public:
    MainFrame();

    // 事件处理
    void OnClose(wxCloseEvent& event);
    void OnTimerTick(wxTimerEvent& event);

    // UI相关
    wxMenuBar* createMenu();
    void ApplyColorToAll(wxColour color);
    void ApplyFontToDisplay(wxFont font);

private:
    // 成员变量
    std::vector<SoundButton*> soundButtons;
    wxButton *startBtn, *stopBtn;
    wxStaticText* timeDisplay;
    wxTimer* timer;
    int remainingSeconds = 0;
    bool isRunning = false;
    wxColour currentBgColor;
    wxFont currentTimeFont;

    // 初始化方法
    void InitUI();
    void CreateSoundPanel(wxWindow* parent);
    void CreateTimerPanel(wxWindow* parent);
    void SetColorRecursive(wxWindow* window, wxColour color);
};
