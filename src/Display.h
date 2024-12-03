#ifndef LGFX_CONFIG_H
#define LGFX_CONFIG_H

#include <LovyanGFX.hpp>
#include <lvgl.h>  // 引入LVGL库，用于创建图形界面

// LGFX 类继承自 LovyanGFX 的设备类
class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;   // 使用 ILI9341 驱动的屏幕
    lgfx::Bus_SPI _bus_instance;           // SPI 总线
    lgfx::Touch_FT5x06 _touch_instance;    // 使用 FT6336 的触摸屏
public:
    LGFX();  // 构造函数声明

    // 初始化显示屏的方法
    void initDisplay(); 
    // 初始化LVGL图形库的方法
    void initLVGL(); 
    // 触摸屏读取回调函数
    static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data); 

    // 可选的资源清理方法
    void deinitLVGL(); // 清理LVGL的相关资源
};

extern LGFX lcd;  // 外部引用一个 LGFX 类对象

#endif // LGFX_CONFIG_H
