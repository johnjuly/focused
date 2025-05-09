#ifndef GLACIERANIMATION_H
#define GLACIERANIMATION_H

#include "AnimationBase.h"
#include <wx/timer.h>
#include <vector>
#include <random>
#include <wx/dcbuffer.h>

class GlacierAnimation : public AnimationBase {
public:
    GlacierAnimation(wxWindow* parent);
    void Start() override;
    void Stop() override;

private:
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    void DrawSnowflake(wxDC& dc, wxPoint center, int size, float angle);

    struct Snowflake {
        wxPoint pos;
        float speed;
        int size;
        float angle;
        float rotation;
        int branches;
    };

    wxTimer m_timer;
    std::vector<Snowflake> m_flakes;
    std::mt19937 m_rng;
    bool m_active = false;
};

#endif // GLACIERANIMATION_H
