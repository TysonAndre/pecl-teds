<?php

use Teds\StrictTreeMap;

// @phan-file-suppress PhanPossiblyUndeclaredVariable

function bench_unserialize_sorted_set(int $n, int $iterations) {
    $original = new StrictTreeMap();
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $original->offsetSet(rand(), rand());
    }
    $ser = serialize($original);
    unset($original);
    $totalDecodeTime = 0;
    $totalReadTime = 0;
    $totalReSortTime = 0;
    $total = 0;

    $startTime = hrtime(true);
    for ($j = 0; $j < $iterations; $j++) {
        srand(1234);
        $startMemory = memory_get_usage();
        $values = unserialize($ser);
        $t2 = hrtime(true);
        $total += $values[rand()];
        $t3 = hrtime(true);
        $values[rand() + 123] = 123;
        $values[rand() + 456] = 123;
        // StrictTreeMap stays sorted by keys
        $t4 = hrtime(true);

        $totalReadTime += $t3 - $t2;
        $totalReSortTime += $t4 - $t3;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);

    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    $totalReSortTimeSeconds = $totalReSortTime / 1000000000;
    printf("Unserializing sorted StrictTreeMap and reading single val: n=%8d iterations=%8d memory=%8d bytes, create+destroy time=%.3f read single time = %.3f add+sort=%.3f total=%.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds - $totalReSortTimeSeconds, $totalReadTimeSeconds, $totalReSortTimeSeconds, $totalTime, $total);
}
function bench_unserialize_sorted_array(int $n, int $iterations) {
    $original = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $k = rand();
        $original[$k] = rand();
    }
    ksort($original);
    $ser = serialize($original);
    unset($original);
    $totalDecodeTime = 0;
    $totalReadTime = 0;
    $totalReSortTime = 0;
    $total = 0;

    $startTime = hrtime(true);
    for ($j = 0; $j < $iterations; $j++) {
        srand(1234);
        $startMemory = memory_get_usage();
        $values = unserialize($ser);
        $t2 = hrtime(true);
        $total += $values[rand()];
        $t3 = hrtime(true);
        $values[rand() + 123] = 123;
        $values[rand() + 456] = 123;
        ksort($values);
        $t4 = hrtime(true);

        $totalReadTime += $t3 - $t2;
        $totalReSortTime += $t4 - $t3;

        $endMemory = memory_get_usage();
        unset($values);
    }
    $endTime = hrtime(true);

    $totalTime = ($endTime - $startTime) / 1000000000;
    $totalReadTimeSeconds = $totalReadTime / 1000000000;
    $totalReSortTimeSeconds = $totalReSortTime / 1000000000;
    printf("Unserializing sorted array         and reading single val: n=%8d iterations=%8d memory=%8d bytes, create+destroy time=%.3f read single time = %.3f add+sort=%.3f total=%.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds - $totalReSortTimeSeconds, $totalReadTimeSeconds, $totalReSortTimeSeconds, $totalTime, $total);
}
$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 1000000],
    [4,  400000],
    [8,  200000],
    [2**10, 1000],
    [2**18, 20],
];
printf(
    "Results for php %s debug=%s with opcache enabled=%s\nThis allocates/frees each node separately, this is testing that performance does not get significantly worse\n",
    PHP_VERSION,
    PHP_DEBUG ? 'true' : 'false',
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "Testing steps:\n1. Unserialize an already sorted collection\n2. Read a value from that sorted collection\n3. Add keys to the collections and sort the collections again\n\n";
echo "(Note that StrictTreeMap unserialization is optimized for the case where inputs are already sorted, but there is still time+memory overhead to creating and destroying the individual nodes)\n";

foreach ($sizes as [$n, $iterations]) {
    bench_unserialize_sorted_set($n, $iterations);
    bench_unserialize_sorted_array($n, $iterations);
    echo "\n";
}
/*

Results for php 8.2.0-dev debug=false with opcache enabled=true
This allocates/frees each node separately, this is testing that performance does not get significantly worse
Testing steps:
1. Unserialize an already sorted collection
2. Read a value from that sorted collection
3. Add keys to the collections and sort the collections again

(Note that StrictTreeMap unserialization is optimized for the case where inputs are already sorted, but there is still time+memory overhead to creating and destroying the individual nodes)
Unserializing sorted StrictTreeMap and reading single val: n=       1 iterations= 1000000 memory=     224 bytes, create+destroy time=2.110 read single time = 0.088 add+sort=0.158 total=2.356 result=1068724585000000
Unserializing sorted array         and reading single val: n=       1 iterations= 1000000 memory=     408 bytes, create+destroy time=1.881 read single time = 0.051 add+sort=0.115 total=2.048 result=1068724585000000

Unserializing sorted StrictTreeMap and reading single val: n=       4 iterations=  400000 memory=     392 bytes, create+destroy time=0.957 read single time = 0.037 add+sort=0.064 total=1.058 result=427489834000000
Unserializing sorted array         and reading single val: n=       4 iterations=  400000 memory=     408 bytes, create+destroy time=0.809 read single time = 0.021 add+sort=0.062 total=0.891 result=427489834000000

Unserializing sorted StrictTreeMap and reading single val: n=       8 iterations=  200000 memory=     616 bytes, create+destroy time=0.560 read single time = 0.019 add+sort=0.033 total=0.612 result=213744917000000
Unserializing sorted array         and reading single val: n=       8 iterations=  200000 memory=     728 bytes, create+destroy time=0.455 read single time = 0.011 add+sort=0.052 total=0.518 result=213744917000000

Unserializing sorted StrictTreeMap and reading single val: n=    1024 iterations=    1000 memory=   57512 bytes, create+destroy time=0.108 read single time = 0.000 add+sort=0.000 total=0.109 result=1068724585000
Unserializing sorted array         and reading single val: n=    1024 iterations=    1000 memory=   82008 bytes, create+destroy time=0.055 read single time = 0.000 add+sort=0.043 total=0.099 result=1068724585000

Unserializing sorted StrictTreeMap and reading single val: n=  262144 iterations=      20 memory=14679056 bytes, create+destroy time=1.375 read single time = 0.000 add+sort=0.000 total=1.375 result=21374491700
Unserializing sorted array         and reading single val: n=  262144 iterations=      20 memory=10485872 bytes, create+destroy time=0.442 read single time = 0.000 add+sort=0.393 total=0.836 result=21374491700

 */
