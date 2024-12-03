// MainPage.h
#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "lvgl.h"

// 主页面的初始化函数
void initMainPage();

// 主页面的事件处理函数
void main_event_handler(lv_event_t *e);
void gps_task_handler(lv_timer_t *task);
extern lv_obj_t *main_screen;  // 外部声明主页面对象

#endif // MAINPAGE_H
