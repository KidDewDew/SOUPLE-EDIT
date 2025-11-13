#ifndef TT_STR_H
#define TT_STR_H

#include <type_traits>
//#include "TT_Str_Macro.h"
#include <array>

#define $$(str) <TStr(str)>

#define TStr(str) string_to_ttstr(str)

template <std::size_t a,std::size_t b>
static constexpr std::size_t tt_min = std::conditional<(a<b),
        std::integral_constant<std::size_t,a>,std::integral_constant<std::size_t,b>>::type::value;



template<std::size_t N>
constexpr auto string_to_ttstr(const char(&str)[N]);


template<std::size_t N0>
struct TT_Str {
    //t_tuple str;

    const std::array<char, N0> str;

    constexpr TT_Str(std::array<char, N0> ttstr):str(ttstr){}

    template<std::size_t N>
    constexpr int getIntArg(const char (&arg_name)[N], int default_value = 0) const { //获取整数参数
        auto arg = TStr(arg_name);
        int arg_i = findSubStrWord(arg);
        //constexpr int arg_i = sizeof(arg.str);
        if (arg_i == -1) return default_value;
        else {
            //读取整数
            int i = skipSpacing(arg_i + sizeof(arg.str));
            if (i == sizeof(str)) {
                static_assert(true, "getIntArg Error: Expect '=' after an interger arg !");
            }
            else {
                if (str[i] != '=') {
                    static_assert(true, "getIntArg Error: Expect '=' after an interger arg !");
                }
                return readInteger(skipSpacing(i+1));
            }
            return default_value;
        }
    }

    template<std::size_t N>
    constexpr bool getBoolArg(const char(&arg_name)[N], bool default_value = false) const { //获取bool参数
        auto arg = TStr(arg_name);
        int arg_i = findSubStrWord(arg);
        if (arg_i == -1) return default_value;
        else {
            //读取BOOL
            int i = skipSpacing(arg_i + sizeof(arg.str));
            if (i == sizeof(str)) {
                static_assert(true, "getBoolArg Error: Expect '=' after an bool arg !");
            }
            else {
                if (str[i] != '=') {
                    static_assert(true, "getBoolArg Error: Expect '=' after an bool arg !");
                }
                return readBool(skipSpacing(i+1));
            }
            return default_value;
        }
    }



    template<std::size_t N>
    constexpr int findSubStrWord(TT_Str<N> arg) const { //寻找子字符串单词
        //if constexpr(sizeof...(substr) != sizeof...(str)) return -1; //长度不一样
        return imp_findSubStrWord(findWordBeginIndex(0), 0, arg);
    }

public:

    constexpr int findWordBeginIndex(int i) const { //找到从索引i(包含i)开始第一个单词首字母的索引
        if (i == sizeof(str)) return sizeof(str);
        else {
            char c = str[i];
            if ((i == 0 || isCharWordEnd(i - 1))
                && (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c == '_')
                ) return i; //上一个字符是分隔符且该字符是单词首字符
            else return findWordBeginIndex(i + 1);
        }
    }

    constexpr int skipSpacing(int i) const { //跳过空格
        if (i == sizeof(str)) return sizeof(str);
        else {
            char c = str[i];
            if (c != ' ') return i; //不是空格
            else return skipSpacing(i + 1);
        }
    }

    constexpr bool isCharWordEnd(int i) const { //索引i处字符是否是单词分隔(前提：i-1处属于单词)
        if (i >= sizeof(str)) return true;
        else {
            char c = str[i];
            if (c == ' ' || c == '=' || c == ',') return true;
            else return false;
        }
    }

    template<std::size_t N>
    constexpr int imp_findSubStrWord(int i, int i2, TT_Str<N> arg) const {
        if (i2 == sizeof(arg.str)) {
            if (isCharWordEnd(i)) // 单词完全匹配
                return i - sizeof(arg.str); // found 返回子字符串索引
            else return imp_findSubStrWord(findWordBeginIndex(i + 1), 0,arg); //单词前缀匹配，~未匹配
        }
        else {
            if (i == sizeof(str)) return -1; //找到末尾都没找到
            else {
                if (str[i] == arg.str[i2]) {
                    return imp_findSubStrWord(i+1,i2+1,arg);
                }
                else {
                    return imp_findSubStrWord(findWordBeginIndex(i+1), 0,arg);
                }
            }
        }
    }

    constexpr int readInteger(int i, bool neg = false, int value = 0) const {
        if (i == sizeof(str)) {
            return value;
        }
        else {
            char c = str[i];
            if (c == '-') {
                return readInteger(i + 1, true, 0);
            }
            else {
                if (c >= '0' && c <= '9')
                    return readInteger(i + 1, neg, value * 10 + c - '0');
                else return value;
            }
        }
    }

    constexpr int readBool(int i, int j = 0, bool isTrue = false, bool isFalse = false) const {
        if (i == sizeof(str)) {
            static_assert(true, "readBool Error: Invalid value."); //bool值不完整
        }
        else {
            char c = str[i];
            if (isTrue) {
                if (j == 1) {
                    if (c == 'r' || c == 'R') return readBool(i + 1, 2, true, false);
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
                else if (j == 2) {
                    if (c == 'u' || c == 'U') return readBool(i + 1, 3, true, false);
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
                else if (j == 3) {
                    if (c == 'e' || c == 'E') return true;
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
            }
            else if (isFalse) {
                if (j == 1) {
                    if (c == 'a' || c == 'A') return readBool(i + 1, 2, false, true);
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
                else if (j == 2) {
                    if (c == 'l' || c == 'L') return readBool(i + 1, 3, false, true);
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
                else if (j == 3) {
                    if (c == 's' || c == 'S') return readBool(i + 1, 4, false, true);
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
                else if (j == 4) {
                    if (c == 'e' || c == 'E') return false;
                    else static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
            }
            else {
                if  (c == 't' || c == 'T') return readBool(i + 1, 1, true, false);
                else if(c == 'f' || c == 'F') return readBool(i + 1, 1, false, true);
                else {
                    static_assert(true, "readBool Error: Invalid value."); //bool值不完整
                }
            }
        }
    }
};

template<std::size_t N>
constexpr auto string_to_ttstr(const char(&str)[N]) {
    return[]<std::size_t... Is>(std::index_sequence<Is...>, const char(&s)[N]) {
        return TT_Str(std::array<char,N-1>{ s[Is]... });
    }(std::make_index_sequence<N - 1>{}, str); // N-1 排除null终止符
}

#endif // TT_STR_H
