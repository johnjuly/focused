#pragma once
#include "AnimationBase.h"
#include <wx/dcbuffer.h>
#include <vector>
#include <random>

class OceanAnimation : public AnimationBase {
public:
    OceanAnimation(wxWindow* parent);
    virtual ~OceanAnimation();
    void OnPaint(wxPaintEvent& event) override;
    void OnTimer(wxTimerEvent& event) override;
    virtual void Start() override;
    virtual void Stop() override;

private:
    struct Wave {
        float baseHeight;
        float amplitude;
        float frequency;
        float speed;
        float offset;
        int red;
        int green;
        int blue;
        int alpha;
    };

    struct FoamParticle {
        float x;
        float y;
        float speedX;
        float size;
        float alpha;
        float offset;
    };

    void InitializeWaves();
    void InitializeBubbles();
    void UpdateWaves();
    void UpdateBubbles();
    void DrawWaves(wxDC& dc);
    void DrawFoam(wxDC& dc);
    void ResetFoamParticle(FoamParticle& foam);
    wxColour GetWaveColor(float alpha) const;

    wxTimer m_timer;
    int m_frameCount;
    bool m_isActive;
    std::mt19937 m_rng;
    std::vector<Wave> m_waves;
    std::vector<FoamParticle> m_foamParticles;
    wxColour m_waterColor;
    float m_phase;
}; 