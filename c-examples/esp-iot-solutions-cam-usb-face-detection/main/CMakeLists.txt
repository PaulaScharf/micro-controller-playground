idf_component_register(SRCS main.cpp wave_1ch_16bits.c
                    INCLUDE_DIRS "." REQUIRES fatfs esp32-camera esp_timer fb_gfx esp-dl)