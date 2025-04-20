#ifndef GLACIERANIMATION_H
#define GLACIERANIMATION_H

#include<random>

#include "AnimationBase.h"

class GlacierAnimation : public AnimationBase {
public:
    GlacierAnimation(wxWindow* parent);
    void Start() override;
    void Stop() override;

private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    struct IceParticle {
        wxPoint pos;
        float speed;
        float size;
        int direction;
    };

    wxTimer m_timer;
    std::vector<IceParticle> m_particles;
    std::mt19937 m_rng;
    bool m_active = false;
};




#endif // GLACIERANIMATION_H
