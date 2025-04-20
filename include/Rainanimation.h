#ifndef RAINANIMATION_H
#define RAINANIMATION_H
#include <wx/wx.h>
#include <wx/timer.h>
#include <vector>
#include <random>
#include "AnimationBase.h"
class RainAnimation : public AnimationBase  {
public:
    RainAnimation(wxWindow* parent);
    void Start()override;
    void Stop()override;


private:
    void UpdateDrops();
    void OnPaint(wxPaintEvent& evt);
    void OnTimer(wxTimerEvent& evt);

    struct RainDrop {
        int x;
        int y;
        int speed;
    };

    wxTimer m_timer;
    std::vector<RainDrop> m_drops;
    std::mt19937 m_rng;
    bool m_isActive = false;
};

#endif // RAINANIMATION_H
