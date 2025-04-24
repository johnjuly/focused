#include "WaveAnimation.h"
#include <wx/dcbuffer.h>
#include <cmath>

WaveAnimation::WaveAnimation(wxWindow* parent)
    : AnimationBase(parent), m_timer(this) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &WaveAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &WaveAnimation::OnTimer, this);
}

void WaveAnimation::Start() {
    m_active = true;
    m_phase = 0.0f;
    m_timer.Start(50);
}

void WaveAnimation::Stop() {
    m_timer.Stop();
    m_active = false;
    Refresh();
}

void WaveAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (!m_active) return;

    wxSize size = GetClientSize();
    int baseY = size.GetHeight() * 3 / 4;

    // 绘制波浪
    wxPoint points[100];
    for (int i = 0; i < 100; ++i) {
        float x = static_cast<float>(i) * size.GetWidth() / 100;
        float y = baseY + 20 * sin(x * 0.05f + m_phase);
        points[i] = wxPoint(static_cast<int>(x), static_cast<int>(y));
    }

    wxColour waveColor(70, 130, 180, 150);
    dc.SetBrush(wxBrush(waveColor));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.DrawPolygon(100, points);
}

void WaveAnimation::OnTimer(wxTimerEvent&) {
    m_phase += 0.1f;
    if (m_phase > 2 * M_PI) m_phase -= 2 * M_PI;
    Refresh();
}
