
#include <SFML/Audio.hpp>
#include "SoundButton.h"
#include <wx/log.h>

SoundButton::SoundButton(wxWindow* parent,
                       const wxString& label,
                       const std::string& filePath)
    : wxToggleButton(parent, wxID_ANY, label)
{
    if(!LoadAudio(filePath)){
        wxLogError("音频加载失败：%s", filePath);
        Disable();
    }
}

// 加载音频文件到缓冲区
bool SoundButton::LoadAudio(const std::string& path){
    if(buffer.loadFromFile(path)){
        sound.setBuffer(buffer);
        sound.setLoop(true);// 设置循环播放
        return isLoaded = true;
    }
    return false;
}


void SoundButton::ToggleSound(){
    if(!isLoaded) return;

    GetValue() ? sound.play() : sound.stop();
}


void SoundButton::SetVolume(float volume){
    if(isLoaded) sound.setVolume(volume * 120);
}
