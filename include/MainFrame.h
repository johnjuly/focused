#pragma once
#include <wx/wx.h>
#include <vector>
#include <wx/timer.h>
#include <wx/spinctrl.h>
#include "StyleConfig.h"
#include "SoundButton.h"
#include "Rainanimation.h"

class MainFrame : public wxFrame {
public:
    MainFrame();
    enum {
    ID_THEME_LIGHT = 3001,
    ID_THEME_DARK,
    ID_THEME_CUSTOM
};

    // 事件处理
    void OnClose(wxCloseEvent& event);
    void OnTimerTick(wxTimerEvent& event);
      void ApplyThemeToWindow(wxWindow* window, const ThemeConfig& theme);
    void OnThemeChanged();
    // UI相关
    wxMenuBar* createMenu();
    void ApplyColorToAll(wxColour color);
    void ApplyFontToDisplay(wxFont font);
     void ApplyTheme(const ThemeConfig& theme);
    void OnCustomTheme(wxCommandEvent&);

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
    RainAnimation* rainAnim = nullptr;
    wxWindow* rightPanel; // 右侧面板
    // 初始化方法
    void InitUI();
    void CreateSoundPanel(wxWindow* parent);
    void CreateTimerPanel(wxWindow* parent);
    void SetColorRecursive(wxWindow* window, wxColour color);
};
