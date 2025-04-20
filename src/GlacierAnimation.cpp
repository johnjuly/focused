#include "GlacierAnimation.h"
#include <wx/dcbuffer.h>

GlacierAnimation::GlacierAnimation(wxWindow* parent)
    : AnimationBase(parent), m_timer(this), m_rng(std::random_device{}()) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &GlacierAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &GlacierAnimation::OnTimer, this);
}

void GlacierAnimation::Start() {
    m_active = true;
    m_particles.clear();
    m_timer.Start(50);
}

void GlacierAnimation::Stop() {
    m_timer.Stop();
    m_active = false;
    Refresh();
}

void GlacierAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if(!m_active) return;

    wxSize size = GetClientSize();
    std::uniform_int_distribution<int> dist(0, size.GetWidth());

    // ���Ʊ�������
    wxBrush iceBrush(wxColour(175, 238, 238));
    dc.SetBrush(iceBrush);

    for(const auto& p : m_particles) {
        dc.DrawCircle(p.pos, p.size);
    }
}

void GlacierAnimation::OnTimer(wxTimerEvent&) {
     wxSize size = GetClientSize();
    std::uniform_int_distribution<int> distX(0, size.GetWidth());
    std::uniform_real_distribution<float> distSpeed(1.0f, 3.0f);  // ����ֲ�
    std::uniform_real_distribution<float> distSize(2.0f, 6.0f);   // ����ֲ�
    std::uniform_int_distribution<int> distDir(-1, 1);

    if (m_active && m_particles.size() < 30) {
        m_particles.push_back({
            wxPoint(distX(m_rng), -10),
            distSpeed(m_rng),  // float ����
            distSize(m_rng),   // float ����
            distDir(m_rng)
        });
    }

    // ����λ��
    for(auto& p : m_particles) {
        p.pos.y += p.speed;
        p.pos.x += p.direction;
        if(p.pos.y > size.GetHeight()) p.pos.y = -10;
    }

    Refresh();
}
