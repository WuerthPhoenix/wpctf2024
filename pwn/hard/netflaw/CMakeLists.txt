cmake_minimum_required(VERSION 3.26)
project(netflaw C)

set(CMAKE_C_STANDARD 11)

add_executable(netflaw main.c
        udp_receiver.c
        udp_receiver.h
        netflow.c
        netflow.h
        csv.c
        csv.h
        status_data.c
        status_data.h
        config.c
        config.h)
