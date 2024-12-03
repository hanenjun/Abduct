#ifndef LGFX_CONFIG_H
#define LGFX_CONFIG_H

#include <LovyanGFX.hpp>
#include <lvgl.h>  // 引入LVGL库，用于创建图形界面

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;         // 使用 ILI9341 驱动的屏幕
    lgfx::Bus_SPI _bus_instance;                // SPI 总线
    lgfx::Touch_FT5x06 _touch_instance;         // 使用 FT6336 的触摸屏

public:
    LGFX(); // 构造函数声明

    // 外部需要调用的初始化方法
    void initDisplay();
    void initLVGL();
    static void touchpad_read(lv_indev_t *indev, lv_indev_data_t *data);
};

extern LGFX lcd;

#endif // LGFX_CONFIG_H
