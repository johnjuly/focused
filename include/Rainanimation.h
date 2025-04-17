#ifndef RAINANIMATION_H
#define RAINANIMATION_H

#include <wx/wx.h>
#include <vector>
#include <random>
class RainAnimation : public  wxWindow {
public:
    RainAnimation(wxWindow* parent);
    void Start();
    void Stop();
    void UpdateDrops();

private:
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
