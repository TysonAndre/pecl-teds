#!/bin/bash
IMAGE_NAME=pecl-teds-arm64-emulated-8.1.0
docker build --platform arm64 --tag=$IMAGE_NAME --build-arg=PHP_VERSION=8.1.0 -f docker/Dockerfile . || exit 1
exec docker run $IMAGE_NAME make test TESTS="--show-diff -j$(nproc)"
