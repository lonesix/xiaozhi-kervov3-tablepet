#include <esp_log.h>
#include <esp_err.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <esp_event.h>

#include "Application.h"
#include "SystemInfo.h"
#include "SystemReset.h"

// #include "config.h"
#define QJG_KEVIN_WIFI
#ifdef QJG_KEVIN_WIFI
#include <freertos/FreeRTOS.h>

#include <esp_log.h>
#include <esp_err.h>
#include <esp_event.h>
#include <driver/gpio.h>
#include <nvs.h>
#include <nvs_flash.h>

#include "lvgl.h"
#include "lv_gui.h"
#include "lvgl_helpers.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ops.h"

#include "avi_player.h"
#include "file_manager.h"
#include "esp_io_expander_tca9554.h"
#define GPIO_INPUT_PIN GPIO_NUM_0



// #define LCD_CTRL_GPIO BIT(1) // TCA9554_GPIO_NUM_1
// #define LCD_RST_GPIO BIT(2)	 // TCA9554_GPIO_NUM_2
// #define LCD_CS_GPIO BIT(3)	 // TCA9554_GPIO_NUM_2

#define LCD_CTRL_GPIO GPIO_NUM_48
#define LCD_RST_GPIO  GPIO_NUM_45                                 
#define LCD_CS_GPIO GPIO_NUM_46

#define SET_GPIO_HIGH(pin) gpio_set_level(pin, 1)
#define SET_GPIO_LOW(pin) gpio_set_level(pin, 0)

#define BSP_IO_EXPANDER_I2C_ADDRESS_TCA9554A (ESP_IO_EXPANDER_I2C_TCA9554A_ADDRESS_000)
#define BSP_IO_EXPANDER_I2C_ADDRESS_TCA9554 (ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000)

#define I2C_SCL_IO (GPIO_NUM_18)
#define I2C_SDA_IO (GPIO_NUM_17)

esp_err_t tfcard_ret = ESP_FAIL;
static void lv_tick_task(void *arg)
{
    (void)arg;
    lv_tick_inc(10);
}

SemaphoreHandle_t xGuiSemaphore;

