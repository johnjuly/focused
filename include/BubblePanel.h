// BubblePanel.h - ���Ӹ�������֧��
#ifndef BUBBLEPANEL_H
#define BUBBLEPANEL_H

#include <wx/wx.h>
#include <vector>
#include <random>
#include <wx/graphics.h>
#include <wx/geometry.h>

struct Bubble {
    wxPoint2DDouble pos;    // ʹ��˫���ȸ�������
    wxPoint2DDouble speed;
    double radius;
};

class BubblePanel : public wxPanel {
public:
    BubblePanel(wxWindow* parent);
    void StartAnimation();
    void StopAnimation();

private:
    std::vector<Bubble> bubbles;
    wxTimer animTimer;
    std::mt19937 rng;

    // �¼�����
    void OnPaint(wxPaintEvent& event);
    void OnAnimTimer(wxTimerEvent& event);
    void OnSize(wxSizeEvent& event);

    // ���ܷ���
    void GenerateBubbles();
    void RespawnBubble(Bubble& b);
};

#endif
