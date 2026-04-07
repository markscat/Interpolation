#pragma once
//Constants.h

namespace CalcConstants {
    // 數學常數 (手動定義高精度數值)
    static constexpr double PI = 3.14159265358979323846;
    static constexpr double expr = 2.71828182845904523536;
    static constexpr double SQRT2 = 1.41421356237309504880;

    // 物理常數 (依據國際標準單位)
    namespace Physics {
        static constexpr double Planck = 6.62607015e-34;   // 普朗克常數 (J·s)
        static constexpr double LightSpeed = 299792458.0; // 光速 (m/s)
        static constexpr double Gravity = 6.67430e-11;    // 萬有引力常數 (N·m²/kg²)
        static constexpr double ElementaryCharge = 1.602176634e-19; // 基本電荷 (C)

    }
    
    namespace Electronics{
        static constexpr double Electric_charge=1.602e-19;  //電子電荷(q)單位：庫倫
        static constexpr double ElectronMobility=1350;      //電子遷移率(μn),單位cm²/V·s
        static constexpr double HoldMobility=480;           //電洞遷移率(μn),單位cm²/V·s
        static constexpr double Botzmann=1.381e-23;         //波茲曼常數(k),單位：J/K
        static constexpr double Vacuum_dc=8.854e-14;        //真空介電係數(ε0​),單位：F/cm
        static constexpr double Silicon_dc =1.04e-12;       //矽介電係數(εsi),11.7ε0,單位：F/cm
        static constexpr double Sio2_dc=3.45e-13;           //氧化層界電係數(εax),3.9ε0,單位：F/cm
    }


// 化學常數 (依據 SI 標準)
    namespace Chemistry {
        static constexpr double Avogadro = 6.02214076e23;     // 阿伏伽德羅常數 (mol⁻¹)
        static constexpr double GasConstant = 8.314462618;    // 理想氣體常數 (J/(mol·K))
        static constexpr double Boltzmann = 1.380649e-23;     // 玻爾茲曼常數 (J/K)
        static constexpr double Faraday = 96485.33212;       // 法拉第常數 (C/mol)
        static constexpr double AtomicMassUnit = 1.66053906660e-27; // 原子質量單位 (kg)
    }


// 統計學與機率論常用常數
    namespace Statistics {
        // 歐拉-馬斯刻若尼常數 (Euler-Mascheroni constant)，常用於分佈計算
        static constexpr double EulerGamma = 0.57721566490153286060;

        // 標準常態分佈常用值 (1 / sqrt(2 * PI))
        static constexpr double InvSqrt2PI = 0.39894228040143267794;

        // 自然對數常用值 (對數轉換、半衰期或機率分佈轉換常用)
        static constexpr double LN2 = 0.693147180559945309417;
        static constexpr double LN10 = 2.30258509299404568402;
    }
}
