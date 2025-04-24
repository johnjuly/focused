#ifndef SOUNDBUTTON_H
#define SOUNDBUTTON_H


#include <wx/tglbtn.h>
#include <SFML/Audio.hpp>
#include<string>
#include "StyleConfig.h"

class SoundButton : public wxToggleButton {
public:
    SoundButton(wxWindow* parent,
                const wxString& label,
                const std::string& filePath);

    void ToggleSound();// �л�����/ֹͣ
    void SetVolume(float volume);
    void UpdateTheme();

private:
    bool LoadAudio(const std::string& path);// Ƶļ

    sf::SoundBuffer buffer;
    sf::Sound sound;// SFML��Ƶ������
    bool isLoaded = false;
};

#endif // SOUNDBUTTON_H
