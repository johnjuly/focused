// FireAnimation.cpp
#include "FireAnimation.h"
#include <wx/dcbuffer.h>
#include <random>

FireAnimation::FireAnimation(wxWindow* parent)
    : AnimationBase(parent)
    , m_timer(this)
    , m_rng(std::mt19937(std::random_device{}()))
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &FireAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &FireAnimation::OnTimer, this);
}

void FireAnimation::Start() {
    m_active = true;
    m_flames.clear();
    m_timer.Start(100);
}

void FireAnimation::Stop() {
    m_timer.Stop();
    m_active = false;
    m_flames.clear();   // ������л���
    Refresh();
}

void FireAnimation::OnPaint(wxPaintEvent&) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    if (!m_active) return;

    // �������л�������
    for (const auto& f : m_flames) {
        // ������ɫ�� phase ��΢����
        int glow = 69 + (f.phase % 30);
        wxColour color(255, glow, 0);
        dc.SetBrush(wxBrush(color));
        dc.SetPen(*wxTRANSPARENT_PEN);

        wxPoint pts[3] = {
            { f.base.x - 10, f.base.y + f.height },
            { f.base.x,       f.base.y         },
            { f.base.x + 10, f.base.y + f.height }
        };
        dc.DrawPolygon(3, pts);
    }
}

void FireAnimation::OnTimer(wxTimerEvent&) {
    if (!m_active) return;

    wxSize size = GetClientSize();

    // �ֲ���x ���ꡢ����߶ȡ�������λ
    std::uniform_int_distribution<int> distX(0, size.GetWidth());
    std::uniform_int_distribution<int> distH(20, 50);
    std::uniform_int_distribution<int> distP(0, 29);

    // �������л������λ������
    for (auto& f : m_flames) {
        f.phase = (f.phase + 5) % 30;
        f.base.y -= 2;  // �̶��ٶ�����
    }

    // ����������㣬�����»���
    if (m_flames.size() < 8) {
        Flame newF;
        newF.base   = wxPoint(distX(m_rng), size.GetHeight() - 50);
        newF.height = distH(m_rng);
        newF.phase  = distP(m_rng);
        m_flames.push_back(newF);
    }

    // �Ƴ��Ѿ���ȫ�Ƴ������Ļ���
    m_flames.erase(
        std::remove_if(m_flames.begin(), m_flames.end(),
            [&](const Flame& f){
                return f.base.y + f.height < 0;
            }),
        m_flames.end()
    );

    Refresh();
}
