<?php

use function Teds\unique_values;

function bench_unique_values(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum(unique_values($values));
    }
    $end = hrtime(true);
    printf("%25s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_teds_sorted_set(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum((new Teds\SortedStrictSet($values))->values());
    }
    $end = hrtime(true);
    printf("%25s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_array_unique(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum(array_unique($values));
    }
    $end = hrtime(true);
    printf("%25s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_array_flip_keys(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum(array_keys(array_flip($values)));
    }
    $end = hrtime(true);
    printf("%25s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 8000000],
    [4, 2000000],
    [8, 1000000],
    [2**10, 20000],
    [2**20, 20],
];
printf(
    "Results for php %s debug=%s with opcache enabled=%s\n\n",
    PHP_VERSION,
    json_encode(PHP_DEBUG),
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "Note that Teds\sorted_set is also sorting the elements and maintaining a balanced binary search tree.\n";

foreach ($sizes as [$n, $iterations]) {
    bench_unique_values($n, $iterations);
    bench_teds_sorted_set($n, $iterations);
    bench_array_unique($n, $iterations);
    bench_array_flip_keys($n, $iterations);
    echo "\n";
}
/*
Results for php 8.2.0-dev debug=false with opcache enabled=true

Results for php 8.2.0-dev debug=0 with opcache enabled=true

Note that Teds\sorted_set is also sorting the elements and maintaining a balanced binary search tree.
      bench_unique_values n=       1 iterations= 8000000 time=0.374 sum=0
    bench_teds_sorted_set n=       1 iterations= 8000000 time=0.889 sum=0
       bench_array_unique n=       1 iterations= 8000000 time=0.210 sum=0
    bench_array_flip_keys n=       1 iterations= 8000000 time=0.568 sum=0

      bench_unique_values n=       4 iterations= 2000000 time=0.155 sum=0
    bench_teds_sorted_set n=       4 iterations= 2000000 time=0.251 sum=0
       bench_array_unique n=       4 iterations= 2000000 time=0.186 sum=0
    bench_array_flip_keys n=       4 iterations= 2000000 time=0.180 sum=0

      bench_unique_values n=       8 iterations= 1000000 time=0.099 sum=1000000
    bench_teds_sorted_set n=       8 iterations= 1000000 time=0.171 sum=1000000
       bench_array_unique n=       8 iterations= 1000000 time=0.140 sum=1000000
    bench_array_flip_keys n=       8 iterations= 1000000 time=0.142 sum=1000000

      bench_unique_values n=    1024 iterations=   20000 time=0.152 sum=636220000
    bench_teds_sorted_set n=    1024 iterations=   20000 time=1.184 sum=636220000
       bench_array_unique n=    1024 iterations=   20000 time=0.817 sum=636220000
    bench_array_flip_keys n=    1024 iterations=   20000 time=0.211 sum=636220000

      bench_unique_values n= 1048576 iterations=      20 time=0.646 sum=674350107920
    bench_teds_sorted_set n= 1048576 iterations=      20 time=9.348 sum=674350107920
       bench_array_unique n= 1048576 iterations=      20 time=4.264 sum=674350107920
    bench_array_flip_keys n= 1048576 iterations=      20 time=0.728 sum=674350107920

 */
