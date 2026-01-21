#ifndef HELPER_WEB_H
#define HELPER_WEB_H
#include <string>
#include <bit>
#include <random>
#include <chrono>

namespace souple_web {
    inline std::string concatStrAndBinary(const std::string& str,char* binary_data,int binary_data_len) {
        std::string result = "....";

        if constexpr(std::endian::native == std::endian::little) {
            //小端序 -> 大端序
            result[0] = (char) ((str.length() >> 24) & 0xFF);  // 最高字节
            result[1] = (char) ((str.length() >> 16) & 0xFF);  // 次高字节
            result[2] = (char) ((str.length() >> 8) & 0xFF);   // 次低字节
            result[3] = (char) (str.length() & 0xFF);          // 最低字节
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
    //生成一个随机标识符
    inline std::string generateUID() {
        static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                     "1234567890";
        auto now = std::chrono::system_clock::now();
        auto seed = now.time_since_epoch();
        //该随机数产生器，线程安全。
        std::mt19937 gen(seed.count());
        std::uniform_int_distribution<int> dist(0, charset.length()-1);
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(seed);
        std::string random_str = "";
        for(unsigned short i = 0; i < 6; ++i)
            random_str += charset[dist(gen)];
        return std::format("{}{}",seed,random_str);
    }
}

#endif // HELPER_WEB_H
