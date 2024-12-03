#include "Display.h"

LGFX lcd; // 创建LGFX显示对象
const unsigned int screenWidth = 240;                                                // 定义屏幕宽度为240像素
const unsigned int screenHeight = 320;   
const unsigned int lvBufferSize = screenWidth * screenHeight * (LV_COLOR_DEPTH / 8); // 计算缓冲区大小，根据屏幕分辨率和颜色深度
uint8_t lvBuffer[lvBufferSize];                                                      // 定义缓冲区，存储像素数据
                                            // 定义屏幕高度为320像素
LGFX::LGFX()
{
    {
                {
            // 配置 SPI 总线引脚
            auto cfg = _bus_instance.config();   // 获取总线配置
            cfg.spi_host = SPI2_HOST;           // 使用 HSPI
            cfg.spi_mode = 0;                   // SPI 模式 0
            cfg.freq_write = 40000000;          // 写入频率 40MHz
            cfg.freq_read = 16000000;           // 读取频率 16MHz
            cfg.spi_3wire = true;               // 使用 3 线 SPI
            cfg.use_lock = true;                // 使用锁机制
            cfg.dma_channel = 1;                // DMA 通道 1
            cfg.pin_sclk = 12;                  // SCK 引脚
            cfg.pin_mosi = 11;                  // MOSI 引脚
            cfg.pin_miso = 13;                  // MISO 引脚
            cfg.pin_dc = 2;                     // 数据/命令 (DC/RS) 引脚
            _bus_instance.config(cfg);          // 应用配置
            _panel_instance.setBus(&_bus_instance);  // 连接总线到面板
        }

        {
            // 配置显示屏参数
            auto cfg = _panel_instance.config(); // 获取显示屏配置
            cfg.pin_cs = 10;                     // CS 引脚
            cfg.pin_rst = 15;                    // RESET 引脚
            cfg.pin_busy = -1;                   // BUSY 引脚（未使用）
            cfg.memory_width = 240;              // 显存宽度
            cfg.memory_height = 320;             // 显存高度
            cfg.panel_width = 240;               // 屏幕宽度
            cfg.panel_height = 320;              // 屏幕高度
            cfg.offset_x = 0;                    // X 偏移
            cfg.offset_y = 0;                    // Y 偏移
            cfg.offset_rotation = 0;             // 旋转偏移
            cfg.dummy_read_pixel = 8;            // 像素读取虚拟周期
            cfg.dummy_read_bits = 1;             // 位读取虚拟周期
            cfg.readable = true;                 // 允许读屏
            cfg.invert = false;                  // 不反转颜色
            cfg.rgb_order = false;               // RGB 颜色顺序
            cfg.dlen_16bit = false;              // 数据总线为 16 位
            cfg.bus_shared = true;               // 共享总线
            _panel_instance.config(cfg);
        }

        {
            // 配置触摸屏参数
            auto cfg = _touch_instance.config(); // 获取触摸屏配置
            cfg.x_min = 0;                       // 最小 X 值
            cfg.x_max = 240;                     // 最大 X 值
            cfg.y_min = 0;                       // 最小 Y 值
            cfg.y_max = 320;                     // 最大 Y 值
            cfg.pin_int = -1;                    // INT 引脚（未使用）
            cfg.bus_shared = false;              // 触摸总线独立
            cfg.offset_rotation = 0;             // 旋转偏移
            cfg.i2c_port = 1;                    // I2C 端口 1
            cfg.i2c_addr = 0x38;                 // I2C 地址（FT6336）
            cfg.pin_sda = 5;                     // SDA 引脚
            cfg.pin_scl = 6;                     // SCL 引脚
            cfg.freq = 400000;                   // I2C 频率
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);  // 连接触摸屏到面板
        }
        setPanel(&_panel_instance);              // 连接面板到 LGFX

    }
}

void LGFX::initDisplay()
{
    lcd.init();         // 初始化LCD屏幕
    lcd.setRotation(0); // 设置屏幕方向（0表示默认方向）
}

void LGFX::initLVGL()
{
    lv_init(); // 初始化LVGL库

#if LV_USE_LOG != 0
    lv_log_register_print_cb([](lv_log_level_t level, const char *buf)
                             {
                                 LV_UNUSED(level);
                                 Serial.println(buf); // 输出日志信息到串口监视器
                                 Serial.flush();      // 刷新串口缓冲区
                             });
#endif

    // 创建LVGL显示对象，并设置显示的宽度和高度
    static auto *lvDisplay = lv_display_create(screenWidth, screenHeight);
    lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565); // 设置颜色格式为RGB565
    lv_display_set_flush_cb(lvDisplay, [](lv_display_t *display, const lv_area_t *area, unsigned char *data)
                            {
                                uint32_t w = lv_area_get_width(area);                      // 获取区域宽度
                                uint32_t h = lv_area_get_height(area);                     // 获取区域高度
                                lv_draw_sw_rgb565_swap(data, w * h);                       // 交换像素数据（RGB565格式）
                                lcd.pushImage(area->x1, area->y1, w, h, (uint16_t *)data); // 将图像数据推送到屏幕
                                lv_display_flush_ready(display);                           // 完成刷新
                            });

    // 设置显示的缓冲区
    lv_display_set_buffers(lvDisplay, lvBuffer, nullptr, lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

    // 创建触摸输入设备对象并设置触摸读取回调
    static lv_indev_t *indev_touchpad = lv_indev_create();
    lv_indev_set_type(indev_touchpad, LV_INDEV_TYPE_POINTER); // 设置输入设备类型为指针
    lv_indev_set_read_cb(indev_touchpad, touchpad_read);      // 设置读取触摸回调
}

void LGFX::touchpad_read(lv_indev_t *indev, lv_indev_data_t *data)
{
    int x, y;
    bool touched = lcd.getTouch(&x, &y); // 检测触摸屏是否被按下，并获取按下的坐标

    if (touched)
    {
        data->state = LV_INDEV_STATE_PRESSED; // 如果触摸屏被按下，设置状态为按下
        data->point.x = screenWidth - x;      // 设置触摸点的X坐标
        data->point.y = screenHeight - y;     // 设置触摸点的Y坐标
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED; // 如果没有按下，设置状态为释放
    }
}