// 屏幕测试函数，用于绘制图形  
void screen_test_draw(void) {  
    // 创建一个按钮对象来模拟矩形  
    lv_obj_t * rect_btn = lv_btn_create(lv_scr_act());  
    lv_obj_set_size(rect_btn, 100, 50); // 设置矩形大小  
    lv_obj_align(rect_btn, LV_ALIGN_CENTER, 0, 0); // 居中对齐  
    // lv_obj_add_style(rect_btn, &lv_style_plain, LV_PART_MAIN); // 可以选择添加样式  
    lv_obj_set_style_bg_color(rect_btn, lv_color_hex(0xff0000), LV_PART_MAIN); // 设置背景颜色为红色  
    lv_obj_set_style_border_color(rect_btn, lv_color_hex(0x0000ff), LV_PART_MAIN); // 设置边框颜色为蓝色  
    lv_obj_set_style_border_width(rect_btn, 2, LV_PART_MAIN); // 设置边框宽度  
    lv_obj_clear_flag(rect_btn, LV_OBJ_FLAG_CLICKABLE); // 如果不希望它是可点击的，可以清除点击标志  
  
    // 创建一个弧形对象来模拟圆形  
    lv_obj_t * circle_arc = lv_arc_create(lv_scr_act());  
    lv_obj_set_size(circle_arc, 80, 80); // 设置圆形大小（直径）  
    lv_obj_align(circle_arc, LV_ALIGN_TOP_MID, -40, -100); // 在屏幕顶部中央稍微偏移的位置  
    lv_arc_set_angles(circle_arc, 0, 360); // 设置弧形为完整圆形  
    lv_obj_set_style_bg_color(circle_arc, lv_color_hex(0x00ff00), LV_PART_MAIN); // 设置背景颜色为绿色  
    lv_obj_set_style_bg_opa(circle_arc, LV_OPA_50, LV_PART_MAIN); // 设置背景透明度为50%  
    lv_obj_add_flag(circle_arc, LV_OBJ_FLAG_HIDDEN); // 隐藏弧形对象的默认线条（因为我们只需要填充的圆形）  
    // lv_obj_clear_flag(circle_arc, LV_OBJ_FLAG_DRAW_ARC); // 清除绘制弧形的标志（但这似乎不是必要的，因为我们已经设置了完整圆形）  
    // 注意：LVGL 的某些版本可能不需要或不支持上述隐藏线条和清除标志的操作，  
    // 因为弧形对象默认就是按照设置的角度绘制的。如果圆形显示不正确，请查阅 LVGL 的文档和示例。  
  
    // 实际上，对于简单的圆形，你可能需要自定义绘制或使用其他方法，  
    // 因为 lv_arc 主要是用来表示弧形（即圆的一部分）的。  
    // 下面是一个使用 lv_canvas 和自定义绘制来创建圆形的示例：  
  
    // // 创建一个画布对象  
    // lv_obj_t * canvas = lv_canvas_create(lv_scr_act());  
    // lv_obj_set_size(canvas, 80, 80); // 设置画布大小  
    // lv_obj_align(canvas, NULL, LV_ALIGN_BOTTOM_MID, 0, 40); // 在屏幕底部中央对齐  
  
    // // 获取画布的绘制缓冲区  
    // lv_canvas_buf_t * cbuf = lv_canvas_get_buf(canvas);  
  
    // 填充圆形（这里使用简单的 Bresenham 算法绘制圆形的轮廓，但实际上 LVGL 提供了更高效的绘制函数）  
    // 注意：下面的代码仅用于说明目的，并不推荐在实际项目中使用，因为 LVGL 提供了更高级的绘制功能。  
    // 正确的做法是使用 lv_draw_line 或 lv_draw_rect 等函数配合变换矩阵来绘制圆形。  
    // 但由于这些函数通常用于更复杂的场景，并且对于简单的圆形来说可能过于繁琐，  
    // 因此在实际项目中，你可能更倾向于使用 lv_img 或预渲染的图像来显示圆形。  
    // 然而，为了完整性，这里还是提供了一个简单的圆形轮廓绘制示例（仅作为学习目的）：  
    // ...（此处省略了圆形绘制代码，因为 LVGL 不推荐这样做，并且实现起来相对复杂）  
    // 正确的做法应该是使用 lv_draw_arc 并设置起始和结束角度为 0 和 360，但 lv_draw_arc 是低级函数，  
    // 通常不直接用于对象绘制。相反，你应该使用 lv_arc 对象并正确设置其样式。  
    // 但由于我们已经创建了一个 lv_arc 对象并且遇到了问题（可能是版本差异或误解 API），  
    // 这里我们将使用 lv_canvas 和自定义绘制函数来演示如何绘制一个填充的圆形。  
    // 然而，请注意，下面的代码并不是使用 LVGL API 的最佳实践。  
  
    // 更好的做法是使用 lv_img 加载一个预制的圆形图像或使用 lv_draw_rect 配合变换来绘制圆形填充。  
    // 但由于这些都不是直接的方法，并且可能需要额外的资源或复杂的设置，  
    // 因此在这里我们提供一个使用 lv_canvas 和简单数学来绘制圆形的示例（仅供学习）。  
  
    // 由于直接绘制圆形比较复杂且不是 LVGL 的推荐用法，  
    // 这里我们提供一个简化的示例，它实际上并不使用 LVGL 的绘制函数来绘制完整的圆形，  
    // 而是展示了如何使用 lv_canvas 和数学来计算圆形的边界点（但这并不构成完整的圆形绘制）。  
    // 为了简洁起见，并避免引入复杂的数学和绘制逻辑，  
    // 这里我们不再展示完整的圆形绘制代码。  
    // 相反，我们建议你查阅 LVGL 的文档和示例，了解如何使用 lv_arc、lv_img 或其他对象来绘制圆形。  
  
    // 由于上述原因，我们在这里仅使用 lv_canvas 来展示如何设置一个空白画布，  
    // 并鼓励你查阅 LVGL 的官方文档和社区资源来了解如何正确地绘制圆形。  
  
    // 刷新屏幕以显示绘制的图形  
    lv_task_handler(); // 处理 LVGL 的任务（在你的主循环中周期性地调用）  
}  

#ifdef ADC_CHARGE_TEST
static lv_obj_t *charge_label = NULL; // 声明标签对象

void label_charge_set_text(char *text)
{
    if (charge_label != NULL)
        lv_event_send(charge_label, LV_EVENT_VALUE_CHANGED, (void *)text);
}

