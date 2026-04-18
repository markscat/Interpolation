/*
Infinitesimalrechnung 德文 無窮小的計算;翻譯成中文則是：微積分
*/
//#include "Infinitesimalrechnung.h"
#include "..\include\Infinitesimalrechnung.h" 

// --- 數值微分 (Central Difference) ---
/**
* @brief 計算 f(x) 在 x 點的導數 (數值微分)
 * @param expr 代表函數 f(x) 的字串表達式 (例如 "sin(x) + x^2")
 * @param x 導數要計算的點
 * @return 包含導數值與狀態的結構體
 */


double Calculus::derivative(double (*f)(double), double x, double h) {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}


double Calculus::integral(double (*f)(double), double a, double b, int n) {

    if (n % 2 != 0) n++;
    double h = (b - a) / n;
    double sum = f(a) + f(b);
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0) sum += 2.0 * f(x);
        else sum += 4.0 * f(x);
    }
    return (h / 3.0) * sum;
}



#if old


double derivative(const std::string& expr, double x) {

    
    /**
    * @brief 這裡使用中心差分法 (Central Difference) 來計算導數，公式為:
    *  h = 微分步長 (通常選擇一個非常小的數值，例如 1e-7)
    */
    double h = 1e-7; // 微分步長

    // f(x + h)
    calc.setVariable("x", x + h);
    auto resPlus = calc.executePostfix(postfix);
    if (resPlus.status != Calculator::CalcStatus::Success) return resPlus;

    // f(x - h)
    calc.setVariable("x", x - h);
    auto resMinus = calc.executePostfix(postfix);
    if (resMinus.status != Calculator::CalcStatus::Success) return resMinus;

    // (f(x+h) - f(x-h)) / 2h
    return { (resPlus.value - resMinus.value) / (2.0 * h), Calculator::CalcStatus::Success };
}



/**
    @brief 計算 f(x) 在 [a, b] 區間的定積分 (數值積分)
    @param expr 代表函數 f(x) 的字串表達式 (例如 "sin(x) + x^2")
    @param a 積分下限
    @param b 積分上限
    @param n 區間數 (越大越精確，但計算量也越大，預設為 1000)
    @return 包含積分值與狀態的結構體
    @note 這裡使用辛普森準則 (Simpson's Rule) 來計算定積分，公式為:
    *  h = (b - a) / n
    *
*/
// --- 數值積分 (Simpson's 1/3 Rule) ---
Calculator::CalcResult Calculus::integral(const std::string& expr, double a, double b, int n) {
    if (n % 2 != 0) n++; // 辛普森準則要求區間數必須是偶數

    auto postfix = calc.parseToPostfix(expr);
    double h = (b - a) / n;
    double sum = 0.0;

    // 計算端點 f(a) 與 f(b)
    calc.setVariable("x", a);
    auto resA = calc.executePostfix(postfix);
    if (resA.status != Calculator::CalcStatus::Success) return resA;

    calc.setVariable("x", b);
    auto resB = calc.executePostfix(postfix);
    if (resB.status != Calculator::CalcStatus::Success) return resB;

    sum = resA.value + resB.value;

    // 辛普森公式： (h/3) * [f(x0) + 4f(x1) + 2f(x2) + 4f(x3) + ... + f(xn)]
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        calc.setVariable("x", x);
        auto res = calc.executePostfix(postfix);
        if (res.status != Calculator::CalcStatus::Success) return res;

        if (i % 2 == 0) sum += 2.0 * res.value; // 偶數項係數 2
        else sum += 4.0 * res.value;           // 奇數項係數 4
    }

    return { (h / 3.0) * sum, Calculator::CalcStatus::Success };
}
#endif