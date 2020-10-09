# -*- mode: python; -*-
#
# Copyright 2010 Google Inc.
#
# Description:
#  The DC3 Suffix Array Creation Algorithm as described in:
#  "Simple Linear Work Suffix Array Construction" (2003)
#    by Juha Karkkarian, Peter Sanders and Stefan Burkhardt.
#  http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.137.7871
#  Implemented by David Eger <eger@google.com>

package(
    default_copts = ["-DDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    default_hdrs_check = "loose",
    default_visibility = ["//visibility:public"],
    features = [
        "-layering_check",
        "-parse_headers",
    ],
)

load("//tools/build_defs:cc_public_library.bzl", "cc_public_library")

VERSION = "1.0"

licenses(["notice"])  # Apache 2.0

cc_public_library(
    name = "rmq",
    hdrs = [
        "rmq.h",
        "st_rmq.h",
    ],
    implementation = ["//third_party/dc3/internal:rmq"],
)

cc_public_library(
    name = "dc3",
    hdrs = ["dc3.h"],
    implementation = ["//third_party/dc3/internal:dc3"],
)

cc_public_library(
    name = "ordinal_map",
    hdrs = ["ordinal_map.h"],
    implementation = ["//third_party/dc3/internal:ordinal_map"],
)
