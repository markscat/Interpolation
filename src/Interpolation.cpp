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
double Interpolation::spline(const std::vector<double>& x, const std::vector<double>& y, double tx) { 
    size_t n = x.size();
    // 樣條插值至少需要 3 個點，若點數不足則退回線性插值
    if (n < 3) return lerp(x, y, tx);

    // 1. 計算步長 h
    std::vector<double> h(n - 1);
    for (size_t i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i];
        if (h[i] < 1e-12) return y[i]; // 防止重複點
    }

    // 2. 建立三對角矩陣 (Thomas Algorithm 用)
    // 矩陣結構: [A] * [M] = [B]
    // M 是二階導數 (Moments)
    std::vector<double> a(n), b(n), c(n), d(n);

    // 自然邊界條件: M[0] = M[n-1] = 0
    b[0] = 1.0; c[0] = 0.0; d[0] = 0.0;
    b[n - 1] = 1.0; a[n - 1] = 0.0; d[n - 1] = 0.0;

    for (size_t i = 1; i < n - 1; ++i) {
        a[i] = h[i - 1];
        b[i] = 2.0 * (h[i - 1] + h[i]);
        c[i] = h[i];
        d[i] = 6.0 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
    }

    // 3. 解三對角矩陣 (追趕法)
    std::vector<double> m(n); // 二階導數解
    for (size_t i = 1; i < n; ++i) {
        double m_factor = a[i] / b[i - 1];
        b[i] = b[i] - m_factor * c[i - 1];
        d[i] = d[i] - m_factor * d[i - 1];
    }

    m[n - 1] = d[n - 1] / b[n - 1];
    for (int i = n - 2; i >= 0; --i) {
        m[i] = (d[i] - c[i] * m[i + 1]) / b[i];
    }

    // 4. 尋找 tx 所在的區間
    size_t idx = 0;
    if (tx <= x[0]) return y[0];
    if (tx >= x[n - 1]) return y[n - 1];

    for (size_t i = 0; i < n - 1; ++i) {
        if (tx >= x[i] && tx <= x[i + 1]) {
            idx = i;
            break;
        }
    }

    // 5. 使用樣條公式計算 y
    double dx = tx - x[idx];
    double h_idx = h[idx];

    // 三次樣條標準公式
    double term1 = m[idx] * std::pow(x[idx + 1] - tx, 3) / (6.0 * h_idx);
    double term2 = m[idx + 1] * std::pow(dx, 3) / (6.0 * h_idx);
    double term3 = (y[idx] - m[idx] * h_idx * h_idx / 6.0) * (x[idx + 1] - tx) / h_idx;
    double term4 = (y[idx + 1] - m[idx + 1] * h_idx * h_idx / 6.0) * dx / h_idx;

    return term1 + term2 + term3 + term4;
}

double Interpolation::pchip(const std::vector<double>& x, const std::vector<double>& y, double tx) {
    size_t n = x.size();
    if (n < 2) return 0.0;
    if (n == 2) return lerp(x, y, tx); // 兩個點只能連直線

    // 1. 計算各段的步長 (h) 與斜率 (d)
    std::vector<double> h(n - 1);
    std::vector<double> d(n - 1);
    for (size_t i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i]; 
        if (h[i] < 1e-12) return y[i]; // 防止重複點
        d[i] = (y[i + 1] - y[i]) / h[i];
    }

    // 2. 計算每個數據點上的導數 (m) —— 這是 PCHIP 的靈魂
    std::vector<double> m(n);

    // 內部點的導數計算 (使用加權諧波平均)
    for (size_t i = 1; i < n - 1; ++i) {
        if (d[i - 1] * d[i] > 0) {
            // 如果兩邊斜率同號，使用加權諧波平均
            double w1 = 2.0 * h[i] + h[i - 1];
            double w2 = h[i] + 2.0 * h[i - 1];
            m[i] = (w1 + w2) / (w1 / d[i - 1] + w2 / d[i]);
        }
        else {
            // 如果斜率異號，說明是極值點，導數強制設為 0 (防止震盪)
            m[i] = 0.0;
        }
    }

    // 端點的導數處理 (非中心差分)
    m[0] = ((2.0 * h[0] + h[1]) * d[0] - h[0] * d[1]) / (h[0] + h[1]);
    if (m[0] * d[0] < 0) m[0] = 0.0; // 修正端點震盪
    else if ((d[0] * d[1] < 0) && (std::abs(m[0]) > std::abs(3.0 * d[0]))) m[0] = 3.0 * d[0];

    m[n - 1] = ((2.0 * h[n - 2] + h[n - 3]) * d[n - 2] - h[n - 2] * d[n - 3]) / (h[n - 2] + h[n - 3]);
    if (m[n - 1] * d[n - 2] < 0) m[n - 1] = 0.0;
    else if ((d[n - 2] * d[n - 3] < 0) && (std::abs(m[n - 1]) > std::abs(3.0 * d[n - 2]))) m[n - 1] = 3.0 * d[n - 2];

    // 3. 尋找 tx 所在的區間
    if (tx <= x[0]) return y[0];
    if (tx >= x[n - 1]) return y[n - 1];

    size_t i = 0;
    for (size_t k = 0; k < n - 1; ++k) {
        if (tx >= x[k] && tx <= x[k + 1]) {
            i = k;
            break;
        }
    }

    // 4. Hermite 三次插值多項式
    double t = (tx - x[i]) / h[i];
    double t2 = t * t;
    double t3 = t * t * t;

    // 使用 Hermite 基底函數
    double h00 = 2 * t3 - 3 * t2 + 1;
    double h10 = t3 - 2 * t2 + t;
    double h01 = -2 * t3 + 3 * t2;
    double h11 = t3 - t2;

    return h00 * y[i] + h10 * h[i] * m[i] + h01 * y[i + 1] + h11 * h[i] * m[i + 1];
}

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