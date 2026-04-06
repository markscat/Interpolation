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

class Interpolation {

public:
    /**
    * @brief Lagrange 插值法 (單點)
    */
    static double lagrange(const std::vector<double>& x_values,
        const std::vector<double>& y_values,
        double x_target);



    static double lerp(const std::vector<double>& x, const std::vector<double>& y, double target);
    
    static double spline(const std::vector<double>& x, const std::vector<double>& y, double target);
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