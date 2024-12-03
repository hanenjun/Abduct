// MainPage.cpp
#include "MainPage.h"
#include "Display.h"
#include "NFCPage.h"   // 引入NFC页面头文件
#include "GPSModule.h" // 引入NFC页面头文件

lv_obj_t *main_screen; // 主页面对象
GPSModule gps;
void gps_task_handler(lv_timer_t *task)
{
    gps.update();

}
// 初始化主页面
void initMainPage()
{
    gps.initGPS();
    main_screen = lv_obj_create(NULL); // 创建主页面

    // 在主页面上添加一个标题
    lv_obj_t *label = lv_label_create(main_screen);
    lv_label_set_text(label, "Main Screen");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    // 创建一个按钮，点击后跳转到 NFC 页面
    lv_obj_t *btn = lv_button_create(main_screen);
    lv_obj_add_event_cb(btn, main_event_handler, LV_EVENT_CLICKED, (void *)"NFC");
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_remove_flag(btn, LV_OBJ_FLAG_PRESS_LOCK);

    // 给按钮添加标签
    label = lv_label_create(btn);
    lv_label_set_text(label, "Go to NFC Page");
    lv_obj_center(label);
    lv_timer_create(gps_task_handler, 5000, NULL); // 创建定时器，100ms执行一次 nfc_task_handler

    lv_scr_load(main_screen); // 加载主页面
}

// 主页面的事件处理函数
void main_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    const char *data = (const char *)lv_event_get_user_data(e);
    // 打印 event code 和 user data 到串口监视器
    Serial.print("Event code: ");
    Serial.println(code); // 打印事件代码
    Serial.print("User data: ");
    Serial.println(data); // 打印 user_data
    if (code == LV_EVENT_CLICKED && data == "NFC")
    {
        // 跳转到 NFC 页面
        initNFCPage();
    }
}