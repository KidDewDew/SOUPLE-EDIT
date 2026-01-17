#ifndef HELPER_WEB_H
#define HELPER_WEB_H
#include <string>
#include <bit>

namespace souple_web {
    inline std::string concatStrAndBinary(const std::string& str,char* binary_data,int binary_data_len) {
        std::string result = "....";

        if constexpr(std::endian::native == std::endian::little) {
            //小端序 -> 大端序
            result[0] = (str.length() & 0xFF000000) >> 24;
            result[1] = (str.length() & 0xFF0000) >> 16;
            result[2] = (str.length() & 0xFF00) >> 8;
            result[3] = (str.length() & 0xFF);
        } else {
            //大端序 -> 大端序，直接赋值即可。
            *((int*)(&result[0])) = (int)str.length();
        }
        result.append(str);
        if(binary_data_len > 0) {
            result.append(binary_data,binary_data_len);
        }
        return std::move(result);
    }
}

#endif // HELPER_WEB_H
