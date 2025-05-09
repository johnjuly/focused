// FireAnimation.cpp
#include "FireAnimation.h"
#include <wx/dcbuffer.h>
#include <random>
#include <algorithm>

FireAnimation::FireAnimation(wxWindow* parent)
    : AnimationBase(parent), frameCount(0) {
    InitializeParticles();

    Bind(wxEVT_PAINT, &FireAnimation::OnPaint, this);
    timer.Bind(wxEVT_TIMER, &FireAnimation::OnTimer, this);
}

void FireAnimation::InitializeParticles() {
    wxSize size = GetClientSize();
    std::random_device rd;
    std::mt19937 gen(rd());// 随机数分布
    std::uniform_int_distribution<> disX(0, size.GetWidth());
    std::uniform_int_distribution<> disLife(30, 60);// 生命周期30-60帧
    std::uniform_int_distribution<> disVelX(-2, 2);// 横向速度范围
    std::uniform_int_distribution<> disVelY(-4, -1);// 纵向速度，负数为向上

    particles.clear();
    for (int i = 0; i < 100; i++) {
        FireParticle particle;
        particle.position = wxPoint(disX(gen), size.GetHeight() - 50);
        particle.velocity = wxPoint(disVelX(gen), disVelY(gen));
        particle.maxLife = disLife(gen);
        particle.life = particle.maxLife;// 剩余生命从最大值开始
        particle.color = GetFireColor(particle.life, particle.maxLife);
        particles.push_back(particle);
    }
}

wxColour FireAnimation::GetFireColor(int life, int maxLife) {
    float ratio = static_cast<float>(life) / maxLife;

    if (ratio > 0.8f) {
        // 最热的火焰 - 白色到黄色
        int intensity = static_cast<int>(255 * (1.0f - (ratio - 0.8f) / 0.2f));
        return wxColour(255, 255, intensity);
    } else if (ratio > 0.5f) {
        // 黄色到橙色
        int r = 255;
        int g = static_cast<int>(255 * (ratio - 0.5f) / 0.3f);
        return wxColour(r, g, 0);
    } else if (ratio > 0.2f) {
        // 橙色到红色
        int r = 255;
        int g = static_cast<int>(128 * (ratio - 0.2f) / 0.3f);
        return wxColour(r, g, 0);
    } else {
        // 红色到深红色
        int r = 255;
        int g = static_cast<int>(64 * ratio / 0.2f);
        return wxColour(r, g, 0);
    }
}

void FireAnimation::UpdateParticles() {
    wxSize size = GetClientSize();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disVelX(-2, 2);
    std::uniform_int_distribution<> disVelY(-3, -1);
    std::uniform_int_distribution<> disLife(30, 60);

    // 更新现有粒子
    for (auto& particle : particles) {
        particle.life--;
        if (particle.life <= 0) {
            // 重置粒子
            particle.position = wxPoint(rand() % size.GetWidth(), size.GetHeight() - 50);
            particle.velocity = wxPoint(disVelX(gen), disVelY(gen));
            particle.maxLife = disLife(gen);
            particle.life = particle.maxLife;
        } else {
            // 更新位置
            particle.position += particle.velocity;
            // 添加一些随机性
            particle.velocity.x += (rand() % 3) - 1;
            particle.velocity.y += (rand() % 2) - 1;
            // 限制速度
            particle.velocity.x = std::clamp(particle.velocity.x, -3, 3);
            particle.velocity.y = std::clamp(particle.velocity.y, -3, -1);
        }
        particle.color = GetFireColor(particle.life, particle.maxLife);
    }
}

void FireAnimation::DrawParticle(wxDC& dc, const FireParticle& particle) {
    int size = 3 + (particle.life * 2 / particle.maxLife);
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(particle.color));
    dc.DrawCircle(particle.position, size);
}

void FireAnimation::OnPaint(wxPaintEvent& event) {
    wxBufferedPaintDC dc(this);
    dc.Clear();

    // 绘制背景

    const auto& theme = ThemeManager::Get().GetCurrentTheme();
    dc.SetBackground(wxBrush(theme.mainBg));

    // 绘制粒子
    for (const auto& particle : particles) {
        DrawParticle(dc, particle);
    }
}

void FireAnimation::OnTimer(wxTimerEvent& event) {
    frameCount++;
    UpdateParticles();
    Refresh();
}

void FireAnimation::Start() {
    timer.Start(50);
}

void FireAnimation::Stop() {
    timer.Stop();
}
