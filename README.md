# 數值分析插值工具箱 (Numerical Interpolation Toolbox)

本專案提供了一套高獨立性、隨插即用的 C++ 插值演算法類別 `Interpolation.cpp`。專為工程數據模擬與物理特性曲線（如 BJT 的 $h_{FE}$ 特性）設計，解決了傳統插值法在數據跨度大時容易產生的震盪問題。

---

## 支援演算法說明 (Mathematical Background)

### 1. 分段線性插值 (Piecewise Linear Interpolation)
在每對相鄰數據點 $(x_i, y_i)$ 與 $(x_{i+1}, y_{i+1})$ 之間建立一階直線方程。

**公式：**
$$y = y_i + (y_{i+1} - y_i) \frac{x - x_i}{x_{i+1} - x_i}$$

*   **特性**：最穩健的方法，絕對保證數值在已知區間內。
*   **適用**：數據點極其密集，且不容許任何數學預測誤差的場景。

---

### 2. 拉格朗日插值 (Lagrange Interpolation)
使用單一 $n$ 階多項式穿過所有 $n+1$ 個已知數據點。

**公式：**
$$L(x) = \sum_{j=0}^{n} y_j \ell_j(x)$$
其中基底函數 $\ell_j(x)$ 為：
$$\ell_j(x) = \prod_{k=0, k \neq j}^{n} \frac{x - x_k}{x_j - x_k}$$

