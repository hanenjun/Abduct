#ifndef NFC_PAGE_H
#define NFC_PAGE_H

#include "NFCMode.h"
#include "lvgl.h"

// 外部声明 NFC 对象
extern NFC nfc;

/**
 * @brief 初始化 NFC 页面
 * 
 * 该函数用于初始化 NFC 页面，创建按钮并设置相关事件处理函数。
 */
void initNFCPage();

/**
 * @brief 事件处理函数
 * 
 * 该函数处理 NFC 页面上的事件，例如扫描、写入卡片等操作。
 * 
 * @param e 事件对象
 */
void nfc_event_handler(lv_event_t *e);

/**
 * @brief NFC 状态更新任务函数
 * 
 * 该函数是定时器任务回调，用于更新 NFC 状态、执行读卡和写卡操作。
 * 
 * @param e 定时器任务对象
 */
void nfc_task_handler(lv_timer_t *e);

#endif // NFC_PAGE_H
