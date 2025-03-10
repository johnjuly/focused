#ifndef MAINFRAME_H
#define MAINFRAME_H



#include <wx/wx.h>
#include <vector>
#include<wx/timer.h>
#include<wx/spinctrl.h>


class SoundButton;


class MainFrame : public wxFrame {
public:
    MainFrame();
    void OnClose(wxCloseEvent& event);// 窗口关闭事件处理
    void OnTimerTick(wxTimerEvent& event);// 计时器事件处理

private:
    std::vector<SoundButton*> soundButtons;
    wxButton* startBtn;
    wxButton* stopBtn;

    wxTimer* timer; // 计时器声明
  int remainingSeconds = 0;
  bool isRunning = false;

    void InitUI();// 初始化界面

    void CreateSoundPanel(wxWindow* parent);// 创建声音面板
    void CreateTimerPanel(wxWindow* parent);// 创建计时面板
    wxStaticText*timeDisplay;

};

#endif // MAINFRAME_H
