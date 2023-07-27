set(component_srcs "bee_uart.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "."
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES "bee_dht11" "bee_nvs" "bee_mqtt" "bee_wifi")
