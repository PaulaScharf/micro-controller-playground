idf_build_get_property(target IDF_TARGET)

set(srcs "src/esp_timer.c"
         "src/ets_timer_legacy.c"
         "src/system_time.c")

    list(APPEND srcs "src/esp_timer_impl_lac.c")


idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES esp_common
                    PRIV_REQUIRES soc driver)
