#!/bin/bash
IMAGE_NAME=pecl-teds-s390x-emulated-8.0.13
docker build --platform S390X --tag=$IMAGE_NAME --build-arg=PHP_VERSION=8.0.13 -f docker/Dockerfile . || exit 1
exec docker run $IMAGE_NAME make test TESTS="--show-diff -j$(nproc)"
