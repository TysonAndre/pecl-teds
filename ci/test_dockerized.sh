#!/usr/bin/env bash

# Author: Tyson Andre
#
# See print_usage for usage below.
print_usage() {
	cat 1>&2 <<EOT
Usage: $0 --docker-image-tag IMAGE_NAME [options]
Run unit tests of Teds.

  e.g. $0 --docker-image-tag 8.1

Options:

  --docker-image-tag DOCKER_IMAGE_TAG
    Required. DOCKER_IMAGE_TAG is the Docker image name from https://hub.docker.com/_/php to use (e.g. 8.1)
    This requires a debian/ubuntu-based image.

  --architecture {amd64, arm32v5, arm32v6, arm32v7, arm64v8, i386, mips64le, ppc64le, s390x}
    Defaults to the native CPU architecture.
    This can optionally be overridden, to emulate on a different architecture.

  --valgrind PHP_TEST_PHP_VERSION_FULL
	Defaults to off. Run tests with valgrind on a given full php version string (e.g. 8.1.11 or 8.2.0RC4)
EOT
}
# TODO: Switch to this for valgrind tests.
# TODO: Better long arg parsing.

print_usage_and_exit() {
	print_usage
	exit 1
}

# -x Exit immediately if any command fails
# -e Echo all commands being executed.
# -u fail for undefined variables

set -xeu

while [[ "$#" -gt 1 ]]; do
	# TODO support --longname=value

	case "$1" in
		--architecture)
			if [[ "$#" -lt 2 ]]; then
				echo "Error: Missing argument for '--argument'" 1>&2
				print_usage_and_exit
			fi
			PHP_TEST_ARCHITECTURE="$2"
			shift
			;;
		--docker-image-tag)
			if [[ "$#" -lt 2 ]]; then
				echo "Error: Missing argument for '--docker-image-tag'. Expected a docker image tag" 1>&2
				print_usage_and_exit
			fi
			PHP_TEST_DOCKER_IMAGE_TAG="$2"
			shift
			shift
			;;
		--valgrind)
			PHP_TEST_COMMAND=ci/test_inner_valgrind.sh
			PHP_TEST_DOCKERFILE=ci/Dockerfile.valgrind
			if [[ "$#" -lt 2 ]]; then
				echo "Error: Missing argument for '--valgrind'. Expected a full php version id such as 8.1.11 or 8.2.0RC4." 1>&2
				print_usage_and_exit
			fi
			PHP_TEST_PHP_VERSION_FULL="$2"
			shift
			shift
			;;
		-h|--help|help) print_usage; exit 0 ;;
		*)
			echo "$0: Unrecognized option '$1'" 1>&2
			print_usage_and_exit
			;;
	esac
done


PHP_TEST_ARCHITECTURE=${PHP_TEST_ARCHITECTURE:-}
if [[ -z "${PHP_TEST_DOCKER_IMAGE_TAG:-}" ]]; then
	echo "--docker-image-tag DOCKER_IMAGE_TAG is required" 1>&2
	print_usage_and_exit
fi

if [[ "$PHP_TEST_ARCHITECTURE" != "" ]]; then
	PHP_TEST_DOCKER_BASE_IMAGE="$PHP_TEST_ARCHITECTURE/php"
else
	PHP_TEST_DOCKER_BASE_IMAGE="php"
fi
PHP_TEST_COMMAND=${PHP_TEST_COMMAND:-ci/test_inner.sh}
PHP_TEST_PHP_VERSION_FULL=${PHP_TEST_PHP_VERSION_FULL:-}
PHP_TEST_DOCKERFILE=${PHP_TEST_DOCKERFILE:-ci/Dockerfile}

PHP_TEST_BUILT_DOCKER_IMAGE="$(echo "teds-test-runner:$PHP_TEST_DOCKER_BASE_IMAGE" | tr 'A-Z/' 'a-z_')"

docker build \
	--build-arg="PHP_VERSION_TAG=$PHP_TEST_DOCKER_IMAGE_TAG" \
	--build-arg="PHP_IMAGE=$PHP_TEST_DOCKER_BASE_IMAGE" \
	--build-arg="PHP_IMAGE=$PHP_TEST_DOCKER_BASE_IMAGE" \
	--tag="$PHP_TEST_BUILT_DOCKER_IMAGE" \
	-f "$PHP_TEST_DOCKERFILE" .

docker run --rm "$PHP_TEST_BUILT_DOCKER_IMAGE" "$PHP_TEST_COMMAND"
