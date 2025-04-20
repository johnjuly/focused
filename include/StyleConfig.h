#ifndef STYLECONFIG_H_INCLUDED
#define STYLECONFIG_H_INCLUDED

#include <wx/colour.h>
#include <wx/font.h>
#include <map>
#include <string>

// �������ýṹ��
struct ThemeConfig {
    wxColour mainBg;        // ������ɫ
    wxColour cardBg;        // ��Ƭ����ɫ
    wxColour primary;       // ��Ҫ��ɫ����ť��������
    wxColour textPrimary;   // ��Ҫ�ı���ɫ
    wxColour textSecondary; // ��Ҫ�ı���ɫ
    wxFont titleFont;       // ��������
    wxFont timeFont;        // ʱ����ʾ����

     wxColour polar_night[4] = {
        wxColour(46, 52, 64),   // nord0
        wxColour(59, 66, 82),   // nord1
        wxColour(67, 76, 94),   // nord2
        wxColour(76, 86, 106)   // nord3
    };

     wxColour snow_storm[3] = {
        wxColour(216, 222, 233), // nord4
        wxColour(229, 233, 240), // nord5
        wxColour(236, 239, 244)  // nord6
    };

     wxColour frost[4] = {
        wxColour(143, 188, 187), // nord7
        wxColour(136, 192, 208), // nord8
        wxColour(129, 161, 193), // nord9
        wxColour(94, 129, 172)   // nord10
    };


    // Ĭ�Ϲ��캯������ɫ���⣩
    ThemeConfig() :
        mainBg(245, 247, 250),
        cardBg(255, 255, 255),
        primary(100, 181, 246),
        textPrimary(60, 64, 67),
        textSecondary(150, 150, 150),
        titleFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
        timeFont(48, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) {}
};

// ����������ࣨ����ģʽ��
class ThemeManager {
public:
    static ThemeManager& Get() {
        static ThemeManager instance;
        return instance;
    }

    // �л�����
    void SetTheme(const ThemeConfig& theme) {
        currentTheme_ = theme;
        NotifyThemeChanged();
    }

    // ��ȡ��ǰ����
    const ThemeConfig& GetCurrentTheme() const {
        return currentTheme_;
    }

    // ע���������ص�
    void RegisterCallback(const std::function<void()>& callback) {
        callbacks_.push_back(callback);
    }

private:
    ThemeManager() = default;

    void NotifyThemeChanged() {
        for (const auto& cb : callbacks_) {
            if (cb) cb();
        }
    }

    ThemeConfig currentTheme_;
    std::vector<std::function<void()>> callbacks_;
};



#endif // STYLECONFIG_H_INCLUDED
