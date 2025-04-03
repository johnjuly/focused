// BubblePanel.cpp
#include "BubblePanel.h"
#include <wx/dcbuffer.h>  // 用于双缓冲
#include <wx/graphics.h>  // 修正 wxGraphicsBrush 相关错误
#include <wx/geometry.h>  // 修正 wxPoint2DDouble 相关错误

BubblePanel::BubblePanel(wxWindow* parent)
    : wxPanel(parent),
      animTimer(this),
      rng(std::random_device{}())  // 初始化随机数引擎
{
    // 启用双缓冲防止闪烁
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // 绑定事件
    Bind(wxEVT_PAINT, &BubblePanel::OnPaint, this);
    Bind(wxEVT_TIMER, &BubblePanel::OnAnimTimer, this);
    Bind(wxEVT_SIZE, &BubblePanel::OnSize, this);

    GenerateBubbles();
}

void BubblePanel::GenerateBubbles() {
    bubbles.clear();
    wxSize size = GetClientSize();

    std::uniform_int_distribution<int>
        xDist(0, size.x),
        yDist(0, size.y),
        speedDist(-3, 3),
        radDist(15, 30);

    for(int i = 0; i < 5; ++i) {
        bubbles.push_back({
            wxPoint2DDouble(xDist(rng), yDist(rng)),
            wxPoint2DDouble(speedDist(rng), speedDist(rng)),
            static_cast<double>(radDist(rng))
        });
    }
}

void BubblePanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
    if (!gc) return;

    wxColour startColor(100, 181, 246, 180);
    wxColour endColor(255, 255, 255, 50);

    for(const auto& b : bubbles) {
        wxGraphicsBrush brush = gc->CreateRadialGradientBrush(
            b.pos.m_x, b.pos.m_y,
            b.pos.m_x, b.pos.m_y, b.radius * 1.5,
            startColor, endColor);

        gc->SetBrush(brush);
        gc->DrawEllipse(b.pos.m_x - b.radius,
                        b.pos.m_y - b.radius,
                        b.radius * 2,
                        b.radius * 2);
    }
    delete gc;
}

void BubblePanel::OnAnimTimer(wxTimerEvent&) {
    wxSize size = GetClientSize();
    std::uniform_real_distribution<float> accDist(-0.3f, 0.3f);

    for(auto& b : bubbles) {
        b.speed.m_x += accDist(rng);
        b.speed.m_y += accDist(rng);

        b.speed.m_x *= 0.98;
        b.speed.m_y *= 0.98;

        b.pos.m_x += b.speed.m_x;
        b.pos.m_y += b.speed.m_y;

        if(b.pos.m_x - b.radius < 0) {
            b.pos.m_x = b.radius;
            b.speed.m_x *= -0.8f;
        }
        else if(b.pos.m_x + b.radius > size.x) {
            b.pos.m_x = size.x - b.radius;
            b.speed.m_x *= -0.8f;
        }

        if(b.pos.m_y - b.radius < 0) {
            b.pos.m_y = b.radius;
            b.speed.m_y *= -0.8f;
        }
        else if(b.pos.m_y + b.radius > size.y) {
            b.pos.m_y = size.y - b.radius;
            b.speed.m_y *= -0.8f;
        }
    }
    Refresh();
}

void BubblePanel::OnSize(wxSizeEvent& event) {
    GenerateBubbles();
    Refresh();
    event.Skip();
}

void BubblePanel::StartAnimation() {
    animTimer.Start(30);
}

void BubblePanel::StopAnimation() {
    animTimer.Stop();
}
