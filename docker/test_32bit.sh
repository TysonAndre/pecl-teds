#!/bin/bash
docker build --platform i386 --tag=pecl-teds-32bit-8.1.2 -f docker/Dockerfile
exec docker run pecl-teds-32bit-8.1.2 make test TESTS="--show-diff -j$(nproc)"
