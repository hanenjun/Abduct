#ifndef NFC_PAGE_H
#define NFC_PAGE_H

#include "NFCMode.h"
#include "lvgl.h"

extern NFC nfc;

// NFC 页面的初始化函数
void initNFCPage();

// 事件处理函数
void nfc_event_handler(lv_event_t *e);
void nfc_task_handler(lv_timer_t *e);


#endif // NFC_PAGE_H
