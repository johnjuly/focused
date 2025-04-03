#ifndef STYLECONFIG_H_INCLUDED
#define STYLECONFIG_H_INCLUDED

namespace Style {
    // 现代配色方案
    const wxColour MAIN_BG(245, 247, 250);   // 主背景色
    const wxColour CARD_BG(255, 255, 255);   // 卡片背景色
    const wxColour PRIMARY(100, 181, 246);    // 主色调
    const wxColour TEXT_PRIMARY(60, 64, 67); // 主要文字颜色

    // 默认字体配置
    const wxFont TITLE_FONT(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    const wxFont TIME_FONT(48, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
}

#endif // STYLECONFIG_H_INCLUDED
