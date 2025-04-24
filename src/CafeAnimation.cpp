#include "CafeAnimation.h"
#include <wx/dcbuffer.h>

CafeAnimation::CafeAnimation(wxWindow* parent)
    : AnimationBase(parent), m_timer(this), m_rng(std::random_device{}()) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &CafeAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &CafeAnimation::OnTimer, this);
}

void CafeAnimation::Start() {
    m_active = true;
    m_particles.clear();
    m_timer.Start(50);
}

void CafeAnimation::Stop() {
    m_timer.Stop();
    m_active = false;
    Refresh();
}

void CafeAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (!m_active) return;

    for (const auto& p : m_particles) {
        wxColour color(240, 240, 220, static_cast<int>(p.opacity * 255));
        dc.SetBrush(wxBrush(color));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawCircle(p.position, static_cast<int>(p.size));
    }
}

void CafeAnimation::OnTimer(wxTimerEvent&) {
    wxSize size = GetClientSize();
    std::uniform_real_distribution<float> distX(0.3f, 0.7f);
    std::uniform_real_distribution<float> distSpeed(0.5f, 1.5f);
    std::uniform_real_distribution<float> distSize(3.0f, 8.0f);

    // 生成新粒子
    if (m_active && m_particles.size() < 15) {
        SteamParticle p;
        p.position = wxPoint(size.GetWidth() * distX(m_rng), size.GetHeight());
        p.opacity = 1.0f;
        p.size = distSize(m_rng);
        p.speed = distSpeed(m_rng);
        m_particles.push_back(p);
    }

    // 更新粒子状态
    for (auto& p : m_particles) {
        p.position.y -= p.speed * 3;
        p.opacity -= 0.02f;
        p.size += 0.1f;
    }

    // 移除消失的粒子
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const auto& p) { return p.opacity <= 0; }),
        m_particles.end()
    );

    Refresh();
}
