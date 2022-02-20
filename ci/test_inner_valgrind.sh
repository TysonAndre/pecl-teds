#!/usr/bin/env bash
# -x Exit immediately if any command fails
# -e Echo all commands being executed.
# -u fail for undefined variables
set -xeu
php --version
php --ini
echo "Run tests without docker first to test reference counting with a php build using --enable-debug"
REPORT_EXIT_STATUS=1 make test TESTS="-P -q --show-diff -j$(nproc) tests/ImmutableSortedStringSet"
echo "Run tests in docker"
REPORT_EXIT_STATUS=1 make test TESTS="-P -q --show-diff -m --show-mem -j$(nproc)"
echo "Test that package.xml is valid"
# Ensure package.xml is valid and that the package is not missing any files
pecl package
pecl install ./teds*.tgz
