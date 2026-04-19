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
* 線性插值的公式是：
* 公式表示法:
*   y = y0 + ( (y1 - y0) * ( (x - x0) / (x1 - x0) ) )
* latex表示法:
*   y = y_0 + \left[ (y_1 - y_0) \cdot \left( \frac{x - x_0}{x_1 - x_0} \right) \right]
* 其中 (x0, y0) 和 (x1, y1) 是目標 x 所在區間的兩個已知點。
* @param x_v 已知的 x 數據點
* @param y_v 已知的 y 數據點
* @param tx 需要插值的目標 x 值
* @return double 對應 tx 的插值結果
* 
* 這個公式的計算過程可以分解為以下幾個步驟：
* 第一步： (x - x0) -> 目標點距離起點的距離（Δx）。
* 第二步： (x1 - x0) -> 整個區間的總長度（L）。
* 第三步:  ( (x - x0) / (x1 - x0) ) -> 算出目標點在區間內的「百分比位置」。
* 第四步:  (y1 - y0) -> 區間內的 y 值變化量（Δy）。
* 第五步:  全部相乘後加上 y0。
* 簡單來說：
* 先求出x已知的變數,X0是輸入的第一個點的x值,x1是第二個點的值
* 找出這兩個點中間的比例,然後把已知點y0到y1的距離求出,和比例相乘加上第一個點的y值
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

