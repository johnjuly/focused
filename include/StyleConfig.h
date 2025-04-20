#ifndef STYLECONFIG_H_INCLUDED
#define STYLECONFIG_H_INCLUDED

#include <wx/colour.h>
#include <wx/font.h>
#include <map>
#include <string>

// 主题配置结构体
struct ThemeConfig {
    wxColour mainBg;        // 主背景色
    wxColour cardBg;        // 卡片背景色
    wxColour primary;       // 主要颜色（按钮、高亮）
    wxColour textPrimary;   // 主要文本颜色
    wxColour textSecondary; // 次要文本颜色
    wxFont titleFont;       // 标题字体
    wxFont timeFont;        // 时间显示字体

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


    // 默认构造函数（亮色主题）
    ThemeConfig() :
        mainBg(245, 247, 250),
        cardBg(255, 255, 255),
        primary(100, 181, 246),
        textPrimary(60, 64, 67),
        textSecondary(150, 150, 150),
        titleFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD),
        timeFont(48, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) {}
};

// 主题管理器类（单例模式）
class ThemeManager {
public:
    static ThemeManager& Get() {
        static ThemeManager instance;
        return instance;
    }

    // 切换主题
    void SetTheme(const ThemeConfig& theme) {
        currentTheme_ = theme;
        NotifyThemeChanged();
    }

    // 获取当前主题
    const ThemeConfig& GetCurrentTheme() const {
        return currentTheme_;
    }

    // 注册主题变更回调
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
