#include "MainFrame.h"


#include "SoundButton.h"

#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/spinctrl.h>

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

    // �Ҳ��ʱ���
    wxPanel* timerPanel = new wxPanel(splitter);
    CreateTimerPanel(timerPanel);

    splitter->SplitVertically(soundPanel, timerPanel, 300);
    splitter->SetMinimumPaneSize(200);


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
            btn->SetBackgroundColour(
                e.IsChecked() ? wxColour(220, 240, 220) : *wxWHITE
            );
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

    // ��ʼ��ʱ����ʾ�ؼ�


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
     sizer->Add(timeDisplay, 1, wxEXPAND|wxALL, 20);
    sizer->Add(timeInput, 0, wxEXPAND|wxALL, 10);
    sizer->Add(startBtn, 0, wxEXPAND|wxALL, 5);
    sizer->Add(stopBtn, 0, wxEXPAND|wxALL, 5);
    parent->SetSizer(sizer);
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
