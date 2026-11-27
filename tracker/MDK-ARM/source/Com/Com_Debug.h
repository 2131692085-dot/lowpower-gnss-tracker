#ifndef __COM_DEBUG_H__
#define __COM_DEBUG_H__

#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

#define DEBUG_ENABLE

#ifdef DEBUG_ENABLE
    // __VA_ARGS__可以代替可变参数...
    // sprintf => "[abc]" format
    // [%s:%d]%d xxxxx
    // __FILE__获取到的是文件路径，可以对路径进行截断操作，只获取文件名即可

    // strrchr => string(str) + right(r) + char(chr) : 从字符串的右边查找指定的字符
    //           [source\App\App_Main.c:8]10 xxxxx
    //           返回查找到的第一个字符的位置(指针),在使用时，会将指定位置的字符串截取到最后

    //           
    #define FILE_NAME strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__
    #define Com_Debug_print(format, ...) printf("[%s:%d]" format, FILE_NAME, __LINE__, ##__VA_ARGS__)
    #define Com_Debug_println(format, ...) printf("[%s:%d]" format "\n", FILE_NAME, __LINE__, ##__VA_ARGS__)
#else
    #define Com_Debug_print(format, ...)
    #define Com_Debug_println(format, ...)
#endif

#endif /* __COM_DEBUG_H__ */
