// Trigonometry.cpp
#include "..\include\Trigonometry.h"

using namespace CalcConstants;

// --- 單位轉換實作 ---
double TrigEngine::degToRad(double deg) { return deg * (PI / 180.0); }
double TrigEngine::radToDeg(double rad) { return rad * (180.0 / PI); }
double TrigEngine::degToGrad(double deg) { return deg * (400.0 / 360.0); }
double TrigEngine::gradToDeg(double grad) { return grad * (360.0 / 400.0); }

double TrigEngine::toRadians(double input) const {
    switch (currentUnit) {
    case AngleUnit::Degree:  return degToRad(input);
    case AngleUnit::Gradian: return degToRad(gradToDeg(input));
    case AngleUnit::Radian:
    default:                 return input;
    }
}

double TrigEngine::fromRadians(double rad) const {
    switch (currentUnit) {
    case AngleUnit::Degree:  return radToDeg(rad);
    case AngleUnit::Gradian: return degToGrad(radToDeg(rad));
    case AngleUnit::Radian:
    default:                 return rad;
    }
}

// --- 三角函數實作 ---
double TrigEngine::sin(double input) const {
    return std::sin(toRadians(input));
}

double TrigEngine::cos(double input) const {
    // 處理 90 度的微小誤差問題 (std::cos(PI/2) 可能不精準等於 0)
    double res = std::cos(toRadians(input));
    return (std::abs(res) < 1e-15) ? 0.0 : res;
}

double TrigEngine::tan(double input) const {
    return std::tan(toRadians(input));
}

// 餘切 (cot)
double TrigEngine::cot(double input) const {
    double val = std::tan(toRadians(input));
    // 防止除以零 (例如 tan(0) = 0)
    if (std::abs(val) < 1e-15) return INFINITY;
    return 1.0 / val;
}

// 正割 (sec)
double TrigEngine::sec(double input) const {
    double val = std::cos(toRadians(input));
    // 防止除以零 (例如 cos(90) = 0)
    if (std::abs(val) < 1e-15) return INFINITY;
    return 1.0 / val;
}

// 餘割 (csc)
double TrigEngine::csc(double input) const {
    double val = std::sin(toRadians(input));
    // 防止除以零 (例如 sin(0) = 0)
    if (std::abs(val) < 1e-15) return INFINITY;
    return 1.0 / val;
}



double TrigEngine::asin(double val) const {
    return fromRadians(std::asin(val));
}

double TrigEngine::acos(double val) const {
    return fromRadians(std::acos(val));
}

double TrigEngine::atan(double val) const {
    return fromRadians(std::atan(val));
}

