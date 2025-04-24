#ifndef WINDANIMATION_H
#define WINDANIMATION_H

#include "AnimationBase.h"
#include <wx/timer.h>
#include <vector>
#include <random>

class WindAnimation : public AnimationBase {
public:
    WindAnimation(wxWindow* parent);
    void Start() override;
    void Stop() override;

private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    struct WindParticle {
        wxPoint start;
        wxPoint end;
        float progress;
        float speed;
    };

    wxTimer m_timer;
    std::vector<WindParticle> m_particles;
    std::mt19937 m_rng;
    bool m_active = false;
};

#endif // WINDANIMATION_H