/**
* @brief Interpolation::spline 三次樣條插值
* @param x 已知的 x 數據點
* @param y 已知的 y 數據點
* @param tx 需要插值的目標 x 值
* @return double 對應 tx 的插值結果
* 
* @details
* 基於 二階導數（Moments, 記作 M） 的表達式
* 
* Term1 (起點彎曲貢獻):
* ((M[i] * ( (x[i+1] - x)^3 )) / (6 * h))
* Term2 (終點彎曲貢獻):
* ((M[i+1] * ( (x - x[i])^3 )) / (6 * h))
* Term3 (起點高度修正):
* ( (y[i] - ((M[i] * (h^2)) / 6)) * ((x[i+1] - x) / h) )
* Term4 (終點高度修正):
* ( (y[i+1] - ((M[i+1] * (h^2)) / 6)) * ((x - x[i]) / h) )
* 
* h=x1−x0
* 
* Latex表示法:
* y = \frac{M_i (x_{i+1} - x)^3}{6h} + \frac{M_{i+1} (x - x_i)^3}{6h} + 
      \left( y_i - \frac{M_i h^2}{6} \right) \frac{x_{i+1} - x}{h} + 
      \left( y_{i+1} - \frac{M_{i+1} h^2}{6} \right) \frac{x - x_i}{h}
      
  y = 
      \frac{M_i(x_{i+1}-x)^3}{6h_i} + 
      \frac{M_{i+1}(x-x_i)^3}{6h_i} + \left( \frac{y_i}{h_i} - 
      \frac{M_i h_i}{6} \right)(x_{i+1}-x) + \left( \frac{y_{i+1}}{h_i} - 
      \frac{M_{i+1} h_i}{6} \right)(x-x_i)
* 
* Spline 的本質是:
* Spline = 線性插值 (這兩點的高度) + 彎曲修正 (這兩點與鄰居的關係)
* 
* 三次樣條插值的核心思想是為每個區間 [x_i, x_{i+1}] 定義一個三次多項式 S_i(x)，
  並且確保這些多項式在數據點處連續且具有連續的一階和二階導數。這樣可以得到一條平滑的曲線，避免拉格朗日插值可能出現的震盪問題。
  
  三次樣條插值的計算過程通常包括以下幾個步驟：
  1. 計算每個區間的步長 h_i = x_{i+1} - x_i。
  2. 建立三對角矩陣來求解二階導數 M_i（也稱為「moments」），這些二階導數用於定義每個區間的三次多項式。
  3. 使用 Thomas Algorithm（追趕法）來高效地解這個三對角矩陣。
  4. 根據目標 x 所在的區間，使用對應的三次多項式公式來計算插值結果。

*/
double Interpolation::spline(const std::vector<double>& x, const std::vector<double>& y, double tx) { 
   
    size_t n = x.size();
    // 樣條插值至少需要 3 個點，若點數不足則退回線性插值
    if (n < 3) return lerp(x, y, tx);

    // 1. 計算步長 h=x1-x0, x2-x1, ...
    std::vector<double> h(n - 1);
    
    for (size_t i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i];
        if (h[i] < 1e-12) return y[i]; // 防止重複點
    }

    // 2. 建立三對角矩陣 (Thomas Algorithm 用)
    // 矩陣結構: [A] * [M] = [B]
    // M 是二階導數 (Moments)
    // 
	// 化成矩陣形式後的方程組如下：
    //       [ A ]     *  [ M ] = [ B ]
    // 
    //  | b0 c0  0  0 |   | M0 |  | d0 |
    //  | a1 b1 c1  0 | * | M1 |  | d1 |
    //  |  0 a2 b2 c2 |   | M2 |= | d2 |
    //  |  0  0 a3 b3 |   | M3 |  | d3 |
    // 

    /**
    * a (Lower Diagonal)：代表「左邊鄰居」對這一點的牽引係數。
    * b (Main Diagonal) ：代表「這一點自己」在平衡狀態中的支撐係數。
    * c (Upper Diagonal)：代表「右邊鄰居」對這一點的牽引係數。
    * d (Right Hand Side)：代表「數據逼迫」這一點必須產生的轉彎力道（已知條件）。
    */
    std::vector<double> a(n), b(n), c(n), d(n);

    // 自然邊界條件: M[0] = M[n-1] = 0
    b[0] = 1.0;
    c[0] = 0.0; 
    d[0] = 0.0;
    a[n - 1] = 0.0;
    b[n - 1] = 1.0;
    d[n - 1] = 0.0;

    //把相對應的資料,放入abcd這四個vector
    for (size_t i = 1; i < n - 1; ++i) {
        
        // a[i]: 裝入「左邊那段」鋼條的長度。
        // 它代表了左邊點 $M_{i-1}$ 對這一點的影響力。
        //已知:
        // h=x[i + 1] - x[i];
        //
        //if i=1
        //a[1]= h[0]
        //h[0]= x1-x0
        a[i] = h[i - 1];

        // b[i]: 這是「主角」，裝入「左段 + 右段」總長度的 2.0 倍。
        // 它是這一點 $M_i$ 自己的權重，因為它同時連接左右兩邊，所以要雙倍負擔。
        //b[1]=2.0 * h[0]+h[1]
        //b[1]= 2.0 * [(x1-x0)+(x2-x1)]
		b[i] = 2.0 * (h[i - 1] + h[i]);

        // c[i]: 裝入「右邊那段」鋼條的長度。
        // 它代表了右邊點 $M_{i+1}$ 對這一點的影響力。
        //c[1] = h[1]
        //c[1] = (x2-x1)
        c[i] = h[i];
        /*
            d[i]: 裝的是數據要求的「二階差分」，即斜率的變化率。
            乘以 6.0 是因為在 Spline 的二次積分推導中，分母會出現 6 (即 2*3)，
            為了抵消公式後段的 1/6，這裡先預先擴大 6 倍，讓解出來的 M 數值正確。
            d[1]= 6.0 *((y[2]-y[1])/h[1])-((y[1]- y[0])/h[0])
            d[1]= 6.0 *
                      ((y[2]-y[1])/(x[2]-x1))
                     -((y[1]-y[0])/(x[1]-x[0]))
            d[1] = 6.0 * ( (第二段斜率) - (第一段斜率) )
            第一段斜率S1是第一個角度
            第二段斜率S2是第二個角度
            S1-S2代表這條線段轉彎的地方
            
            幾何意義：它是「凹凸性」
            這個相減的結果，決定了曲線是**「向上彎」還是「向下彎」**：
            結果為正（S2 > S1）:
                斜率越來越大（比如從平的變成往上爬）。這代表曲線像個碗一樣**「向上開口」**。
            結果為正（S2 < S1）:
                斜率越來越小（比如從往上爬變成往下滑）。這代表曲線像個山丘一樣**「向下開口」**。
            結果為正（S2 = S1）:
                這是一個轉折點或直線，沒有彎曲。  
        */

        d[i] = 6.0 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
    }

    // 3. 解三對角矩陣 (追趕法)
    std::vector<double> m(n); // 二階導數解

    for (size_t i = 1; i < n; ++i) {
    /*
        a[1]= x1-x0
        b[0]= 1 (預設值)
        m_factor = x1-x0
        
        b[1] = b[1] - m_factor * c[0]
        => 2.0 * [(x1-x0)+(x2-x1)] - ((x1-x0)*(x2-x1))
        
        d[1] = d[1] - (m_factor-d[0])
             => d[1]= 6.0 *((y[2]-y[1])/(x[2]-x1))-((y[1]-y[0])/(x[1]-x[0])) -[(x1-x0)*0(d[0]=0預設值)]
             => d[1]= 6.0 *((y[2]-y[1])/(x[2]-x1))-((y[1]-y[0])/(x[1]-x[0]))
        
    */
        double m_factor = a[i] / b[i - 1];
        b[i] = b[i] - m_factor * c[i - 1];
        d[i] = d[i] - m_factor * d[i - 1];
    }

    /*
        最後一個點的二階導數 M[n-1]：
        根據「自然邊界條件」，我們在前面已經強迫設定：
        b[n-1] = 1.0, d[n-1] = 0.0
        
        所以最後一點的彎曲度永遠是：
        m[n-1] = 0.0 / 1.0 = 0
    */
    m[n - 1] = d[n - 1] / b[n - 1];
    
    for (int i = n - 2; i >= 0; --i) {
         /*
            這裡我們以 i = 1 (倒數第二個點) 為例，並把之前「裝貨」和「追趕」後的數值還原：
            
            1. 分母 b[1] 經過之前的「往前追」後，變成了：
               b[1] = 2.0 * (x[2] - x[0])  <-- 注意！這就是兩段區間的總跨度
               
            2. 分子 d[1] 當時「裝貨」的內容是：
               d[1] = 6.0 * [ ((y[2]-y[1])/(x[2]-x[1])) - ((y[1]-y[0])/(x[1]-x[0])) ]
               
            3. c[1] 是右邊那段的長度：
               c[1] = (x[2] - x[1])
               
            所以 M[1] 的座標還原公式為：
            m[1] = ( {6.0 * [((y[2]-y[1])/(x[2]-x[1])) - ((y[1]-y[0])/(x[1]-x[0]))]} - {(x[2]-x[1]) * m[2]} ) 
                   / (2.0 * (x[2] - x[0]))

            因為 m[2] = 0 (終點不彎曲)，所以簡化後：
            m[1] = 3.0 * [((y[2]-y[1])/(x[2]-x[1])) - ((y[1]-y[0])/(x[1]-x[0]))] / (x[2] - x[0])
            
            M_1 = \frac{3 \left( \frac{y_2 - y_1}{x_2 - x_1} - \frac{y_1 - y_0}{x_1 - x_0} \right)}{x_2 - x_0}  
            白話文：
            「第 1 點的彎曲度，就是兩側斜率的變動量，除以總跨度，再修正 3 倍的係數。」
        */
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