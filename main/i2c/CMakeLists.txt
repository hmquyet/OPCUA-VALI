set(app_src ds3231.c i2cdev.c stm32f1i2c.c)
set(pri_req)
idf_component_register(SRCS ${app_src}
                    INCLUDE_DIRS "."
                    INCLUDE_DIRS "D:/ESP32/TEST-IOT/components/wifi_mqtt"
                    INCLUDE_DIRS "D:/ESP32/Espressif/frameworks/esp-idf-v4.3.6/components"
                    REQUIRES ${pri_req}
                    PRIV_REQUIRES main nvs_flash app_update)