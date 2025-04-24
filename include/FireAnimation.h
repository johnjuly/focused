#pragma once
#include "AnimationBase.h"
#include <wx/dcbuffer.h>
#include <vector>

class FireAnimation : public AnimationBase {
public:
    FireAnimation(wxWindow* parent);
    void OnPaint(wxPaintEvent& event) override;
    void OnTimer(wxTimerEvent& event) override;
    void Start() override;
    void Stop() override;

private:
    wxTimer timer;
    int frameCount;
    struct FireParticle {
        wxPoint position;
        wxPoint velocity;
        int life;
        int maxLife;
        wxColour color;
    };
    std::vector<FireParticle> particles;
    void InitializeParticles();
    void UpdateParticles();
    void DrawParticle(wxDC& dc, const FireParticle& particle);
    wxColour GetFireColor(int life, int maxLife);
};