static void charge_label_event_handler(lv_event_t *event)
{
        char *text = (char *)event->param;
    if (NULL != text)
    {
        printf("lv_label_set_text_static 1:%s\n", text);
        lv_label_set_text(charge_label, text);
    }
}

static bool charge_label_Is_NULL()
{
    if (charge_label == NULL)
    {
        return true;
    }
    return false;
    
}

static void gui_charge_display()
{
    lv_obj_t *parent = lv_scr_act();
    	    // 创建标签对象
    charge_label = lv_label_create(parent);
    // lv_obj_set_pos(charge_label, 0, 0);
    //设置背景透明度
    lv_obj_set_style_bg_opa(charge_label, LV_OPA_0, 0);
    //设置白色背景
    lv_obj_set_style_text_color(charge_label, lv_color_white(), 0);
    //更改对象的对齐方式并设置新的坐标。
	lv_obj_align(charge_label,  LV_ALIGN_OUT_TOP_RIGHT, 0, 0);//LV_ALIGN_OUT_RIGHT_TOP
    //设置文本对齐方式为右对齐。
    lv_obj_set_style_text_align(charge_label, LV_TEXT_ALIGN_RIGHT, 0);

    // lv_obj_add_style(charge_label, &style, 0);
    lv_label_set_text(charge_label, "?");
    //设置事件  
    lv_obj_add_event_cb(charge_label, charge_label_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

}
#endif

static void gui_task(void *arg)
{
    xGuiSemaphore = xSemaphoreCreateMutex();
    lv_init(); // lvgl内核初始化
    SET_GPIO_LOW(LCD_RST_GPIO);

	// tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_LOW);
	vTaskDelay(20 / portTICK_PERIOD_MS);
    SET_GPIO_HIGH(LCD_RST_GPIO);
	// // tca9554_set_output_state(LCD_RST_GPIO, TCA9554_IO_HIGH);
	vTaskDelay(200 / portTICK_PERIOD_MS);
    lvgl_driver_init(); // lvgl显示接口初始化

    /* Example for 1) */
    static lv_disp_draw_buf_t draw_buf;

    lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
    lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_DMA | MALLOC_CAP_8BIT);

    // lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    // lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc(DISP_BUF_SIZE * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    lv_disp_draw_buf_init(&draw_buf, buf1, buf2, DLV_HOR_RES_MAX * DLV_VER_RES_MAX); /*Initialize the display buffer*/

    static lv_disp_drv_t disp_drv;         /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);           /*Basic initialization*/
    disp_drv.draw_buf = &draw_buf;         /*Set an initialized buffer*/
    disp_drv.flush_cb = disp_driver_flush; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = 320;                /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = 240;                /*Set the vertical resolution in pixels*/
    disp_drv.sw_rotate = 1; // 添加旋转
    disp_drv.rotated = LV_DISP_ROT_NONE; // 添加旋转
    lv_disp_drv_register(&disp_drv);       /*Register the driver and save the created display objects*/
                                           /*触摸屏输入接口配置*/
    // touch_driver_init();
    // lv_indev_drv_t indev_drv;
    // lv_indev_drv_init(&indev_drv);
    // // indev_drv.read_cb = touch_driver_read;
    // indev_drv.type = LV_INDEV_TYPE_POINTER;
    // lv_indev_drv_register(&indev_drv);

    // esp_register_freertos_tick_hook(lv_tick_task);

    /* 创建一个定时器中断来进入 lv_tick_inc 给lvgl运行提供心跳 这里是10ms一次 主要是动画运行要用到 */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 10 * 1000));

    lv_main_page();
    if (tfcard_ret == ESP_OK)
    {avi_player_load();}
    #ifdef ADC_CHARGE_TEST
    gui_charge_display();
    #endif
        // lv_main_page();
    // screen_test_draw();

    // lv_demo_widgets();
    // lv_demo_music();
    // lv_demo_benchmark();
    while (1)
    {
        /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
        vTaskDelay(pdMS_TO_TICKS(10));

        /* Try to take the semaphore, call lvgl related function on success */
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {

            lv_timer_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
    }
}
#endif

#define TAG "main"

