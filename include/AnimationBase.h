#ifndef ANIMATIONBASE_H
#define ANIMATIONBASE_H

#include <wx/wx.h>
#include <wx/timer.h>
#include "StyleConfig.h"

class AnimationBase : public wxPanel {
public:
    AnimationBase(wxWindow* parent)
        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        m_isActive = false;
        frameCount = 0;
        
        // Bind events
        Bind(wxEVT_PAINT, &AnimationBase::OnPaint, this);
    }

    virtual ~AnimationBase() {}

    virtual void Start() = 0;
    virtual void Stop() = 0;

    virtual void UpdateTheme() {
        const auto& theme = ThemeManager::Get().GetCurrentTheme();
        SetBackgroundColour(theme.mainBg);
        Refresh();
    }

protected:
    wxTimer timer;
    bool m_isActive;
    int frameCount;

    virtual void OnPaint(wxPaintEvent& evt) = 0;
    virtual void OnTimer(wxTimerEvent& evt) = 0;
};

#endif // ANIMATIONBASE_H
