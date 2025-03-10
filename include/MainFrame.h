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
    void OnClose(wxCloseEvent& event);// ���ڹر��¼�����
    void OnTimerTick(wxTimerEvent& event);// ��ʱ���¼�����

private:
    std::vector<SoundButton*> soundButtons;
    wxButton* startBtn;
    wxButton* stopBtn;

    wxTimer* timer; // ��ʱ������
  int remainingSeconds = 0;
  bool isRunning = false;

    void InitUI();// ��ʼ������

    void CreateSoundPanel(wxWindow* parent);// �����������
    void CreateTimerPanel(wxWindow* parent);// ������ʱ���
    wxStaticText*timeDisplay;

};

#endif // MAINFRAME_H
