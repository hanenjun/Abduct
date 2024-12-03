#include "Display.h"
#include "./pages/MainPage.h"  // 引入主页面头文件

void setup()
{
    Serial.begin(115200);  // 初始化串口，设置波特率
    lcd.initDisplay(); // 初始化显示
    lcd.initLVGL();    // 初始化 LVGL

    // 初始化主页面
    initMainPage();
}

void loop()
{
    lv_task_handler(); // 处理LVGL任务，更新GUI
    lv_tick_inc(5);    // 增加LVGL的时钟，保持定时器正常
    delay(5);          // 延时5毫秒
}
