// algorithm.h

#pragma once
#include <cmath> // for std::isfinite in implementations if needed (header-only include is harmless but optional)
#include <vector>
#include <string>
#include <functional> // 必須包含，用來儲存函式動作
#include <unordered_map> 
#include "Constants.h"
#include "Trigonometry.h"

using DoubleList = std::vector<double>;
using CharList = std::vector<char>;

class Calculator {
public:
    //bool isLogging = false; // 由 UI 控制這個開關


    // 定義錯誤類型，方便 UI 顯示錯誤訊息
    enum class CalcStatus {
        Success = 0,
        DivideByZero,    // 除以零
        NegativeRoot,    // 負數開根號 (若未來擴充)
        InvalidOperator, // 無效的符號
        Overflow,        // 數字太大或非有限數
        UnknownError     // 未知錯誤
    }; // <--- 這裡加分號

    // 定義一個結構來同時攜帶「數值」與「狀態」
    struct CalcResult {
        double value;       // 計算結果
        CalcStatus status;  // 狀態碼
    }; // <--- 這裡加分號


    // 在 Calculator 類別內定義
    /**
    * @param Number 類型的 Token，代表數字 (如 10.5)
    * @param Operator 類型的 Token，代表運算符 (如 +, -, *, /)
    * @param Parenthesis 類型的 Token，代表括號 (如 (, ) )
    * @param Function 類型的 Token，代表函數 (如 abs, log, sin)
    * @param Constant 類型的 Token，代表常數 (如 PI, E)
    */
    enum class TokenType {
        Number,         // 數字 (如 10.5)
        Operator,       // 運算符 (如 +, -, *, /)
        Parenthesis,    // 括號 (如 (, ) )
        Function,       // 函數 (如 abs, log, sin)
        Constant        //常數 (如 PI, E)
    };
    /**
    * @brief Token 結構體：用來表示分詞後的基本單位
    * @param type Token 的類型 (數字、符號、括號)
    * @param vaule 如果是數字，存數值；否則為 0
    * @param symbol 如果是符號，存字元；否則為 '\0'
    * @param precedence 優先級：數字為0, + -為1, * /為2, ^ s為3
    * @param name 如果是單字 (abs, log, sin, PI)，存名稱；否則為空字串
    */
    struct Token {
        TokenType type;
        double value;    // 如果是數字，存數值
        char symbol;     // 如果是符號，存字元
        int precedence; // 優先級：數字為0, + -為1, * /為2, ^ s為3
        std::string name; // 新增：用於單字 (abs, log, sin, PI)
    };

    // 這是未來對外的統一入口：輸入字串，得到結果
    CalcResult execute(std::string expression) noexcept;

    Calculator() noexcept; // 構造函數


    /**
      * 核心計算函式
      * @param a 第一個數字
      * @param op 運算符號 ('+', '-', '*', '/','^','s')
      * @param b 第二個數字
      * @return 包含結果與狀態的結構體
      */
    CalcResult calculate(double a, char op, double b) noexcept;

    /**
      * 靜態工具函式：將列舉轉為文字 (方便 UI 顯示)
      */
    static const char* statusToText(CalcStatus s) noexcept;

    CalcStatus getLastStatus() const noexcept;


    // 新增：處理連續運算
    // numbers: 數字列表 (例如 {10, 5, 2})
    // ops: 運算符列表 (例如 {'-', '-'})
    //CalcResult calculateSequence(std::vector<double> numbers, std::vector<char> ops) noexcept;

    CalcResult calculateSequence(DoubleList numbers, CharList ops) noexcept;


    std::pair<long long, long long> fibPair(long long n);
    long long fibonacci(long long n);
    std::vector<long long> fibonacciSeries(long long n);


    // 取得解析後的後序表達式 (RPN)，供微積分重複使用
    std::vector<Token> parseToPostfix(std::string expression) noexcept;

