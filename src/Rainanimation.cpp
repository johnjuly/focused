#include "Rainanimation.h"
#include <wx/dcbuffer.h>

#include "RainAnimation.h"



RainAnimation::RainAnimation(wxWindow* parent)
    : wxWindow(parent, wxID_ANY),
      m_timer(this),
      m_rng(std::random_device{}()) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &RainAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &RainAnimation::OnTimer, this);
}

void RainAnimation::Start() {
    m_isActive = true;
    m_timer.Start(30); // 约33fps
}

void RainAnimation::Stop() {
    m_timer.Stop();
    m_isActive = false;
    Refresh();
}

void RainAnimation::UpdateDrops() {
    wxSize size = GetClientSize();

    // 移除超出屏幕的雨滴
    m_drops.erase(std::remove_if(m_drops.begin(), m_drops.end(),
        [&](const auto& drop) { return drop.y > size.y; }),
        m_drops.end());

    // 生成新雨滴
    std::uniform_int_distribution<int> distX(0, size.x);
    std::uniform_int_distribution<int> distSpeed(10, 20);
    if(m_isActive && m_drops.size() < 50) {
        m_drops.push_back({
            distX(m_rng),
            -10,
            distSpeed(m_rng)
        });
    }

    // 更新位置
    for(auto& drop : m_drops) {
        drop.y += drop.speed;
    }
}

void RainAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if(!m_isActive) return;

    wxPen pen(wxColour(100, 181, 246), 3); // 蓝色粗线条
    dc.SetPen(pen);

    for(const auto& drop : m_drops) {
        dc.DrawLine(drop.x, drop.y, drop.x, drop.y + 20);
    }
}

void RainAnimation::OnTimer(wxTimerEvent&) {
    UpdateDrops();
    Refresh();
}
