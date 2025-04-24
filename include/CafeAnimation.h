#ifndef CAFEANIMATION_H
#define CAFEANIMATION_H

#include "AnimationBase.h"
#include <wx/timer.h>
#include <vector>
#include <random>

class CafeAnimation : public AnimationBase {
public:
    CafeAnimation(wxWindow* parent);
    void Start() override;
    void Stop() override;

private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    struct SteamParticle {
        wxPoint position;
        float opacity;
        float size;
        float speed;
    };

    wxTimer m_timer;
    std::vector<SteamParticle> m_particles;
    std::mt19937 m_rng;
    bool m_active = false;
};

#endif // CAFEANIMATION_H
