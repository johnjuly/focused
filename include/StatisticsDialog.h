#pragma once
#include <wx/wx.h>
#include <wx/dialog.h>
#include <vector>
#include <chrono>

class StatisticsDialog : public wxDialog {
public:
    StatisticsDialog(wxWindow* parent, const std::vector<std::pair<std::chrono::system_clock::time_point, int>>& records);
    
private:
    void OnPaint(wxPaintEvent& event);
    void DrawBarChart(wxDC& dc, const wxRect& rect);
    void DrawPieChart(wxDC& dc, const wxRect& rect);
    
    std::vector<std::pair<std::chrono::system_clock::time_point, int>> focusRecords;
    wxChoice* chartTypeChoice;
    
    enum ChartType {
        BAR_CHART,
        PIE_CHART
    };
    
    void OnChartTypeChanged(wxCommandEvent& event);
}; 