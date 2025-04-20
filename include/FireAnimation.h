#ifndef FIREANIMATION_H
#define FIREANIMATION_H
#include <vector>
#include "AnimationBase.h"
#include <wx/wx.h>
#include <wx/timer.h>
#include<random>
class FireAnimation : public AnimationBase {
public:
    FireAnimation(wxWindow* parent);
     ~FireAnimation() override = default;
    void Start() override;
    void Stop() override;


private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    struct Flame {
        wxPoint base;
        int height;
        int phase;
    };

    wxTimer m_timer;
    std::vector<Flame> m_flames;
    std::mt19937 m_rng;
    bool m_active = false;
};


#endif // FIREANIMATION_H
