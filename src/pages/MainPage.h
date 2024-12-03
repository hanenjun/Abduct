#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "lvgl.h"
#include "GPSModule.h"
#include "WiFi.h"
#include "BLEDevice.h"

// 主页面对象
extern lv_obj_t *main_screen;

// GPS 模块对象
extern GPSModule gps;

// 状态栏的对象
extern lv_obj_t *status_bar;
extern lv_obj_t *gps_label, *time_label, *wifi_label, *bluetooth_label, *battery_label, *mem_label;

// 面板和按钮区
extern lv_obj_t *left_panel, *right_panel;
extern lv_obj_t *btn_wifi, *btn_bluetooth, *btn_nfc, *btn_rfid, *btn_infrared, *btn_rf_244, *btn_rf_433;
extern lv_obj_t *mem_status_label, *wifi_status_label, *bt_status_label, *battery_status_label, *rf_244_status_label, *rf_433_status_label;

// 更新状态
void updateStatus();

// 通用函数：创建标签
lv_obj_t* createLabel(lv_obj_t* parent, const char* text, lv_align_t align, lv_coord_t x, lv_coord_t y);

// 通用函数：创建按钮并绑定事件
lv_obj_t* createButton(lv_obj_t* parent, const char* label_text, lv_event_cb_t event_handler, void* user_data, lv_align_t align, lv_coord_t x, lv_coord_t y);

// 更新页面状态的定时器任务
void status_update_task_handler(lv_timer_t* task);

// 初始化状态栏
void initStatusBar(lv_obj_t *main_screen);
// 初始化左侧按钮区
void initLeftPanel();
// 初始化右侧设备信息区
void initRightPanel();
// 主页面事件处理函数
void main_event_handler(lv_event_t* e);

// 初始化主页面
void initMainPage();

#endif // MAINPAGE_H
