#include "GlacierAnimation.h"
#include <wx/dcbuffer.h>
#include <cmath>

GlacierAnimation::GlacierAnimation(wxWindow* parent)
    : AnimationBase(parent), m_timer(this), m_rng(std::random_device{}()) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_PAINT, &GlacierAnimation::OnPaint, this);
    Bind(wxEVT_TIMER, &GlacierAnimation::OnTimer, this);
}

void GlacierAnimation::Start() {
    m_active = true;
    m_flakes.clear();
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

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    for(const auto& flake : m_flakes) {
        // 手动实现旋转和平移
        DrawSnowflake(dc, flake.pos, flake.size, flake.angle);
    }
}

void GlacierAnimation::DrawSnowflake(wxDC& dc, wxPoint center, int size, float angle) {
    wxColour iceColor(200, 225, 255);// 冰蓝色
    dc.SetPen(wxPen(iceColor, 1));

    const int branches = 6;
    const float angleStep = 2 * M_PI / branches;// 分支间角度间隔

    for(int i = 0; i < branches; ++i) {
        float branchAngle = angleStep * i + angle;

        // 主分支
        wxPoint end(
            center.x + size * cos(branchAngle),
            center.y + size * sin(branchAngle)
        );
        dc.DrawLine(center, end);

        // 2个次级分支
        for(int j = 1; j <= 2; ++j) {
            float subAngle = branchAngle + angleStep/3 * j;
            wxPoint subEnd(
                end.x + size/2 * cos(subAngle),
                end.y + size/2 * sin(subAngle)
            );
            dc.DrawLine(end, subEnd);
        }
    }
}
void GlacierAnimation::OnTimer(wxTimerEvent&) {
    wxSize size = GetClientSize();

    // 随机数分布
    std::uniform_int_distribution<int> distX(0, size.GetWidth());
    std::uniform_real_distribution<float> distSpeed(1.0f, 3.0f);
    std::uniform_int_distribution<int> distSize(4, 12);
    std::uniform_real_distribution<float> distAngle(0, 2*M_PI);
    std::uniform_real_distribution<float> distRotation(-0.05f, 0.05f);

    // 生成新雪花
    if(m_active && m_flakes.size() < 80) {
        m_flakes.push_back({
            wxPoint(distX(m_rng), -20),
            distSpeed(m_rng),
            distSize(m_rng),
            distAngle(m_rng),  // 初始角度
            0.0f,              // 旋转速度
            distSize(m_rng)/2   // 分支复杂度
        });
    }

    // 更新状态
    for(auto& flake : m_flakes) {
        flake.pos.y += flake.speed;
        flake.pos.x += 0.5 * sin(flake.pos.y * 0.1f); // 横向漂移
        flake.angle += flake.rotation;
        flake.rotation += distRotation(m_rng);
    }

    // 移除超出边界的雪花
    m_flakes.erase(
        std::remove_if(m_flakes.begin(), m_flakes.end(),
            [&](const auto& f){ return f.pos.y > size.GetHeight() + 50; }),
        m_flakes.end()
    );

    Refresh();
}
