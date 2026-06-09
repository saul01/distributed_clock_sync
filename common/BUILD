load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "clock",
    srcs = ["clock/clock.cpp"],
    hdrs = ["clock/clock.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "comms",
    srcs = ["comms/udp_comms.cpp"],
    hdrs = ["comms/udp_comms.h", "comms/i_comms.h"],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "audio_node",
    srcs = ["audio_node/audio_node.cpp"],
    hdrs = ["audio_node/audio_node.h"],
    deps = [":clock", ":comms"],
    visibility = ["//visibility:public"],
)
