#ifndef ANIMATIONBASE_H_INCLUDED
#define ANIMATIONBASE_H_INCLUDED

#include <wx/wx.h>

class AnimationBase : public wxWindow {
public:
    AnimationBase(wxWindow* parent) : wxWindow(parent, wxID_ANY) {}
    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual ~AnimationBase() {}

};
#endif // ANIMATIONBASE_H_INCLUDED
