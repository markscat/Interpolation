#ifndef CAUVERDRAWABLE_H
#define CAUVERDRAWABLE_H


#include <vector>
#include <QString>

struct Point;



// 純抽象類別，定義「可畫曲線」的行為
class CurveDrawable {
public:
    virtual ~CurveDrawable() = default;

    // 根據輸入參數（Vgs 或 Ib）產生輸出曲線
    virtual std::vector<Point> generateCurve(double inputParam) const = 0;

    // 取得這個元件的類型名稱（給 UI 顯示用）
    virtual QString deviceType() const = 0;

    // 取得輸入參數的單位（給 UI 顯示用）
    virtual QString inputUnit() const = 0;


};

#endif // CAUVERDRAWABLE_H
