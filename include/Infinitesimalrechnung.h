#pragma once
//#include "algorithm.h"
#include <string>
class Calculus; // 前置宣告，讓 Calculator 類別知道 Calculus 的存在

//#include "Calculator.h" // 包含 Calculator 的定義，讓 Calculus 可以使用它

class Calculus {
public:
       

    // 傳入已經存在的計算機引擎
    /**
    * @brief 建構函式：接受一個 Calculator 類別的引用，讓微積分模組可以直接使用計算機的功能。
     * @param engine 已經存在的 Calculator 實例，微積分模組將利用它來解析與計算表達式。
     * 這樣設計的好處是:
	 * 1. 不需要在 Calculus 類別內部重複實現解析與計算邏輯，減少代碼重複。
    */
    //Calculus(Calculator& engine) : calc(engine) {}

    // 數值微分：計算 f(x) 在 x 點的導數
    /**
	* @brief 計算 f(x) 在 x 點的導數 (數值微分)
	* @param expr 代表函數 f(x) 的字串表達式 (例如 "sin(x) + x^2")
	* @param x 導數要計算的點
    */
    
    double derivative(double (*f)(double), double x, double h = 1e-7);
    
    
    /**
	* @brief 計算 f(x) 在 [a, b] 區間的定積分
	* @param expr 代表函數 f(x) 的字串表達式 (例如 "sin(x) + x^2")
    */
    // 數值積分：計算 f(x) 在 [a, b] 區間的定積分 (使用辛普森準則)

    double integral(double (*f)(double), double a, double b, int n = 1000);

    
private:
    /**
    * @param calc 這裡使用引用來接收外部的 Calculator 實例，讓微積分模組可以直接利用計算機的解析與計算功能。
     * 這樣設計的好處是:
     * 1. 不需要在 Calculus 類別內部重複實現解析與計算邏輯，減少代碼重複。
	 * 2. 可以直接使用 Calculator 的變數設定功能 (setVariable)，方便在微積分計算過程中動態改變 x 的值。
    */
    //Calculator& calc;


    /**
    * 語法double (*f)(double),
    * 
     double square(double x) {
        return x * x;
    }

    // 一個普通的函數
    double cube(double x) {
        return x * x * x;
    }

int main() {
    double (*f)(double);  // 宣告一個函數指標 f
    
    f = square;           // 讓 f 指向 square
    double a = f(3);      // a = 9
    
    f = cube;             // 讓 f 指向 cube
    double b = f(3);      // b = 27
}
    
    */
};
