#include "OceanAnimation.h"
#include <random>
#include <algorithm>

OceanAnimation::OceanAnimation(wxWindow* parent)
    : AnimationBase(parent)
    , m_timer(this)
    , m_frameCount(0)
    , m_isActive(false)
    , m_waterColor(0, 119, 190)  // Deep blue
{
    // Initialize random number generator
    std::random_device rd;
    m_rng.seed(rd());

    // Initialize waves and bubbles
    InitializeWaves();
    InitializeBubbles();

    // Bind timer event
    m_timer.Bind(wxEVT_TIMER, &OceanAnimation::OnTimer, this);
}

OceanAnimation::~OceanAnimation()
{
    if (m_isActive) {
        Stop();
    }
}

void OceanAnimation::Start()
{
    if (!m_isActive) {
        m_timer.Start(50); // 20 FPS
        m_isActive = true;
    }
}

void OceanAnimation::Stop()
{
    if (m_isActive) {
        m_timer.Stop();
        m_isActive = false;
    }
}

void OceanAnimation::OnTimer(wxTimerEvent& event)
{
    m_frameCount++;
    UpdateWaves();
    UpdateBubbles();
    Refresh();
}

void OceanAnimation::OnPaint(wxPaintEvent& evt) {
    wxAutoBufferedPaintDC dc(this);
    wxSize size = GetSize();
    
    // 绘制海水渐变背景
    wxColour topColor(100, 150, 255);    // 浅蓝色
    wxColour bottomColor(0, 50, 150);    // 深蓝色
    dc.GradientFillLinear(wxRect(0, 0, size.GetWidth(), size.GetHeight()),
                         topColor, bottomColor, wxSOUTH);
    
    // 绘制波浪
    DrawWaves(dc);
    
    // 绘制泡沫
    DrawFoam(dc);
}

void OceanAnimation::InitializeWaves()
{
    std::uniform_real_distribution<float> ampDist(5.0f, 20.0f);
    std::uniform_real_distribution<float> freqDist(0.01f, 0.03f);
    std::uniform_real_distribution<float> speedDist(0.5f, 2.0f);
    std::uniform_real_distribution<float> alphaDist(0.3f, 0.8f);
    
    m_waves.clear();
    for (int i = 0; i < 8; ++i) {
        Wave wave;
        wave.baseHeight = 0.4f + i * 0.07f;
        wave.amplitude = ampDist(m_rng);
        wave.frequency = freqDist(m_rng);
        wave.speed = speedDist(m_rng);
        wave.offset = 0.0f;
        wave.red = 100 + i * 20;
        wave.green = 150 + i * 15;
        wave.blue = 255;
        wave.alpha = static_cast<int>(alphaDist(m_rng) * 255);
        m_waves.push_back(wave);
    }
}

void OceanAnimation::InitializeBubbles()
{
    wxSize size = GetSize();
    std::uniform_real_distribution<float> xDist(0, size.GetWidth());
    std::uniform_real_distribution<float> yDist(size.GetHeight() * 0.5f, size.GetHeight());
    std::uniform_real_distribution<float> speedDist(1.0f, 3.0f);
    std::uniform_real_distribution<float> sizeDist(2.0f, 6.0f);
    std::uniform_real_distribution<float> alphaDist(0.3f, 0.8f);
    
    m_foamParticles.clear();
    for (int i = 0; i < 30; ++i) {
        FoamParticle foam;
        foam.x = xDist(m_rng);
        foam.y = yDist(m_rng);
        foam.speedX = speedDist(m_rng);
        foam.size = sizeDist(m_rng);
        foam.alpha = alphaDist(m_rng);
        foam.offset = 0.0f;
        m_foamParticles.push_back(foam);
    }
}

void OceanAnimation::UpdateWaves() {
    m_phase += 0.05f;  // 控制波浪移动速度
    
    // 更新每层波浪
    for (auto& wave : m_waves) {
        wave.offset = m_phase * wave.speed;
    }
    
    // 更新泡沫粒子
    for (auto& foam : m_foamParticles) {
        foam.x += foam.speedX;
        foam.y += std::sin(m_phase + foam.offset) * 0.5f;
        foam.alpha = std::max(0.0f, foam.alpha - 0.01f);
        
        // 如果泡沫消失或移出屏幕，重置它
        if (foam.alpha <= 0 || foam.x > GetSize().GetWidth()) {
            ResetFoamParticle(foam);
        }
    }
    
    Refresh();
}

void OceanAnimation::UpdateBubbles()
{
    wxSize size = GetSize();
    std::uniform_real_distribution<float> xDist(0, size.GetWidth());
    std::uniform_real_distribution<float> yDist(size.GetHeight() * 0.5f, size.GetHeight());
    
    for (auto& foam : m_foamParticles) {
        foam.y -= foam.speedX;
        foam.x += std::sin(m_frameCount * 0.1f + foam.y * 0.01f) * 0.5f;
        
        // Reset foam if it goes too high
        if (foam.y < size.GetHeight() * 0.4f) {
            foam.x = xDist(m_rng);
            foam.y = yDist(m_rng);
        }
    }
}

void OceanAnimation::DrawWaves(wxDC& dc) {
    wxSize size = GetSize();
    const int steps = 200;  // 波浪的精细度
    const float dx = static_cast<float>(size.GetWidth()) / steps;
    
    for (const auto& wave : m_waves) {
        wxPoint points[steps + 1];
        for (int i = 0; i <= steps; ++i) {
            float x = i * dx;
            float y = size.GetHeight() * wave.baseHeight +
                     std::sin(x * wave.frequency + wave.offset) * wave.amplitude;
            points[i] = wxPoint(x, y);
        }
        
        // 创建波浪渐变色
        wxColour waveColor(wave.red, wave.green, wave.blue, wave.alpha);
        dc.SetPen(wxPen(waveColor, 2));
        dc.SetBrush(wxBrush(waveColor));
        
        // 绘制封闭的波浪形状
        wxPoint* fillPoints = new wxPoint[steps + 3];
        memcpy(fillPoints, points, (steps + 1) * sizeof(wxPoint));
        fillPoints[steps + 1] = wxPoint(size.GetWidth(), size.GetHeight());
        fillPoints[steps + 2] = wxPoint(0, size.GetHeight());
        dc.DrawPolygon(steps + 3, fillPoints);
        delete[] fillPoints;
    }
}

void OceanAnimation::DrawFoam(wxDC& dc) {
    for (const auto& foam : m_foamParticles) {
        int alpha = static_cast<int>(foam.alpha * 255);
        wxColour foamColor(255, 255, 255, alpha);
        dc.SetPen(wxPen(foamColor));
        dc.SetBrush(wxBrush(foamColor));
        dc.DrawCircle(foam.x, foam.y, foam.size);
    }
}

void OceanAnimation::ResetFoamParticle(FoamParticle& foam) {
    wxSize size = GetSize();
    foam.x = -10;  // 从屏幕左侧开始
    foam.y = size.GetHeight() * 0.3f + (rand() % 100) / 100.0f * size.GetHeight() * 0.4f;
    foam.speedX = 1.0f + (rand() % 100) / 100.0f;
    foam.size = 2 + (rand() % 3);
    foam.alpha = 0.8f + (rand() % 20) / 100.0f;
    foam.offset = (rand() % 628) / 100.0f;  // 0 到 2π
}

wxColour OceanAnimation::GetWaveColor(float alpha) const
{
    return wxColour(m_waterColor.Red(), m_waterColor.Green(), m_waterColor.Blue(),
                   static_cast<unsigned char>(alpha * 255));
} 