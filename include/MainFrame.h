#pragma once
#include <wx/wx.h>
#include <vector>
#include <wx/timer.h>
#include <wx/spinctrl.h>
#include "StyleConfig.h"
#include "SoundButton.h"
#include "AnimationBase.h"
#include <unordered_map>

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



    ~MainFrame() {
    for(auto& [key, anim] : animMap) {
        delete anim;
    }
}


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


    wxWindow* rightPanel; // 右侧面板
    wxPanel* animPanel;

     AnimationBase* currentAnim = nullptr;
    std::unordered_map<std::string, AnimationBase*> animMap;

    // 初始化方法
    void InitUI();
    void CreateSoundPanel(wxWindow* parent);
    void CreateTimerPanel(wxWindow* parent);
    void CreateAnimationPanel();  // 新增动画面板初始化方法
   void SwitchAnimation(const std::string& animType); // 新增动画切换方法
    void SetColorRecursive(wxWindow* window, wxColour color);
    void OnShowStatistics(wxCommandEvent& event);

     void ApplyNordLightTheme();
    void ApplyNordDarkTheme();


};
