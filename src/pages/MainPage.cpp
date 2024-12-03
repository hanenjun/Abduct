#include "MainPage.h"
#include "Display.h"
#include "GPSModule.h"
#include "WiFi.h"
#include "BLEDevice.h"

#include "../images/img_bluetooth_off.c"
#include "../images/img_bluetooth_on.c"
#include "../images/img_gps_connected.c"
#include "../images/img_gps_disconnected.c"
#include "../images/img_wifi_connected.c"
#include "../images/img_wifi_disconnected.c"
#include "../images/img_mem_low.c"
#include "../images/img_mem_normal.c"


lv_obj_t *main_screen; // 主页面对象
GPSModule gps;         // 创建GPS模块对象

// 声明状态栏和各个状态图标
lv_obj_t *status_bar;
lv_obj_t *gps_status_icon, *wifi_status_icon, *bluetooth_status_icon, *battery_status_icon, *mem_status_icon;
lv_obj_t *time_label;

// 面板和按钮区
lv_obj_t *left_panel, *right_panel;
lv_obj_t *btn_wifi, *btn_bluetooth, *btn_nfc, *btn_rfid, *btn_infrared, *btn_rf_244, *btn_rf_433;
lv_obj_t *mem_status_label, *wifi_status_label, *bt_status_label, *battery_status_label, *rf_244_status_label, *rf_433_status_label;


// 图标资源（假设你已经准备好了对应的图标文件）
LV_IMG_DECLARE(img_wifi_connected);
LV_IMG_DECLARE(img_wifi_disconnected);
LV_IMG_DECLARE(img_bluetooth_on);
LV_IMG_DECLARE(img_bluetooth_off);
LV_IMG_DECLARE(img_gps_connected);
LV_IMG_DECLARE(img_gps_disconnected);
LV_IMG_DECLARE(img_battery_full);
LV_IMG_DECLARE(img_battery_low);
LV_IMG_DECLARE(img_mem_normal);
LV_IMG_DECLARE(img_mem_low);


// 通用函数：创建按钮并绑定事件
lv_obj_t *createButton(lv_obj_t *parent, const char *label_text, lv_event_cb_t event_handler, void *user_data, lv_align_t align, lv_coord_t x, lv_coord_t y)
{
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_align(btn, align, x, y);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, user_data);

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, label_text);
    lv_obj_center(label);

    return btn;
}

// 更新页面状态的定时器任务
void status_update_task_handler(lv_timer_t *task)
{
    updateStatus();
}


// 创建状态图标的函数
lv_obj_t *createStatusIcon(lv_obj_t *parent, int x_offset, lv_obj_t **icon_obj)
{
    *icon_obj = lv_img_create(parent);
    lv_obj_align(*icon_obj, LV_ALIGN_CENTER, x_offset, 0);  // 设置图标位置
    return *icon_obj;
}

// 创建时间标签
lv_obj_t *createLabel(lv_obj_t *parent, const char *text, lv_align_t align, int x_offset, int y_offset)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_align(label, align, x_offset, y_offset);
    return label;
}

// 初始化状态栏
void initStatusBar(lv_obj_t *main_screen)
{
    status_bar = lv_obj_create(main_screen);  // 创建状态栏
    lv_obj_set_size(status_bar, LV_HOR_RES, 40);  // 设置状态栏尺寸
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0); // 设置位置
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x000000), LV_PART_MAIN);  // 设置背景颜色为黑色
    lv_obj_set_style_pad_all(status_bar, 5, LV_PART_MAIN);  // 设置内边距

    // 创建状态图标
    gps_status_icon = NULL;
    createStatusIcon(status_bar, 10, &gps_status_icon);

    wifi_status_icon = NULL;
    createStatusIcon(status_bar, 30, &wifi_status_icon);

    bluetooth_status_icon = NULL;
    createStatusIcon(status_bar, 50, &bluetooth_status_icon);

    battery_status_icon = NULL;
    createStatusIcon(status_bar, 70, &battery_status_icon);

    mem_status_icon = NULL;
    createStatusIcon(status_bar, 90, &mem_status_icon);

    // 创建时间标签，右对齐
    time_label = createLabel(status_bar, "Time", LV_ALIGN_CENTER, 110, 0);  // 时间显示
}

// 更新GPS状态
void updateGPSStatus()
{
    if (gps.isState) {
        lv_img_set_src(gps_status_icon, &img_gps_connected);  // 设置GPS连接图标
    } else {
        lv_img_set_src(gps_status_icon, &img_gps_disconnected);  // 设置GPS断开图标
    }
}

// 更新WiFi状态
void updateWiFiStatus()
{
    if (WiFi.isConnected()) {
        lv_img_set_src(wifi_status_icon, &img_wifi_connected);  // 设置Wi-Fi连接图标
    } else {
        lv_img_set_src(wifi_status_icon, &img_wifi_disconnected);  // 设置Wi-Fi断开图标
    }
}

// 更新蓝牙状态
void updateBluetoothStatus()
{
    if (BLEDevice::getInitialized()) {
        lv_img_set_src(bluetooth_status_icon, &img_bluetooth_on);  // 设置蓝牙开启图标
    } else {
        lv_img_set_src(bluetooth_status_icon, &img_bluetooth_off);  // 设置蓝牙关闭图标
    }
}

