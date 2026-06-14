cmake_minimum_required(VERSION 3.16)

set(PARTITION_TABLE_CSV_PATH "${CMAKE_SOURCE_DIR}/partitions/partitions.csv")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(secure_iot_esp32)
