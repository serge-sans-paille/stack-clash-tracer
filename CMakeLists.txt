cmake_minimum_required (VERSION 3.2)
project(QBDITemplate)

find_package(QBDI REQUIRED)
find_package(QBDIPreload REQUIRED)

add_library(stack_clash_tracer SHARED stack_clash_tracer.c)
target_link_libraries(stack_clash_tracer QBDI::QBDI QBDIPreload::QBDIPreload)
