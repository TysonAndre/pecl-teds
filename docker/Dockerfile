ARG PHP_VERSION
FROM php:$PHP_VERSION
WORKDIR /code
ENV NO_INTERACTION=1
ADD *.c *.stub.php *.h config.m4 ./
RUN phpize &&  ./configure && make -j$(nproc)
ADD tests ./tests
