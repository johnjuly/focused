#ifndef WAVEANIMATION_H
#define WAVEANIMATION_H

#include "AnimationBase.h"
#include <wx/timer.h>

class WaveAnimation : public AnimationBase {
public:
    WaveAnimation(wxWindow* parent);
    void Start() override;
    void Stop() override;

private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    wxTimer m_timer;
    float m_phase = 0.0f;
    bool m_active = false;
};

#endif // WAVEANIMATION_H
