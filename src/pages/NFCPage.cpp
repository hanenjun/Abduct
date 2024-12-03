#include "NFCPage.h"
#include "lvgl.h"
#include "Display.h"
#include "NFCMode.h"
#include "MainPage.h" // 确保引入 MainPage.h

NFC nfc; // 创建NFC对象

// 常量定义，提升可维护性
#define SCAN_BUTTON_TEXT "Scan Card NFC"
#define WRITE_BUTTON_TEXT "Write Card NFC"
#define NONE_BUTTON_TEXT "None Card NFC"
#define BACK_BUTTON_TEXT "Back to Main"
#define STOP_BUTTON_TEXT "Stop Card NFC"
#define BUTTON_Y_OFFSET 50  // 按钮的垂直偏移量

// 创建按钮的通用函数
lv_obj_t* create_button(lv_obj_t *parent, const char *label_text, lv_event_cb_t event_cb, const char *user_data, lv_align_t align, int offset_y) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, (void *)user_data);
    lv_obj_align(btn, align, 0, offset_y);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK); // 去除按钮按下锁定标志

    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, label_text);
    lv_obj_center(label);

    return btn;
}

// NFC 状态更新的任务函数
void nfc_task_handler(lv_timer_t *task) {
    Serial.print(nfc.currentMode);
    // 轮询NFC状态
    if (nfc.currentMode != NFCMode::NONE) {
        if (nfc.currentMode == NFCMode::READ) {
            String cardData = nfc.readCard();
            if (cardData != "No card detected, waiting...\n") {
                Serial.println(cardData);
                // 如果读取到卡片数据，停止任务
                lv_timer_del(task);
            }
        } else if (nfc.currentMode == NFCMode::WRITE) {
            const char *cardData = "1";
            // 执行写卡操作
            if (nfc.writeCard(reinterpret_cast<const uint8_t *>(cardData), 1)) {
                Serial.println("Card written successfully.");
                // 如果写入成功，停止任务
                lv_timer_del(task);
            }
        }
    }
}

// NFC 页面事件处理函数
void nfc_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    const char *data = (const char *)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED) {
        if (data == "scan") {
            nfc.setReadMode();
        } else if (data == "write") {
            nfc.setWriteMode();
        } else if (data == "none") {
            nfc.setNoneMode();
        } else if (data == "stop") {
            Serial.print("Event stop ");
            nfc.setNoneMode();
        } else if (data == "back") {
            Serial.print("Event back ");
            // 返回主页面
            lv_scr_load(main_screen); // 加载主页面
        }
    }
}

// NFC 页面初始化
void initNFCPage() {
    nfc.initNFC();
    lv_obj_t *lcd_obj = lv_screen_active(); // 获取当前活动屏幕

    // 创建按钮
    create_button(lcd_obj, SCAN_BUTTON_TEXT, nfc_event_handler, "scan", LV_ALIGN_CENTER, 0);
    create_button(lcd_obj, WRITE_BUTTON_TEXT, nfc_event_handler, "write", LV_ALIGN_CENTER, BUTTON_Y_OFFSET);
    create_button(lcd_obj, NONE_BUTTON_TEXT, nfc_event_handler, "none", LV_ALIGN_CENTER, BUTTON_Y_OFFSET * 2);
    create_button(lcd_obj, BACK_BUTTON_TEXT, nfc_event_handler, "back", LV_ALIGN_BOTTOM_MID, 150);
    create_button(lcd_obj, STOP_BUTTON_TEXT, nfc_event_handler, "stop", LV_ALIGN_BOTTOM_MID, 200);

    // 创建LVGL任务，每1000毫秒调用一次nfc_task_handler
    lv_timer_create(nfc_task_handler, 1000, NULL); // 创建定时器，1000ms执行一次nfc_task_handler
}
