#include "include/Interpolation.h"
#include "include/File/File_save.h"
#include "include/cauver/curveexporter.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>


int main() {
    std::cout << "===============================================" << std::endl;
    std::cout << "   數值分析系統：BJT hFE 特性曲線模擬測試      " << std::endl;
    std::cout << "===============================================" << std::endl;

    // 1. 準備實驗數據 (DataSet)
    // 數據點：(0.1, 40), (1.0, 80), (10.0, 120)
    DataSet hfeData(
        1.0,                       // Vce = 1V
        { 0.1, 1.0, 10.0 },        // Ic (mA)
        { 40.0, 80.0, 120.0 },     // hFE
        "BJT Ta=25C Vce=1V"        // 描述
    );

    // 2. 設定繪圖範圍與參數 (嚴格遵守 0.1~10.0 區間，拒絕外推瞎猜)
    double xMin = 0.1;
    double xMax = 10.0;
    int samplePoints = 100;    // 產生 100 個插值點讓曲線更滑順

    // 3. 定義要測試的方法列表與對應的檔案前綴
    struct TestConfig {
        InterpolationMethod method;
        std::string name;
        std::string filePrefix;
    };

    std::vector<TestConfig> configs = {
        { InterpolationMethod::Linear,      "線性插值 (Linear)",   "hFE_Linear_" },
        { InterpolationMethod::Lagrange,    "拉格朗日 (Lagrange)", "hFE_Lagrange_" },
        { InterpolationMethod::CubicSpline, "三次樣條 (Spline)",   "hFE_Spline_" },
        { InterpolationMethod::PCHIP,       "單調三次 (PCHIP)",    "hFE_PCHIP_" }
    };

    // 4. 執行批量計算與檔案匯出
    for (const auto& config : configs) {
        std::cout << "\n正在處理 " << config.name << "..." << std::endl;

        // A. 批量產生曲線點
        auto curve = Interpolation::generateCurve(hfeData, xMin, xMax, samplePoints, config.method);

        // B. 產生具備時間戳記的檔案名稱
        std::string filename = File_save::generateTimestampFilename(config.filePrefix, ".csv");

        // C. 匯出為 CSV (使用你之前寫好的 CurveExporter)
        if (CurveExporter::toCSV(curve, filename, "Ic(mA)", "hFE")) {
            std::cout << "   -> [成功] 檔案已儲存至: " << filename << std::endl;
        }
        else {
            std::cout << "   -> [失敗] 無法寫入檔案: " << filename << std::endl;
        }
    }

    // 5. 展示「單點求座標」的功能 (使用者最核心的需求)
    std::cout << "\n===============================================" << std::endl;
    std::cout << "   特定座標計算驗證 (當 Ic = 5.0 mA 時)        " << std::endl;
    std::cout << "===============================================" << std::endl;

    double testIc = 5.0;
    std::cout << std::fixed << std::setprecision(4);

    // 直接呼叫 Public 演算法函式求 Y
    std::cout << "Linear   求得 hFE: " << Interpolation::lerp(hfeData.X, hfeData.Y, testIc) << " (預期 97.7 左右)" << std::endl;
    std::cout << "Lagrange 求得 hFE: " << Interpolation::lagrange(hfeData.X, hfeData.Y, testIc) << " (預期震盪過衝)" << std::endl;
    std::cout << "Spline   求得 hFE: " << Interpolation::spline(hfeData.X, hfeData.Y, testIc) << " (極度平滑)" << std::endl;
    std::cout << "PCHIP    求得 hFE: " << Interpolation::pchip(hfeData.X, hfeData.Y, testIc) << " (保證單調且平滑)" << std::endl;

    std::cout << "\n測試完成。請查看當前資料夾下的 CSV 檔案並使用 Excel 繪圖比較。" << std::endl;

    return 0;
}