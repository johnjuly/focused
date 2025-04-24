#include "RainAnimation.h"
#include <wx/dcbuffer.h>

RainAnimation::RainAnimation(wxWindow* parent)
    : AnimationBase(parent)
    , m_rng(std::random_device{}())
{
    Bind(wxEVT_PAINT, &RainAnimation::OnPaint, this);
    timer.Bind(wxEVT_TIMER, &RainAnimation::OnTimer, this);
}

void RainAnimation::Start() {
    m_isActive = true;
    timer.Start(30);
}

void RainAnimation::Stop() {
    timer.Stop();
    m_isActive = false;
    Refresh();
}

void RainAnimation::UpdateDrops() {
    wxSize size = GetClientSize();
    
    m_drops.erase(std::remove_if(m_drops.begin(), m_drops.end(),
        [&](const auto& drop) { return drop.y > size.y; }),
        m_drops.end());

    std::uniform_int_distribution<int> distX(0, size.x);
    std::uniform_int_distribution<int> distSpeed(10, 20);
    
    if(m_isActive && m_drops.size() < 50) {
        m_drops.push_back({
            distX(m_rng),
            -10,
            distSpeed(m_rng)
        });
    }

    for(auto& drop : m_drops) {
        drop.y += drop.speed;
    }
}

void RainAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    
    // Draw background using theme color
    const auto& theme = ThemeManager::Get().GetCurrentTheme();
    dc.SetBackground(wxBrush(theme.mainBg));
    dc.Clear();

    if(!m_isActive) return;

    // Draw rain drops with theme-appropriate color
    wxPen pen(theme.frost[2], 2); // Use frost[2] color for rain drops
    dc.SetPen(pen);

    for(const auto& drop : m_drops) {
        dc.DrawLine(drop.x, drop.y, drop.x, drop.y + 15);
    }
}

void RainAnimation::OnTimer(wxTimerEvent&) {
    UpdateDrops();
    Refresh();
}
