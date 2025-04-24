#include "WindAnimation.h"
#include <wx/dcbuffer.h>
#include <cmath>

WindAnimation::WindAnimation(wxWindow* parent)
    : AnimationBase(parent), m_timer(this), m_rng(std::random_device{}()) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &WindAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &WindAnimation::OnTimer, this);
}

void WindAnimation::Start() {
    m_active = true;
    m_particles.clear();
    m_timer.Start(30);
}

void WindAnimation::Stop() {
    m_timer.Stop();
    m_active = false;
    Refresh();
}

void WindAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (!m_active) return;

    wxColour windColor(150, 200, 255, 100);
    dc.SetPen(wxPen(windColor, 2));

    for (const auto& p : m_particles) {
        int alpha = static_cast<int>(255 * (1.0f - p.progress));
        wxColour lineColor(150, 200, 255, alpha);
        dc.SetPen(wxPen(lineColor, 1));

        wxPoint current(
            p.start.x + (p.end.x - p.start.x) * p.progress,
            p.start.y + (p.end.y - p.start.y) * p.progress
        );
        dc.DrawLine(p.start, current);
    }
}

void WindAnimation::OnTimer(wxTimerEvent&) {
    wxSize size = GetClientSize();
    std::uniform_int_distribution<int> distX(0, size.GetWidth());
    std::uniform_int_distribution<int> distY(0, size.GetHeight());
    std::uniform_real_distribution<float> distSpeed(0.02f, 0.08f);

    // 生成新粒子
    if (m_active && m_particles.size() < 40) {
        WindParticle p;
        p.start = wxPoint(distX(m_rng), distY(m_rng));
        p.end = wxPoint(p.start.x + 50 + distX(m_rng)%100, p.start.y - 50);
        p.progress = 0.0f;
        p.speed = distSpeed(m_rng);
        m_particles.push_back(p);
    }

    // 更新粒子
    for (auto& p : m_particles) {
        p.progress += p.speed;
    }

    // 移除完成的粒子
    m_particles.erase(
        std::remove_if(m_particles.begin(), m_particles.end(),
            [](const auto& p) { return p.progress >= 1.0f; }),
        m_particles.end()
    );

    Refresh();
}
