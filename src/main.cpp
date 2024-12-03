#include "Display.h"
#include "./pages/MainPage.h"  // 引入主页面头文件

// 初始化显示和图形库
void initDisplayAndLVGL()
{
    lcd.initDisplay();  // 初始化显示
    lcd.initLVGL();     // 初始化 LVGL
}

// 初始化程序的各个模块
void initSystem()
{
    Serial.begin(115200);  // 初始化串口，设置波特率
    initDisplayAndLVGL();   // 初始化显示与LVGL
    initMainPage();         // 初始化主页面
}

// 主循环，处理GUI和定时器任务
void mainLoop()
{
    lv_task_handler();  // 处理LVGL任务，更新GUI
    lv_tick_inc(5);     // 增加LVGL的时钟，保持定时器正常
    delay(5);           // 延时5毫秒，避免过多占用CPU资源
}

void setup()
{
    initSystem();  // 初始化系统的各个部分
}

void loop()
{
    mainLoop();  // 运行主循环
}
