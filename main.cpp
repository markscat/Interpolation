#include "include/Interpolation.h"
#include "include/File/File_save.h"
#include "include/cauver/curveexporter.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>


int main() {
    // 1. 準備實驗數據 (Ic vs hFE)
    DataSet hfeData(
        1.0,
        { 0.1, 1.0, 10.0 },
        { 40.0, 80.0, 120.0 },
        "BJT hFE Test Data"
    );

    // 設定繪圖參數：在數據有效的 0.1 ~ 10.0 區間內產生 100 個採樣點
    double xMin = 0.1;
    double xMax = 10.0;
    int points = 100;

    std::cout << "開始計算並匯出曲線數據..." << std::endl;

    // ==========================================
    // 2. 處理 Lagrange 插值 (會震盪、有大肚子的拋物線)
    // ==========================================
    auto lagrangeCurve = Interpolation::generateCurve(hfeData, xMin, xMax, points, InterpolationMethod::Lagrange);

    // 檔名：hFE_Lagrange_YYYYMMDD_HHMMSS.csv
    std::string lagrangeFilename = File_save::generateTimestampFilename("hFE_Lagrange_", ".csv");

    if (CurveExporter::toCSV(lagrangeCurve, lagrangeFilename, "Ic(mA)", "hFE")) {
        std::cout << "[成功] Lagrange 數據已儲存至: " << lagrangeFilename << std::endl;
    }

    // ==========================================
    // 3. 處理 Linear 插值 (連直線、絕對穩定的折線)
    // ==========================================
    auto linearCurve = Interpolation::generateCurve(hfeData, xMin, xMax, points, InterpolationMethod::Linear);

    // 檔名：hFE_Linear_YYYYMMDD_HHMMSS.csv
    std::string linearFilename = File_save::generateTimestampFilename("hFE_Linear_", ".csv");

    if (CurveExporter::toCSV(linearCurve, linearFilename, "Ic(mA)", "hFE")) {
        std::cout << "[成功] Linear   數據已儲存至: " << linearFilename << std::endl;
    }

    // ==========================================
    // 4. 單點驗證 (不存檔，僅在終端機顯示座標)
    // ==========================================
    double testIc = 5.0;
    std::cout << "\n--- 單點座標驗證 (Ic = 5.0 mA) ---" << std::endl;
    std::cout << "Lagrange 求得 y = " << Interpolation::lagrange(hfeData.X, hfeData.Y, testIc) << std::endl;
    std::cout << "Linear   求得 y = " << Interpolation::lerp(hfeData.X, hfeData.Y, testIc) << std::endl;

    std::cout << "\n處理完成。你可以用 Excel 開啟這兩個 CSV 檔案來比較差異。" << std::endl;

    return 0;
}
