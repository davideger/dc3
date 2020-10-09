# -*- mode: python; -*-
#
# Copyright 2010 Google Inc
#
# Description:
#  The DC3 Suffix Array Creation Algorithm as described in:
#  "Simple Linear Work Suffix Array Construction" (2003)
#    by Juha Karkkarian, Peter Sanders and Stefan Burkhardt.
#  http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.137.7871
#  Implemented by David Eger <eger@google.com>

package(
    default_copts = ["-DDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    default_visibility = ["//visibility:public"],
    features = [
        "-layering_check",
        "-parse_headers",
    ],
)

load("//tools/build_defs:cc_public_library.bzl", "cc_internal_library")

VERSION = "1.0"

licenses(["notice"])  # Apache 2.0

cc_internal_library(
    name = "ordinal_map",
    srcs = ["ordinal_map.cc"],
    copts = ["-UDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    implements = ["//third_party/dc3/public:ordinal_map"],
)

cc_internal_library(
    name = "rmq",
    srcs = ["st_rmq.cc"],
    copts = ["-UDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    # deps = ["//base"],
    implements = ["//third_party/dc3/public:rmq"],
)

cc_internal_library(
    name = "dc3",
    srcs = ["dc3.cc"],
    copts = ["-UDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    deps = [
        ":ordinal_map",
        ":rmq",
        "//base",
    ],
    implements = ["//third_party/dc3/public:dc3"],
)

# TODO: Integrate with the open source versions of googletest,
#       absl, and file APIs.
cc_test(
    name = "rmq_test",
    size = "small",
    srcs = [
        "rmq_test.cc",
        "trivial_rmq.cc",
        "trivial_rmq.h",
    ],
    copts = ["-UDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    deps = [
        ":rmq",
        "//strings",
        "//testing/base/public:gunit",
        "//testing/base/public:gunit_main",
        "//third_party/abcl/strings",
    ],
)

cc_test(
    name = "dc3_test",
    size = "medium",
    srcs = ["dc3_test.cc"],
    copts = ["-UDISABLE_GOOGLE_GLOBAL_USING_DECLARATIONS"],
    data = ["testdata/blobbox.cpp.2007.txt"],
    deps = [
        ":dc3",
        "//file/base",
        "//file/base:path",
        "//file/localfile",
        "//testing/base/public:gunit",
        "//testing/base/public:gunit_main",
    ],
)