*   **特性**：數學構造優雅，但在處理非均勻分佈數據時，容易產生 **龍格現象 (Runge's phenomenon)**，導致曲線在邊界處產生劇烈震盪（Overshoot）。

---

### 3. 三次樣條插值 (Natural Cubic Spline)
在每對點之間建立三次多項式 $S_i(x)$，並強制要求連接點處的一階與二階導數皆連續。

**公式：**
$$S_i(x) = a_i + b_i(x - x_i) + c_i(x - x_i)^2 + d_i(x - x_i)^3$$
本專案採用 **自然邊界條件 (Natural Boundary Condition)**，即 $S''(x_0) = S''(x_n) = 0$，並透過高效的 **Thomas Algorithm (追趕法)** 求解三對角矩陣。

*   **特性**：視覺效果最平滑，是工程繪圖的標準演算法。

---

### 4. 單調三次 Hermite 插值 (PCHIP)
**Piecewise Cubic Hermite Interpolating Polynomial**。專為物理模擬設計，解決了 Spline 與 Lagrange 的震盪問題。

**核心邏輯：**
在每個點計算斜率 $m_i$。若相鄰段斜率異號（存在極小/大值點），則強制令該點斜率 $m_i = 0$。插值公式採用 Hermite 基底：
$$P(x) = h_{00}(t)y_i + h_{10}(t)h_i m_i + h_{01}(t)y_{i+1} + h_{11}(t)h_i m_{i+1}$$
其中 $t = (x - x_i) / h_i$。

*   **特性**：**具備單調性保護 (Shape-preserving)**。它保證曲線在數據範圍內平滑流動，絕對不會產生物理上不合理的「鼓包」或「回頭拋物線」。**在模擬電晶體 $h_{FE}$ 特性時最為準確。**

---

## 演算法比較表 (Comparison Table)

| 演算法 (Method) | 平滑度 (Smoothness) | 單調性 (Monotonicity) | 物理真實性 |
| :--- | :--- | :--- | :--- |
| **Linear** | 低 (有折角) | 絕對保證 | 普通 |
| **Lagrange** | 高 | 極差 (易震盪) | 差 (數據跨度大時失真) |
| **Spline** | 極高 (最圓滑) | 普通 (可能有輕微震盪) | 優 |
| **PCHIP** | 高 | **優秀 (絕對不震盪)** | **極佳 (電子元件模擬首選)** |

---

## 如何使用 (Usage)

本工具箱採用「隨插即用」設計，僅需將 `Interpolation.h` 與 `Interpolation.cpp` 加入專案即可。

```cpp
// 1. 定義數據集
DataSet myData(1.0, {0.1, 1.0, 10.0}, {40, 80, 120});

// 2. 求特定座標點 (x=5.0)
double y = Interpolation::pchip(myData.X, myData.Y, 5.0);

// 3. 產生繪圖用曲線 (0.1 到 10.0 區間)
auto curve = Interpolation::generateCurve(myData, 0.1, 10.0, 100, InterpolationMethod::PCHIP);
```

如果有任何疑問,或是我的演算法哪邊有問題,煩請和我聯絡
markscar@gmail.com

# Numerical Interpolation Toolbox (C++)

A high-portability, standalone C++ library for numerical interpolation. This toolbox is designed specifically for engineering data simulation and physical characteristic modeling (such as Transistor $h_{FE}$ curves). It effectively eliminates the "overshoot" issues commonly found in traditional interpolation methods when dealing with non-uniform data.

---

## Supported Interpolation Methods

### 1. Piecewise Linear Interpolation
The most robust and fundamental method. It constructs a first-order linear equation between each pair of adjacent data points $(x_i, y_i)$ and $(x_{i+1}, y_{i+1})$.

**Formula:**
$$y = y_i + (y_{i+1} - y_i) \frac{x - x_i}{x_{i+1} - x_i}$$

*   **Characteristics**: Extremely stable. It strictly guarantees that interpolated values stay within the bounds of the original dataset.
*   **Best For**: Dense datasets where mathematical "guessing" or smoothing is not allowed.

---

### 2. Lagrange Interpolation
Constructs a single $n$-degree polynomial that passes through all $n+1$ known data points.

**Formula:**
$$L(x) = \sum_{j=0}^{n} y_j \ell_j(x)$$
where the basis polynomial $\ell_j(x)$ is:
$$\ell_j(x) = \prod_{k=0, k \neq j}^{n} \frac{x - x_k}{x_j - x_k}$$

*   **Characteristics**: Mathematically elegant. However, it is highly susceptible to **Runge's phenomenon**, causing wild oscillations (overshoot) at the boundaries when data points are numerous or unevenly spaced.

---

### 3. Natural Cubic Spline Interpolation
Constructs a cubic polynomial $S_i(x)$ between each pair of points, ensuring that the first and second derivatives are continuous at the knots.

**Formula:**
$$S_i(x) = a_i + b_i(x - x_i) + c_i(x - x_i)^2 + d_i(x - x_i)^3$$
This implementation employs the **Natural Boundary Condition**, where $S''(x_0) = S''(x_n) = 0$. The system of equations is solved efficiently using the **Thomas Algorithm** ($O(n)$ complexity).

*   **Characteristics**: Provides the highest level of visual smoothness.
*   **Best For**: General data visualization where a "professional" and rounded appearance is required.

---

### 4. PCHIP (Piecewise Cubic Hermite Interpolating Polynomial)
A specialized algorithm designed for physical modeling. It solves the oscillation problems inherent in Spline and Lagrange methods.

**Core Logic:**
PCHIP calculates the slope $m_i$ at each point. If the slopes of adjacent segments have opposite signs (indicating a local extremum), it forces $m_i = 0$. The interpolation uses the Hermite basis:
$$P(x) = h_{00}(t)y_i + h_{10}(t)h_i m_i + h_{01}(t)y_{i+1} + h_{11}(t)h_i m_{i+1}$$
where $t = (x - x_i) / h_i$.

*   **Characteristics**: **Shape-preserving and Monotonic**. It ensures the curve flows smoothly within the data bounds without ever producing unphysical "bumps" or "dips."
*   **Engineering Verdict**: **This is the preferred method for simulating semiconductor characteristics (e.g., $h_{FE}$ vs. $I_c$).**

---

## Algorithm Comparison Table

| Method | Smoothness | Monotonicity (No Overshoot) | Physical Realism |
| :--- | :--- | :--- | :--- |
| **Linear** | Low (Sharp angles) | **Guaranteed** | Moderate |
| **Lagrange** | High | Very Poor (Oscillates) | Poor (Unphysical for $h_{FE}$) |
| **Spline** | **Extreme (Roundest)** | Moderate (Minor overshoot) | Good |
| **PCHIP** | High | **Excellent (Zero Overshoot)** | **Best (Recommended)** |

---

## Features & Implementation

1.  **Plug-and-Play**: Standalone `Interpolation.h` and `Interpolation.cpp`. No external dependencies (no Eigen, no GSL required).
2.  **Safety First**: Built-in boundary protection. The `generateCurve` function automatically clamps inputs to the data range, preventing "unreliable extrapolation."
3.  **Dual Interface**: Supports both **Single-Point Calculation** (get $y$ for a specific $x$) and **Curve Generation** (produce a vector of points for plotting).

## Quick Start (C++)

```cpp
#include "Interpolation.h"

// 1. Define your experimental dataset
DataSet myData(1.0, {0.1, 1.0, 10.0}, {40, 80, 120}, "BJT Data");

// 2. Calculate a single coordinate (e.g., what is hFE when Ic=5.0?)
double y = Interpolation::pchip(myData.X, myData.Y, 5.0);

// 3. Generate a smooth curve for plotting (100 sample points)
auto curve = Interpolation::generateCurve(myData, 0.1, 10.0, 100, InterpolationMethod::PCHIP);
