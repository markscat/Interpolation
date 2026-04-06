#include "../include/Interpolation.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>


/** @brief Interpolation::lagrange 單點插值
*  L(x) = \sum_{i=0}^{n} y_i \cdot \prod_{j \ne i} \frac{x - x_j}{x_i - x_j}
 * @param x_values 已知的 x 數據點
 * @param y_values 已知的 y 數據點
 * @param x_target 需要插值的目標 x 值
 * @return double 對應 x_target 的插值結果
 * @details 這個方法實現了 Lagrange 插值法，接受一組已知的 x 和 y 數據點，以及一個目標 x 值。
  它會計算並返回對應於 x_target 的插值結果。方法中包含了基本的錯誤檢查，例如確保輸入數據點的大小匹配，
  以及避免除以零的情況（當有重複的 x 值時）。

*/
double Interpolation::lagrange(const std::vector<double>& x_values,
                               const std::vector<double>& y_values,
                               double x_target) {
    if (x_values.size() != y_values.size()) {
        throw std::invalid_argument("x_values and y_values size mismatch");
    }
    if (x_values.size() < 2) {
        throw std::invalid_argument("Need at least 2 points");
    }

    size_t n = x_values.size();
    double result = 0.0;

    for (size_t i = 0; i < n; ++i) {
        double term = y_values[i];
        for (size_t j = 0; j < n; ++j) {
            if (j != i) {
                if (std::abs(x_values[i] - x_values[j]) < 1e-9) {
                    throw std::invalid_argument("Duplicate x values");
                }
                term *= (x_target - x_values[j]) / (x_values[i] - x_values[j]);
                /* term *= (x_target - x_values[j]) / (x_values[i] - x_values[j]);
                   這裡是 Lagrange 插值的核心計算部分，對每個 i 的項進行累乘。
                   這段代碼會對每個 j（除了 i）計算一個分式，將其乘到 term 上。
                   分子是 (x_target - x_values[j])，分母是 (x_values[i] - x_values[j])。
				   這樣就構建了 Lagrange 基函數的一部分，最終將所有的 term 加總起來得到插值結果。
                */
            }
        }
        result += term;
    }
    return result;
}

std::vector<Point> Interpolation::generateCurve(const DataSet& dataset,
                                                double x_min,
                                                double x_max,
                                                int num_points,
                                                InterpolationMethod method) {

    std::vector<Point> curve;
    curve.reserve(num_points);

	// 基本檢查：確保數據集不為空，且點數合理
    if (dataset.X.empty() || num_points < 2) return curve;

    // ==========================================
    // 1. 自動修正邊界：確保不進行「外推」
    // ==========================================
    // 找出數據集實際擁有的最小值和最大值
    double data_start = dataset.X.front();
    double data_end = dataset.X.back();


    // ==========================================
    // 強制約束：絕對不允許超出區間（拒絕瞎猜）
    // ==========================================
    // 如果要求的 x_min 比數據起點還小，就從數據起點開始
    double safe_xmin = std::max(x_min, data_start);
    // 如果要求的 x_max 比數據終點還大，就到數據終點結束
    double safe_xmax = std::min(x_max, data_end);


    // 如果安全區間不合法（例如要求的範圍完全在數據之外），直接回傳空向量
    if (safe_xmin >= safe_xmax) {
        return curve;
    }

    curve.reserve(num_points);

    double step = (safe_xmax- safe_xmin) / (num_points - 1);

    for (int i = 0; i < num_points; ++i) {

        double x = safe_xmin + i * step;
        double y = 0;



        //y = lagrange(dataset.X, dataset.Y, x);


        // --- 使用 switch-case 處理所有演算法 ---
        switch (method) {
        case InterpolationMethod::Linear:
            y = lerp(dataset.X, dataset.Y, x);
            break;

        case InterpolationMethod::Lagrange:
            y = lagrange(dataset.X, dataset.Y, x);
            break;

        case InterpolationMethod::CubicSpline:
            y = spline(dataset.X, dataset.Y, x); // 待實作
            break;

        case InterpolationMethod::PCHIP:
            y = pchip(dataset.X, dataset.Y, x); // 待實作
            break;

        default:
            y = lagrange(dataset.X, dataset.Y, x); // 安全回退
            break;
        }


        // ==========================================
        // 2. 物理限幅：絕對不能是負數
        // ==========================================
        //這裡假設 y 的物理意義是某種「量」，因此不能為負。根據實際情況，你也可以改成其他的限制條件。
        //if (y < 0) y = 0;

        curve.push_back(Point(x, y));
    }
    return curve;
}

/**
* @brief Interpolation::lerp 線性插值
* 線性插值的公式是：y = y0 + (y1 - y0) * (x - x0) / (x1 - x0)
* 其中 (x0, y0) 和 (x1, y1) 是目標 x 所在區間的兩個已知點。
* 
* @param x_v 已知的 x 數據點
* @param y_v 已知的 y 數據點
* @param tx 需要插值的目標 x 值
* @return double 對應 tx 的插值結果
* 
* @details 這個方法實現了線性插值，接受一組已知的 x 和 y 數據點，以及一個目標 x 值。
  它會找到目標 x 所在的區間，然後使用該區間的兩個點進行線性插值計算。方法中包含了基本的錯誤檢查，
  例如確保輸入數據點的大小匹配，以及避免除以零的情況（當有重複的 x 值時）。如果目標 x 超出數據範圍，則返回最後一個 y 值。

*/
double Interpolation::lerp(const std::vector<double>& x_v, const std::vector<double>& y_v, double tx) {
    if (x_v.size() < 2) return 0.0;
    for (size_t i = 0; i < x_v.size() - 1; ++i) {
        if (tx >= x_v[i] && tx <= x_v[i + 1]) {
            double dx = x_v[i + 1] - x_v[i];
            if (std::abs(dx) < 1e-12) return y_v[i];
            return y_v[i] + (y_v[i + 1] - y_v[i]) * (tx - x_v[i]) / dx;
        }
    }
    return y_v.back();
}

// spline 和 pchip 先留空標記，等到要實作時再補內容即可
double Interpolation::spline(const std::vector<double>& x, const std::vector<double>& y, double tx) { return 0.0; }
double Interpolation::pchip(const std::vector<double>& x, const std::vector<double>& y, double tx) { return 0.0; }

std::vector<std::vector<Point>> Interpolation::generateMultiCurves(
    const std::vector<DataSet>& datasets,
    double x_min,
    double x_max,
    int num_points) {

    std::vector<std::vector<Point>> allCurves;
    allCurves.reserve(datasets.size());

    for (const auto& dataset : datasets) {
        allCurves.push_back(generateCurve(dataset, x_min, x_max, num_points));
    }
    return allCurves;
}