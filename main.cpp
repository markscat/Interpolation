#include "include/Matrix.h"

int main() {
    // 準備兩組 5x5 的資料（這裡完全沒有迴圈，直接寫死）
    double dataA[] = { 12,45,78,23,56, 89,34,11,67,90, 22,55,88,33,66, 44,77,99,10,25, 15,35,45,65,75 };
    double dataB[] = { 98,76,54,32,10, 13,31,57,75,99, 24,42,68,86,11, 35,53,79,97,22, 46,64,80,02,33 };

    Matrix A(5, 5), B(5, 5);

    A.load(dataA); // 一次倒進去
    B.load(dataB); // 一次倒進去

    std::cout << "矩陣 A:\n"; A.print();
    std::cout << "\n矩陣 B:\n"; B.print();
    std::cout << "\n結果 A * B:\n"; A.multiply(B).print();

    return 0;
}