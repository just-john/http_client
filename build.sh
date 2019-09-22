#!/bin/bash

source ./profile

mkdir -p "${PROJECT_BUILD_DIR}"
cd "${PROJECT_BUILD_DIR}"

cmake -G Ninja ../
ninja
ninja install

cd "${PROJECT_ROOT}"

