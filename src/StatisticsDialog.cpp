#include "StatisticsDialog.h"
#include <wx/dcbuffer.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

StatisticsDialog::StatisticsDialog(wxWindow* parent, 
    const std::vector<std::pair<std::chrono::system_clock::time_point, int>>& records)
    : wxDialog(parent, wxID_ANY, "专注时间统计", wxDefaultPosition, wxSize(800, 600)),
      focusRecords(records)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    
    // 创建主布局
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // 创建图表类型选择器
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    chartTypeChoice = new wxChoice(this, wxID_ANY);
    chartTypeChoice->Append("柱状图");
    chartTypeChoice->Append("饼图");
    chartTypeChoice->SetSelection(0);
    controlSizer->Add(new wxStaticText(this, wxID_ANY, "图表类型:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    controlSizer->Add(chartTypeChoice, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    mainSizer->Add(controlSizer, 0, wxALL | wxALIGN_LEFT, 10);
    
    // 创建绘图区域
    wxPanel* chartPanel = new wxPanel(this, wxID_ANY);
    chartPanel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    chartPanel->Bind(wxEVT_PAINT, &StatisticsDialog::OnPaint, this);
    mainSizer->Add(chartPanel, 1, wxEXPAND | wxALL, 10);
    
    // 绑定事件
    chartTypeChoice->Bind(wxEVT_CHOICE, &StatisticsDialog::OnChartTypeChanged, this);
    
    SetSizer(mainSizer);
    Center();
}

void StatisticsDialog::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();
    
    wxRect clientRect = GetClientRect();
    wxRect chartRect = clientRect;
    chartRect.Deflate(20); // 添加边距
    
    if (chartTypeChoice->GetSelection() == BAR_CHART) {
        DrawBarChart(dc, chartRect);
    } else {
        DrawPieChart(dc, chartRect);
    }
}

void StatisticsDialog::DrawBarChart(wxDC& dc, const wxRect& rect) {
    if (focusRecords.empty()) return;
    
    // 计算最近7天的数据
    auto now = std::chrono::system_clock::now();
    std::vector<int> dailyHours(7, 0);
    
    for (const auto& record : focusRecords) {
        auto duration = std::chrono::duration_cast<std::chrono::hours>(
            now - record.first).count();
        if (duration < 7 * 24) { // 最近7天
            int dayIndex = 6 - (duration / 24);
            dailyHours[dayIndex] += record.second / 3600; // 转换为小时
        }
    }
    
    // 绘制坐标轴
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(rect.x, rect.y + rect.height - 20, 
                rect.x + rect.width, rect.y + rect.height - 20); // X轴
    dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height - 20); // Y轴
    
    // 计算柱状图参数
    int barWidth = rect.width / 8;
    int maxHours = *std::max_element(dailyHours.begin(), dailyHours.end());
    if (maxHours == 0) maxHours = 1;
    
    // 绘制柱状图
    for (int i = 0; i < 7; ++i) {
        int barHeight = (dailyHours[i] * (rect.height - 40)) / maxHours;
        int x = rect.x + (i + 1) * barWidth;
        int y = rect.y + rect.height - 20 - barHeight;
        
        // 绘制柱子
        dc.SetBrush(wxBrush(wxColour(100, 150, 200)));
        dc.DrawRectangle(x - barWidth/2, y, barWidth, barHeight);
        
        // 绘制标签
        wxString label = wxString::Format("%d小时", dailyHours[i]);
        dc.DrawText(label, x - barWidth/2, y - 20);
        
        // 绘制日期
        auto date = std::chrono::system_clock::now() - std::chrono::hours(24 * (6 - i));
        auto time = std::chrono::system_clock::to_time_t(date);
        std::tm* tm = std::localtime(&time);
        wxString dateStr = wxString::Format("%d/%d", tm->tm_mon + 1, tm->tm_mday);
        dc.DrawText(dateStr, x - barWidth/2, rect.y + rect.height - 15);
    }
}

void StatisticsDialog::DrawPieChart(wxDC& dc, const wxRect& rect) {
    if (focusRecords.empty()) return;
    
    // 计算最近7天的数据
    auto now = std::chrono::system_clock::now();
    std::vector<int> dailyHours(7, 0);
    int totalHours = 0;
    
    for (const auto& record : focusRecords) {
        auto duration = std::chrono::duration_cast<std::chrono::hours>(
            now - record.first).count();
        if (duration < 7 * 24) { // 最近7天
            int dayIndex = 6 - (duration / 24);
            dailyHours[dayIndex] += record.second / 3600; // 转换为小时
            totalHours += record.second / 3600;
        }
    }
    
    if (totalHours == 0) return;
    
    // 绘制饼图
    int centerX = rect.x + rect.width / 2;
    int centerY = rect.y + rect.height / 2;
    int radius = std::min(rect.width, rect.height) / 2 - 20;
    
    double startAngle = 0;
    const wxColour colors[] = {
        wxColour(255, 100, 100),
        wxColour(100, 255, 100),
        wxColour(100, 100, 255),
        wxColour(255, 255, 100),
        wxColour(255, 100, 255),
        wxColour(100, 255, 255),
        wxColour(200, 200, 200)
    };
    
    for (int i = 0; i < 7; ++i) {
        if (dailyHours[i] > 0) {
            double angle = (dailyHours[i] * 360.0) / totalHours;
            
            dc.SetBrush(wxBrush(colors[i]));
            dc.SetPen(*wxBLACK_PEN);
            dc.DrawEllipticArc(centerX - radius, centerY - radius,
                             radius * 2, radius * 2,
                             startAngle, startAngle + angle);
            
            // 绘制标签
            double midAngle = startAngle + angle / 2;
            double labelX = centerX + (radius + 20) * cos(midAngle * M_PI / 180);
            double labelY = centerY + (radius + 20) * sin(midAngle * M_PI / 180);
            
            auto date = std::chrono::system_clock::now() - std::chrono::hours(24 * (6 - i));
            auto time = std::chrono::system_clock::to_time_t(date);
            std::tm* tm = std::localtime(&time);
            wxString label = wxString::Format("%d/%d: %d小时", 
                tm->tm_mon + 1, tm->tm_mday, dailyHours[i]);
            
            dc.DrawText(label, labelX, labelY);
            
            startAngle += angle;
        }
    }
}

void StatisticsDialog::OnChartTypeChanged(wxCommandEvent& event) {
    Refresh();
} 