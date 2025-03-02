#ifndef SOUNDBUTTON_H
#define SOUNDBUTTON_H


#include <wx/tglbtn.h>
#include <SFML/Audio.hpp>
#include<string>

class SoundButton : public wxToggleButton {
public:
    SoundButton(wxWindow* parent,
                const wxString& label,
                const std::string& filePath);

    void ToggleSound();// «–ªª≤•∑≈/Õ£÷π
    void SetVolume(float volume);

private:
    bool LoadAudio(const std::string& path);// º”‘ÿ“Ù∆µŒƒº˛

    sf::SoundBuffer buffer;
    sf::Sound sound;// SFML“Ù∆µ≤•∑≈∆˜
    bool isLoaded = false;
};

#endif // SOUNDBUTTON_H
