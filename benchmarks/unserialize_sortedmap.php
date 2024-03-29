<?php

use Teds\StrictSortedVectorMap;
use Teds\StrictTreeMap;

// @phan-file-suppress PhanPossiblyUndeclaredVariable

function bench_unserialize_sorted_map(int $n, int $iterations) {
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
    printf("Unserializing sorted StrictTreeMap         and reading single val: n=%8d iterations=%8d memory=%8d bytes, create+destroy time=%.3f read single time = %.3f add+sort=%.3f total=%.3f result=%d\n",
        $n, $iterations, $endMemory - $startMemory, $totalTime - $totalReadTimeSeconds - $totalReSortTimeSeconds, $totalReadTimeSeconds, $totalReSortTimeSeconds, $totalTime, $total);
}
function bench_unserialize_sorted_vector_map(int $n, int $iterations) {
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[rand()] = rand();
    }
    $original = new StrictSortedVectorMap($values);
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
    printf("Unserializing sorted StrictSortedVectorMap and reading single val: n=%8d iterations=%8d memory=%8d bytes, create+destroy time=%.3f read single time = %.3f add+sort=%.3f total=%.3f result=%d\n",
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
    printf("Unserializing sorted array                 and reading single val: n=%8d iterations=%8d memory=%8d bytes, create+destroy time=%.3f read single time = %.3f add+sort=%.3f total=%.3f result=%d\n",
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
    "Results for php %s debug=%s with opcache enabled=%s\n",
    PHP_VERSION,
    PHP_DEBUG ? 'true' : 'false',
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "Testing steps:\n1. Unserialize an already sorted collection\n2. Read a value from that sorted collection\n3. Add keys to the collections and sort the collections again\n\n";
echo "(Note that StrictTreeMap unserialization is optimized for the case where inputs are already sorted, but there is still time+memory overhead to creating and destroying the individual nodes)\n";

foreach ($sizes as [$n, $iterations]) {
    bench_unserialize_sorted_map($n, $iterations);
    bench_unserialize_sorted_vector_map($n, $iterations);
    bench_unserialize_sorted_array($n, $iterations);
    echo "\n";
}
/*

Results for php 8.2.0-dev debug=false with opcache enabled=true
Testing steps:
1. Unserialize an already sorted collection
2. Read a value from that sorted collection
3. Add keys to the collections and sort the collections again

(Note that StrictTreeMap unserialization is optimized for the case where inputs are already sorted, but there is still time+memory overhead to creating and destroying the individual nodes)
Unserializing sorted StrictTreeMap         and reading single val: n=       1 iterations= 1000000 memory=     232 bytes, create+destroy time=2.267 read single time = 0.065 add+sort=0.103 total=2.435 result=1068724585000000
Unserializing sorted StrictSortedVectorMap and reading single val: n=       1 iterations= 1000000 memory=     192 bytes, create+destroy time=2.454 read single time = 0.075 add+sort=0.122 total=2.651 result=1068724585000000
Unserializing sorted array                 and reading single val: n=       1 iterations= 1000000 memory=     408 bytes, create+destroy time=2.032 read single time = 0.055 add+sort=0.120 total=2.208 result=1068724585000000

Unserializing sorted StrictTreeMap         and reading single val: n=       4 iterations=  400000 memory=     400 bytes, create+destroy time=1.052 read single time = 0.029 add+sort=0.042 total=1.123 result=427489834000000
Unserializing sorted StrictSortedVectorMap and reading single val: n=       4 iterations=  400000 memory=     320 bytes, create+destroy time=1.095 read single time = 0.033 add+sort=0.074 total=1.201 result=427489834000000
Unserializing sorted array                 and reading single val: n=       4 iterations=  400000 memory=     408 bytes, create+destroy time=0.887 read single time = 0.022 add+sort=0.064 total=0.973 result=427489834000000

Unserializing sorted StrictTreeMap         and reading single val: n=       8 iterations=  200000 memory=     624 bytes, create+destroy time=0.619 read single time = 0.016 add+sort=0.023 total=0.657 result=213744917000000
Unserializing sorted StrictSortedVectorMap and reading single val: n=       8 iterations=  200000 memory=     576 bytes, create+destroy time=0.644 read single time = 0.017 add+sort=0.040 total=0.701 result=213744917000000
Unserializing sorted array                 and reading single val: n=       8 iterations=  200000 memory=     728 bytes, create+destroy time=0.514 read single time = 0.012 add+sort=0.057 total=0.583 result=213744917000000

Unserializing sorted StrictTreeMap         and reading single val: n=    1024 iterations=    1000 memory=   57520 bytes, create+destroy time=0.134 read single time = 0.000 add+sort=0.000 total=0.134 result=1068724585000
Unserializing sorted StrictSortedVectorMap and reading single val: n=    1024 iterations=    1000 memory=   65600 bytes, create+destroy time=0.128 read single time = 0.000 add+sort=0.001 total=0.129 result=1068724585000
Unserializing sorted array                 and reading single val: n=    1024 iterations=    1000 memory=   82008 bytes, create+destroy time=0.073 read single time = 0.000 add+sort=0.056 total=0.128 result=1068724585000

Unserializing sorted StrictTreeMap         and reading single val: n=  262144 iterations=      20 memory=14679064 bytes, create+destroy time=1.405 read single time = 0.000 add+sort=0.000 total=1.405 result=21374491700
Unserializing sorted StrictSortedVectorMap and reading single val: n=  262144 iterations=      20 memory= 8388696 bytes, create+destroy time=0.859 read single time = 0.000 add+sort=0.007 total=0.867 result=21374491700
Unserializing sorted array                 and reading single val: n=  262144 iterations=      20 memory=10485872 bytes, create+destroy time=0.468 read single time = 0.000 add+sort=0.453 total=0.921 result=21374491700

 */
