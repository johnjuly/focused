#pragma once
#include <wx/wx.h>
#include <vector>
#include <wx/timer.h>
#include <wx/spinctrl.h>
#include "StyleConfig.h"
#include "SoundButton.h"
#include "AnimationBase.h"
#include <unordered_map>
#include <chrono>

#include <wx/frame.h>          // wxFrame::CreateToolBar()
#include <wx/toolbar.h>        // wxToolBar
#include <wx/bitmap.h>         // wxBitmap

class MainFrame : public wxFrame {
public:
    MainFrame();
    enum {
        ID_THEME_LIGHT = wxID_HIGHEST + 1,
        ID_THEME_DARK,
        ID_THEME_CUSTOM,
        ID_THEME_MENU,
        ID_THEME_NORD_LIGHT,
        ID_THEME_NORD_DARK
    };

    // 事件处理
    void OnClose(wxCloseEvent& event);
    void OnTimerTick(wxTimerEvent& event);
    void OnPaint(wxPaintEvent& event);
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

    // Theme management
    void UpdateTheme();

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
    wxSpinCtrl* timeInput;

    wxToolBar* m_toolbar;

    struct FocusRecord {
        std::chrono::system_clock::time_point startTime;
        int durationSec;
    };
    std::vector<FocusRecord> focusRecords;
    wxString dataFilePath = "focus_data.csv";

    wxWindow* rightPanel;
    wxPanel* animPanel;

    AnimationBase* currentAnim = nullptr;
    std::unordered_map<std::string, AnimationBase*> animMap;

    // 初始化方法
    void InitUI();
    void CreateSoundPanel(wxWindow* parent);
    void CreateTimerPanel(wxWindow* parent);
    void CreateAnimationPanel();
    void SwitchAnimation(const std::string& animType);
    void SetColorRecursive(wxWindow* window, wxColour color);
    void OnShowStatistics(wxCommandEvent& event);

    void ApplyNordLightTheme();
    void ApplyNordDarkTheme();
    void ApplyCustomTheme();

    void SaveRecords();
    void LoadRecords();
    void RecordSession();
};
