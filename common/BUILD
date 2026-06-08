load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    srcs = ["clock.cpp"],
    hdrs = ["clock.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "udp_socket",
    srcs = ["udp_socket.cpp"],
    hdrs = ["udp_socket.h", "msg.h"],
    visibility = ["//visibility:public"],
)