// 更新电池状态
void updateBatteryStatus()
{
    // int battery_level = analogRead(34);  // 假设电池电压连接在GPIO 34
    // int battery_percentage = map(battery_level, 0, 4095, 0, 100);
    int battery_percentage = 60;
    
    if (battery_percentage >= 50) {
        lv_img_set_src(battery_status_icon, &img_battery_full);  // 设置电池满图标
    } else {
        lv_img_set_src(battery_status_icon, &img_battery_low);  // 设置电池低图标
    }
}

// 更新内存状态
void updateMemoryStatus()
{
    int free_mem = ESP.getFreeHeap();  // 获取剩余内存
    if (free_mem < 10000) {  // 假设内存低于10KB为低
        lv_img_set_src(mem_status_icon, &img_mem_low);  // 设置内存低图标
    } else {
        lv_img_set_src(mem_status_icon, &img_mem_normal);  // 设置内存正常图标
    }
}

// 更新时间
void updateTimeStatus()
{
    String gps_time = gps.getGPSTimeInfo();  // 获取 GPS 时间字符串
    lv_label_set_text(time_label, gps_time.c_str());
}

// 更新状态信息的函数
void updateStatus()
{
    updateGPSStatus();
    updateWiFiStatus();
    updateBluetoothStatus();
    updateBatteryStatus();
    updateMemoryStatus();
    updateTimeStatus();
}
// 初始化左侧按钮区
void initLeftPanel()
{
    left_panel = lv_obj_create(main_screen);
    lv_obj_set_size(left_panel, LV_HOR_RES / 3, LV_VER_RES - 40);
    lv_obj_align(left_panel, LV_ALIGN_LEFT_MID, 0, 0);
    lv_obj_set_style_bg_color(left_panel, lv_color_hex(0x333333), LV_PART_MAIN);

    btn_wifi = createButton(left_panel, "WiFi", main_event_handler, (void *)"WiFi", LV_ALIGN_TOP_MID, 0, 10);
    btn_bluetooth = createButton(left_panel, "Bluetooth", main_event_handler, (void *)"Bluetooth", LV_ALIGN_TOP_MID, 0, 60);
    btn_nfc = createButton(left_panel, "NFC", main_event_handler, (void *)"NFC", LV_ALIGN_TOP_MID, 0, 110);
    btn_rfid = createButton(left_panel, "RFID", main_event_handler, (void *)"RFID", LV_ALIGN_TOP_MID, 0, 160);
    btn_infrared = createButton(left_panel, "Infrared", main_event_handler, (void *)"Infrared", LV_ALIGN_TOP_MID, 0, 210);
    btn_rf_244 = createButton(left_panel, "RF 244", main_event_handler, (void *)"RF_244", LV_ALIGN_TOP_MID, 0, 260);
    btn_rf_433 = createButton(left_panel, "RF 433", main_event_handler, (void *)"RF_433", LV_ALIGN_TOP_MID, 0, 310);
}

// 初始化右侧设备信息区
void initRightPanel()
{
    right_panel = lv_obj_create(main_screen);
    lv_obj_set_size(right_panel, LV_HOR_RES * 2 / 3, LV_VER_RES - 40);
    lv_obj_align(right_panel, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_color(right_panel, lv_color_hex(0x555555), LV_PART_MAIN);

    mem_status_label = createLabel(right_panel, "Free Memory: 128 KB", LV_ALIGN_TOP_LEFT, 10, 10);
    wifi_status_label = createLabel(right_panel, "WiFi: Connected", LV_ALIGN_TOP_LEFT, 10, 40);
    bt_status_label = createLabel(right_panel, "Bluetooth: On", LV_ALIGN_TOP_LEFT, 10, 70);
    battery_status_label = createLabel(right_panel, "Battery: 75%", LV_ALIGN_TOP_LEFT, 10, 100);
    rf_244_status_label = createLabel(right_panel, "RF 244: Active", LV_ALIGN_TOP_LEFT, 10, 130);
    rf_433_status_label = createLabel(right_panel, "RF 433: Inactive", LV_ALIGN_TOP_LEFT, 10, 160);
}

// 主页面事件处理函数
void main_event_handler(lv_event_t *e)
{
    const char *data = (const char *)lv_event_get_user_data(e);
    Serial.print("Button clicked: ");
    Serial.println(data);

    // 根据点击的按钮跳转到相应页面（例如WiFi、蓝牙、NFC等）
    // 在这里可以根据实际需求进行处理
}

// 初始化主页面
void initMainPage()
{
    main_screen = lv_obj_create(NULL); // 创建主页面
    if (!main_screen)
    {
        Serial.println("Failed to create main screen");
        return;
    }

    initStatusBar(main_screen);
    initLeftPanel();
    initRightPanel();

    // 创建定时器，每秒更新一次状态
    lv_timer_create(status_update_task_handler, 30000, NULL);

    lv_scr_load(main_screen); // 加载主页面
}
