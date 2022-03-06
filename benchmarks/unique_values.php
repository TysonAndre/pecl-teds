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
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_teds_tree_set(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum((new Teds\StrictTreeSet($values))->values());
    }
    $end = hrtime(true);
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_teds_sortedvector_set(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum((new Teds\StrictSortedVectorSet($values))->values());
    }
    $end = hrtime(true);
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
}
function bench_teds_hash_set(int $n, int $iterations) {
    $values = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand(0, ($n >> 2) - 1);
    }
    $start = hrtime(true);
    $sum = 0;
    for ($i = 0; $i < $iterations; $i++) {
        $sum += array_sum((new Teds\StrictHashSet($values))->values());
    }
    $end = hrtime(true);
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
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
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
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
    printf("%30s n=%8d iterations=%8d time=%.3f sum=%d\n", __FUNCTION__, $n, $iterations, ($end - $start)/1e9, $sum);
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
    bench_teds_hash_set($n, $iterations);
    bench_array_flip_keys($n, $iterations);
    bench_teds_sortedvector_set($n, $iterations);
    bench_teds_tree_set($n, $iterations);
    bench_array_unique($n, $iterations);
    echo "\n";
}
/*

Results for php 8.2.0-dev debug=0 with opcache enabled=true

Note that Teds\sorted_set is also sorting the elements and maintaining a balanced binary search tree.
Results for php 8.2.0-dev debug=0 with opcache enabled=true

Note that Teds\sorted_set is also sorting the elements and maintaining a balanced binary search tree.
           bench_unique_values n=       1 iterations= 8000000 time=0.377 sum=0
           bench_teds_hash_set n=       1 iterations= 8000000 time=1.115 sum=0
         bench_array_flip_keys n=       1 iterations= 8000000 time=0.575 sum=0
   bench_teds_sortedvector_set n=       1 iterations= 8000000 time=0.905 sum=0
           bench_teds_tree_set n=       1 iterations= 8000000 time=1.006 sum=0
            bench_array_unique n=       1 iterations= 8000000 time=0.220 sum=0

           bench_unique_values n=       4 iterations= 2000000 time=0.161 sum=0
           bench_teds_hash_set n=       4 iterations= 2000000 time=0.304 sum=0
         bench_array_flip_keys n=       4 iterations= 2000000 time=0.183 sum=0
   bench_teds_sortedvector_set n=       4 iterations= 2000000 time=0.455 sum=0
           bench_teds_tree_set n=       4 iterations= 2000000 time=0.292 sum=0
            bench_array_unique n=       4 iterations= 2000000 time=0.196 sum=0

           bench_unique_values n=       8 iterations= 1000000 time=0.100 sum=1000000
           bench_teds_hash_set n=       8 iterations= 1000000 time=0.167 sum=1000000
         bench_array_flip_keys n=       8 iterations= 1000000 time=0.148 sum=1000000
   bench_teds_sortedvector_set n=       8 iterations= 1000000 time=0.394 sum=1000000
           bench_teds_tree_set n=       8 iterations= 1000000 time=0.195 sum=1000000
            bench_array_unique n=       8 iterations= 1000000 time=0.151 sum=1000000

           bench_unique_values n=    1024 iterations=   20000 time=0.136 sum=636220000
           bench_teds_hash_set n=    1024 iterations=   20000 time=0.150 sum=636220000
         bench_array_flip_keys n=    1024 iterations=   20000 time=0.220 sum=636220000
   bench_teds_sortedvector_set n=    1024 iterations=   20000 time=2.632 sum=636220000
           bench_teds_tree_set n=    1024 iterations=   20000 time=1.200 sum=636220000
            bench_array_unique n=    1024 iterations=   20000 time=0.900 sum=636220000

           bench_unique_values n= 1048576 iterations=      20 time=0.609 sum=674350107920
           bench_teds_hash_set n= 1048576 iterations=      20 time=0.628 sum=674350107920
         bench_array_flip_keys n= 1048576 iterations=      20 time=0.710 sum=674350107920
   bench_teds_sortedvector_set n= 1048576 iterations=      20 time=5.609 sum=674350107920
           bench_teds_tree_set n= 1048576 iterations=      20 time=8.289 sum=674350107920
            bench_array_unique n= 1048576 iterations=      20 time=4.175 sum=674350107920

 */
