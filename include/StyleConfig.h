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
