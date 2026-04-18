#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <vector>
#include <string>

// 基本點結構
struct Point {
    double x;
    double y;
    Point() : x(0), y(0) {}
    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};


enum class InterpolationMethod {
    Linear,         // 線性 (最安全)
    Lagrange,       // 拉格朗日 (你目前的)
    CubicSpline,    // 三次樣條 (最平滑)
    PCHIP           // 單調三次 (最適合電子元件，不震盪)
};




// 一組數據集：在某個 Z 條件下的 (X, Y) 點集
struct DataSet {
    double Z;                        // 條件參數
    std::vector<double> X;           // X 數據點
    std::vector<double> Y;           // Y 數據點
    std::string description;         // 描述

    DataSet() : Z(0) {}
    DataSet(double z, const std::vector<double>& x, const std::vector<double>& y, const std::string& desc = "")
        : Z(z), X(x), Y(y), description(desc) {
    }
};

/** 
* @brief Interpolation 類別：提供多種插值方法的實作
 * 這個類別包含了多種插值方法的實作，並且提供了從數據集產生插值曲線的功能。
 * 你可以根據需要選擇不同的插值方法來生成更平滑或更適合電子元件特性的曲線。
 * @details 目前已經實作了 Lagrange 和 Linear 插值方法，Cubic Spline 和 PCHIP 的實作將在後續完成。
 *  這些方法都接受一組已知的數據點，並且能夠對任意目標 x 值進行插值計算。
 */
class Interpolation {

public:
    /**
    * @brief Lagrange 插值法 (單點)
    */
    static double lagrange(const std::vector<double>& x_values,
        const std::vector<double>& y_values,
        double x_target);


    /**
	* @brief 線性插值法 (單點)
	* @param x 已知的 x 數據點
	* @param y 已知的 y 數據點
	* @param target 需要插值的目標 x 值
    * @return double 對應 target 的插值結果
     * @details 這個方法實現了線性插值，接受一組已知的 x 和 y 數據點，以及一個目標 x 值。
     * 它會找到目標 x 所在的區間，然後使用該區間的兩個點進行線性插值計算。方法中包含了基本的錯誤檢查，
	 * 例如確保輸入數據點的大小匹配，以及避免除以零的情況（當有重複的 x 值時）。如果目標 x 超出數據範圍，則返回最後一個 y 值。
    */
    static double lerp(const std::vector<double>& x, const std::vector<double>& y, double target);
    
    /**
    * @brief Cubic Spline 插值法 (單點)
     * @param x 已知的 x 數據點
     * @param y 已知的 y 數據點
     * @param target 需要插值的目標 x 值
     * @return double 對應 target 的插值結果
      * @details 這個方法實現了三次樣條插值，接受一組已知的 x 和 y 數據點，以及一個目標 x 值。
	  * 它會計算每個區間的三次多項式係數，然後根據目標 x 所在的區間進行插值計算。方法中包含了基本的錯誤檢查，
    */
    static double spline(const std::vector<double>& x, const std::vector<double>& y, double target);

/**
* @brief PCHIP 插值法 (單點)
* @param x 已知的 x 數據點
* @param y 已知的 y 數據點
* @param target 需要插值的目標 x 值
* @return double 對應 target 的插值結果
 * @details 這個方法實現了單調三次 Hermite 插值，接受一組已知的 x 和 y 數據點，以及一個目標 x 值。
 * 它會計算每個區間的斜率，並且根據斜率來確保插值曲線在數據點之間保持單調性，避免震盪。方法中包含了基本的錯誤檢查，
*/
    static double pchip(const std::vector<double>& x, const std::vector<double>& y, double target);


    /**
    * @brief 從 DataSet 產生插值曲線 (預設使用 Lagrange
    * @param dataset 包含 X, Y 原始點的數據集
    * @param x_min 期望的起始 X (會自動縮減至數據邊界)
    * @param x_max 期望的結束 X (會自動縮減至數據邊界)
    * @param num_points 採樣點數
    * @return 產生的曲線點集 (std::vector<Point>)
    */
    static std::vector<Point> generateCurve(const DataSet& dataset,
        double x_min,
        double x_max,
        int num_points = 100,
        InterpolationMethod method = InterpolationMethod::Linear);

    /**
    * @brief 從多組 DataSet 產生多條曲線 (用於比較不同 Z)
    * @param datasets 多組數據集
    * @param x_min X 最小值
    * @param x_max X 最大值
    * @param num_points 每條曲線的點數
    * @return 多條曲線的點集 (每個 DataSet 對應一條)
    */
    static std::vector<std::vector<Point>> generateMultiCurves(
        const std::vector<DataSet>& datasets,
        double x_min,
        double x_max,
        int num_points = 100);
};

#endif