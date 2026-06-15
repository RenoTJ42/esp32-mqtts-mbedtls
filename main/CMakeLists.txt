idf_component_register(
    SRCS         "main.c"
    INCLUDE_DIRS "."
    REQUIRES     uart_driver
                 esp_partition
                 log
                 esp_common
)
