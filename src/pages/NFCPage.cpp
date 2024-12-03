#include "NFCPage.h"
#include "lvgl.h"
#include "Display.h"
#include "NFCMode.h"
#include "MainPage.h" // 确保引入 MainPage.h

NFC nfc; // 创建NFC对象
// NFC 状态更新的任务函数
void nfc_task_handler(lv_timer_t *task)
{
    Serial.print(nfc.currentMode);
    // 轮询NFC状态
    if (nfc.currentMode != NFCMode::NONE)
    {
        if (nfc.currentMode == NFCMode::READ)
        {
            String cardData = nfc.readCard();
            if (cardData != "No card detected, waiting...\n")
            {
                Serial.println(cardData);
                // 如果读取到卡片数据，停止任务
                lv_timer_del(task);
            }
        }
        else if (nfc.currentMode == NFCMode::WRITE)
        {
            const char *cardData = "1";
            // 执行写卡操作
            if (nfc.writeCard(reinterpret_cast<const uint8_t *>(cardData), 1))
            {
                Serial.println("Card written successfully.");
                // 如果写入成功，停止任务
                lv_timer_del(task);
            }
        }
    }
}

// NFC 页面事件处理函数
void nfc_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *data = (const char *)lv_event_get_user_data(e);

    if (code == LV_EVENT_CLICKED)
    {
        if (data == "scan")
        {
            nfc.setReadMode();
        }
        else if (data == "write")
        {
            nfc.setWriteMode();
        }
        else if (data == "none")
        {
            nfc.setNoneMode();
        }
        else if (data == "stop")
        {
            Serial.print("Event stop ");
            nfc.setNoneMode();
        }
        else if (data == "back")
        {
            Serial.print("Event back ");
            // 返回主页面
            lv_scr_load(main_screen); // 加载主页面
        }
    }
}

// NFC 页面初始化
void initNFCPage()
{
    nfc.initNFC();
    lv_obj_t *lcd_obj = lv_screen_active(); // 获取当前活动屏幕
    lv_obj_t *label;

    // 创建扫描按钮
    lv_obj_t *btn1 = lv_button_create(lcd_obj);
    lv_obj_add_event_cb(btn1, nfc_event_handler, LV_EVENT_CLICKED, (void *)"scan");
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Scan Card NFC");
    lv_obj_center(label);

    // 创建写入按钮
    lv_obj_t *btn2 = lv_button_create(lcd_obj);
    lv_obj_add_event_cb(btn2, nfc_event_handler, LV_EVENT_CLICKED, (void *)"write");
    lv_obj_align(btn2, LV_ALIGN_CENTER, 0, 50);
    lv_obj_remove_flag(btn2, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Write Card NFC");
    lv_obj_center(label);

    // 创建无操作按钮
    lv_obj_t *btn3 = lv_button_create(lcd_obj);
    lv_obj_add_event_cb(btn3, nfc_event_handler, LV_EVENT_CLICKED, (void *)"none");
    lv_obj_align(btn3, LV_ALIGN_CENTER, 0, 100);
    lv_obj_remove_flag(btn3, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn3);
    lv_label_set_text(label, "None Card NFC");
    lv_obj_center(label);

    // 创建返回主页面的按钮
    lv_obj_t *btn_back = lv_button_create(lcd_obj);
    lv_obj_add_event_cb(btn_back, nfc_event_handler, LV_EVENT_CLICKED, (void *)"back");
    lv_obj_align(btn_back, LV_ALIGN_BOTTOM_MID, 0, 150);
    lv_obj_remove_flag(btn_back, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn_back);
    lv_label_set_text(label, "Back to Main");
    lv_obj_center(label);

    // 创建返回主页面的按钮
    lv_obj_t *btn_stop = lv_button_create(lcd_obj);
    lv_obj_add_event_cb(btn_stop, nfc_event_handler, LV_EVENT_CLICKED, (void *)"stop");
    lv_obj_align(btn_stop, LV_ALIGN_BOTTOM_MID, 0, 200);
    lv_obj_remove_flag(btn_stop, LV_OBJ_FLAG_PRESS_LOCK);

    label = lv_label_create(btn_stop);
    lv_label_set_text(label, "Stop Card NFC");
    lv_obj_center(label);
    // 创建LVGL任务，每100毫秒调用一次nfc_task_handler
    lv_timer_create(nfc_task_handler, 1000, NULL); // 创建定时器，100ms执行一次 nfc_task_handler
}
