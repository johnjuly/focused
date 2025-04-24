#ifndef RAINANIMATION_H
#define RAINANIMATION_H

#include "AnimationBase.h"
#include <random>
#include <vector>

class RainAnimation : public AnimationBase {
public:
    RainAnimation(wxWindow* parent);
    void OnPaint(wxPaintEvent& event) override;
    void OnTimer(wxTimerEvent& event) override;
    void Start() override;
    void Stop() override;

private:
    void UpdateDrops();

    struct RainDrop {
        int x;
        int y;
        int speed;
    };

    std::vector<RainDrop> m_drops;
    std::mt19937 m_rng;
};

#endif // RAINANIMATION_H
