// BubblePanel.h - 增加浮点运算支持
#ifndef BUBBLEPANEL_H
#define BUBBLEPANEL_H

#include <wx/wx.h>
#include <vector>
#include <random>
#include <wx/graphics.h>
#include <wx/geometry.h>

struct Bubble {
    wxPoint2DDouble pos;    // 使用双精度浮点坐标
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

    // 事件处理
    void OnPaint(wxPaintEvent& event);
    void OnAnimTimer(wxTimerEvent& event);
    void OnSize(wxSizeEvent& event);

    // 功能方法
    void GenerateBubbles();
    void RespawnBubble(Bubble& b);
};

#endif
