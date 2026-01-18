#ifndef CONFIG_H
#define CONFIG_H

class Config {
public:
    //初始化配置类
    static void initConfig();
    static inline constexpr int WORKER_THREAD_NUM = 8;
private:
};

#endif // CONFIG_H
