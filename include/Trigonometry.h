// Trigonometry.h
#pragma once
#include "Constants.h"
#include <cmath>
#include <string>

class TrigEngine {
public:
    // 定義角度單位
    enum class AngleUnit {
        Degree,     // 度 (0-360)
        Radian,     // 弧度 (0-2PI)
        Gradian     // 百分度 (0-400)
    };

    TrigEngine() : currentUnit(AngleUnit::Degree) {}

    // 設定與取得目前單位
    void setUnit(AngleUnit unit) { currentUnit = unit; }
    AngleUnit getUnit() const { return currentUnit; }

    // 核心三角函數 (會根據目前單位自動轉換)
    double sin(double input) const;
    double cos(double input) const;
    double tan(double input) const;

    double cot(double input) const;
    double sec(double input) const;
    double csc(double input) const;

    // 反三角函數 (結果會根據目前單位輸出)
    double asin(double val) const;
    double acos(double val) const;
    double atan(double val) const;

    // 單位轉換工具 (靜態函式，方便外部直接調用)
    static double degToRad(double deg);
    static double radToDeg(double rad);
    static double degToGrad(double deg);
    static double gradToDeg(double grad);

private:
    AngleUnit currentUnit;

    // 內部輔助：將輸入值統一轉為「弧度」，因為 std::sin 只吃弧度
    double toRadians(double input) const;
    // 內部輔助：將弧度結果轉回「目前單位」
    double fromRadians(double rad) const;
};