    // 直接執行後序表達式 (跳過分詞與重排)
    CalcResult executePostfix(const std::vector<Token>& postfix) noexcept;

    // 設定變數接口
    void setVariable(const std::string& name, double value) noexcept;

    // 清除所有變數 (選用，通常在重設計算機時使用)
    void clearVariables() noexcept;

    /**@brief startLogging 啟動日誌紀錄功能
     * 這個函式的設計是提供一個簡單的介面讓使用者可以啟動計算日誌的存檔功能。
     * 當使用者按下按鈕時，會呼叫這個函式來產生一個新的檔名（例如 "calc_log_20240630_153000.txt"），
	 * 並開啟一個新的檔案來紀錄後續的每筆計算。每次計算都會把輸入表達式、後序表達式 (RPN) 和結果寫入這個檔案。
    */
    void startLogging(); // 使用者按下按鈕：產生檔名，開啟紀錄

    /**@brief stopLogging 停止日誌紀錄功能
     * 這個函式的設計是提供一個簡單的介面讓使用者可以停止計算日誌的存檔功能。
     * 當使用者放開按鈕時，會呼叫這個函式來關閉目前開啟的日誌檔案，並將 isLogging 開關設回 false，停止後續的紀錄。
     * 這樣使用者就可以透過按住按鈕來控制何時開始與停止紀錄計算日誌。
     * 注意：在實作中，你需要確保在 startLogging 中成功開啟檔案後，才允許 stopLogging 來關閉它，以避免錯誤。
     * 另外，如果使用者在沒有啟動日誌功能的情況下呼叫 stopLogging，你也應該妥善處理這種情況（例如忽略或顯示警告）。
	 * 總之，這兩個函式共同提供了一個簡單的機制，讓使用者可以透過按鈕來控制計算日誌的存檔行為。
    */
    void stopLogging();  // 使用者放開按鈕：關閉紀錄功能


    // 新增這個函數，讓外部可以設定角度單位
    void setAngleUnit(TrigEngine::AngleUnit unit) {
        trig.setUnit(unit);
    }


    // 如果外部真的需要「讀取」目前是否在錄製，可以給一個唯讀的 Getter
    bool getLoggingStatus() const noexcept { return isLogging; }

private:
    // 階段一：切單字
    std::vector<Token> tokenize(const std::string expr) noexcept;

    // 階段二：重排語法 (中序轉後序)
    std::vector<Token> shunt(const std::vector<Token>& tokens) noexcept;

    // 階段三：理解並計算
    CalcResult evaluate(const std::vector<Token>& postfix) noexcept;

    // 輔助：取得符號權重
    int getPrecedence(char op) const noexcept;

    CalcStatus lastStatus = CalcStatus::Success;

    // 輔助函數：判斷運算符是否為右結合
    bool isRightAssociative(char op) const noexcept;

    // 第一階段的核心函數：把字串切成 Token 清單
    //std::vector<Token> tokenize(std::string& expr) noexcept;

    bool tryGetConstant(const std::string& name, double& outValue) noexcept;

    //bool tryExecuteFunction(const std::string& name, double x, double& outRes, CalcStatus& outStatus) const noexcept;
    bool tryExecuteFunction(const std::string& name, double x, double& outRes, Calculator::CalcStatus& outStatus) const noexcept;
    TrigEngine trig;


    Token scanNumber(const std::string& expr, size_t& i);
    Token scanWord(const std::string& expr, size_t& i, bool& expectOp);
    Token scanOperator(char ch, bool& expectOp);
    Token scanParenthesis(char ch, bool& expectOperand) noexcept;

	/**
	* @brief 變數表：用來存放使用者定義的變數名稱與對應的數值 (例如 x=5)，
    */
    std::unordered_map<std::string, double> variables;

    std::string currentSessionFile; // 紀錄當前會話的檔名
    bool isLogging = false; // 由 UI 控制這個開關

};