extern "C" void app_main(void)
{
    // Check if the reset button is pressed
    SystemReset::GetInstance().CheckButtons();

    // Initialize the default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize NVS flash for WiFi configuration
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    #ifdef QJG_KEVIN_WIFI
    // Initialize NVS flash for WiFi configuration


gpio_config_t gpio_conf;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.mode = GPIO_MODE_OUTPUT;
     gpio_conf.pin_bit_mask = (1ULL << LCD_CTRL_GPIO) | (1ULL << LCD_RST_GPIO) \
            | (1ULL << LCD_CS_GPIO);
    gpio_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    
    gpio_config(&gpio_conf);




        // 设置输出模式
    gpio_set_direction(LCD_CTRL_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_RST_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_CS_GPIO, GPIO_MODE_OUTPUT);
	// 控制引脚电平
    SET_GPIO_HIGH(LCD_CTRL_GPIO);
    SET_GPIO_LOW(LCD_RST_GPIO);

    vTaskDelay(200 / portTICK_PERIOD_MS);
    SET_GPIO_HIGH(LCD_RST_GPIO);
    vTaskDelay(200 / portTICK_PERIOD_MS);
// /* Initialize I2C peripheral */
//     i2c_master_bus_handle_t i2c_handle = NULL;
//     const i2c_master_bus_config_t bus_config = {
//         .i2c_port = I2C_NUM_0,
//         .sda_io_num = I2C_SDA_IO,
//         .scl_io_num = I2C_SCL_IO,
//         .clk_source = I2C_CLK_SRC_DEFAULT,
//     };
//     i2c_new_master_bus(&bus_config, &i2c_handle);

//     // const i2c_config_t es_i2c_cfg = {
//     //     .mode = I2C_MODE_MASTER,
//     //     .sda_io_num = I2C_SDA_IO,
//     //     .scl_io_num = I2C_SCL_IO,
//     //     .sda_pullup_en = GPIO_PULLUP_ENABLE,
//     //     .scl_pullup_en = GPIO_PULLUP_ENABLE,
//     //     .master = {
//     //         .clk_speed = 400000,
//     //     }};
//     // (i2c_param_config(I2C_NUM_0, &es_i2c_cfg));
//     // (i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
//     // ESP_IO_EXPANDER_I2C_TCA9554_ADDRESS_000
//     static esp_io_expander_handle_t io_expander = NULL; // IO expander tca9554 handle
//     if ((esp_io_expander_new_i2c_tca9554(i2c_handle, BSP_IO_EXPANDER_I2C_ADDRESS_TCA9554, &io_expander) != ESP_OK) &&
//         (esp_io_expander_new_i2c_tca9554(i2c_handle, BSP_IO_EXPANDER_I2C_ADDRESS_TCA9554A, &io_expander) != ESP_OK))
//     {
//         ESP_LOGE(TAG, "Failed to initialize IO expander");
//     }
//     else
//     {
//         ESP_LOGI(TAG, "Initialize IO expander OK");

//         (esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_2, IO_EXPANDER_OUTPUT));
//         (esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, false));
//         vTaskDelay(200);
//         (esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_2, true));
//     }
//     i2c_del_master_bus(i2c_handle);
// static const char *TAG = "LVGL_TEST";
tfcard_ret = fm_sdcard_init();
xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 0);
vTaskDelay(1000);
// vTaskDelay(2000/ portTICK_PERIOD_MS);
    #endif
    // Otherwise, launch the application
    // Application::GetInstance().Start();
//     tfcard_ret = fm_sdcard_init();
// vTaskDelay(1000);
// xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 0);
//     xTaskCreatePinnedToCore(&gui_task, "gui task", 1024 * 5, NULL, 5, NULL, 0);
    #ifdef QJG_KEVIN_WIFI
if (tfcard_ret == ESP_OK)
    {
        while (1)
        {
            // biaoqing = FACE_NULL;
            switch (biaoqing)
            {
            case 0:
                play_change(FACE_STATIC);
                break;

            case 1:
                play_change(FACE_HAPPY);

                break;
            case 2:
                play_change(FACE_ANGRY);

                break;
            case 3:
                play_change(FACE_BAD);

                break;
            case 4:
                play_change(FACE_FEAR);

                break;
            case 5:
                play_change(FACE_NOGOOD);

                break;
            default:
                play_change(FACE_NULL);
                break;
            }
            biaoqing = 0;
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
    }
    #endif
    // Dump CPU usage every 10 second
    while (true) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        // SystemInfo::PrintRealTimeStats(pdMS_TO_TICKS(1000));
        int free_sram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        int min_free_sram = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
        ESP_LOGI(TAG, "Free internal: %u minimal internal: %u", free_sram, min_free_sram);
    }
}
