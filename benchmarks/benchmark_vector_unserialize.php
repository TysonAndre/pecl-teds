<?php

use Teds\ImmutableSortedIntSet;
use Teds\IntVector;
use Teds\LowMemoryVector;
use Teds\SortedIntVectorSet;
use Teds\Vector;

// @phan-file-suppress PhanPossiblyUndeclaredVariable

function bench_array(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $lastValue = $values[$n - 1];
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += array_search($lastValue, $values);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing with in_array():   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_associative_array(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[rand()] = $i;
    }
    $lastValue = array_key_last($values);
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values[$lastValue];
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserialize associative array:   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_vector(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = new Vector();
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $lastValue = $values[$n - 1];
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values->indexOf($lastValue);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing Vector:            n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_low_memory_vector(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = new LowMemoryVector();
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $lastValue = $values[$n - 1];
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values->indexOf($lastValue);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_int_vector(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = new IntVector();
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $lastValue = $values[$n - 1];
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values->indexOf($lastValue);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing IntVector          n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_sortedintvectorset(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $values = new SortedIntVectorSet($values); // NOTE: this uses insertion sort by design to reduce memory usage and speed up unserialization, and is faster to construct all at once.
    $lastValue = $values->last();
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values->indexOf($lastValue);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}
function bench_immutablesortedintset(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand();
    }
    $values = new ImmutableSortedIntSet($values); // NOTE: this uses insertion sort by design to reduce memory usage and speed up unserialization, and is faster to construct all at once.
    $lastValue = $values->last();
    $ser = serialize($values);
    if (!is_string($ser)) { throw new RuntimeException("failed to serialize\n"); }
    unset($values);

    $totalSearchTime = 0;
    $totalReserializeTime = 0;
    $totalSearchTime = 0;
    $startTime = hrtime(true);

    for ($j = 0; $j < $iterations; $j++) {
        unset($values);
        $startMemory = memory_get_usage();
        $startUnserializeTime = hrtime(true);
        $values = unserialize($ser);
        $endMemory = memory_get_usage();

        $startSingleSearchTime = hrtime(true);
        $total += $values->indexOf($lastValue);
        $startReserializeTime = hrtime(true);

        serialize($values);  // serialize, not used

        $endReserializeTime = hrtime(true);

        $totalSearchTime += $startReserializeTime - $startSingleSearchTime;
        $totalReserializeTime += $endReserializeTime - $startReserializeTime;
    }
    $endTime = hrtime(true);

    $totalTimeSeconds = ($endTime - $startTime) / 1000000000;
    $totalSearchTimeSeconds = $totalSearchTime / 1000000000;
    $totalReserializeTimeSeconds = $totalReserializeTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.4f seconds unserialize+free time=%.4f reserialize time = %.4f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser));
}

$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 4000000],
    [4, 1000000],
    [8, 500000],
    [1024, 8000],
    [4096, 2000],
    [2**17, 160],
    [2**18, 80],
    [2**20, 20],
];
echo "Test efficiency of different representations of lists of 32-bit integers\n\n";
printf(
    "Results for php %s debug=%s with opcache enabled=%s\n",
    PHP_VERSION,
    PHP_DEBUG ? 'true' : 'false',
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "(Note that LowMemoryVector/IntVector has specialized representations for collections of int with smaller in-memory and serialized representations, and can skip garbage collection if using a specialized type where none of the elements are reference counted)\n";
echo "(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)\n";
echo "(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)\n";
echo "(Note that SortedIntVectorSet removes duplicates)\n\n";

foreach ($sizes as [$n, $iterations]) {
    bench_array($n, $iterations);
    bench_associative_array($n, $iterations);
    bench_vector($n, $iterations);
    bench_low_memory_vector($n, $iterations);
    bench_int_vector($n, $iterations);
    bench_sortedintvectorset($n, $iterations);
    bench_immutablesortedintset($n, $iterations);
    echo "\n";
}
/*

Test efficiency of different representations of lists of 32-bit integers

Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that LowMemoryVector/IntVector has specialized representations for collections of int with smaller in-memory and serialized representations, and can skip garbage collection if using a specialized type where none of the elements are reference counted)
(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)
(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)
(Note that SortedIntVectorSet removes duplicates)

Repeatedly unserializing, searching, reserializing with in_array():   n=       1 iterations= 4000000 memory=     376 bytes
 => total time = 1.5821 seconds unserialize+free time=0.9593 reserialize time = 0.4351 search time=0.1876 result=0
 => serialized memory=      22 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       1 iterations= 4000000 memory=     376 bytes
 => total time = 1.6266 seconds unserialize+free time=0.9877 reserialize time = 0.4410 search time=0.1979 result=0
 => serialized memory=      22 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       1 iterations= 4000000 memory=      96 bytes
 => total time = 2.6656 seconds unserialize+free time=1.6472 reserialize time = 0.7703 search time=0.2480 result=0
 => serialized memory=      39 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.9319 seconds unserialize+free time=1.8262 reserialize time = 0.8554 search time=0.2503 result=0
 => serialized memory=      55 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.9107 seconds unserialize+free time=1.7954 reserialize time = 0.8641 search time=0.2512 result=0
 => serialized memory=      49 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.9817 seconds unserialize+free time=1.8538 reserialize time = 0.8681 search time=0.2599 result=0
 => serialized memory=      58 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.9266 seconds unserialize+free time=1.8143 reserialize time = 0.8579 search time=0.2545 result=0
 => serialized memory=      61 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.6141 seconds unserialize+free time=0.3686 reserialize time = 0.1929 search time=0.0526 result=3000000
 => serialized memory=      73 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.6295 seconds unserialize+free time=0.3756 reserialize time = 0.2046 search time=0.0493 result=3000000
 => serialized memory=      73 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       4 iterations= 1000000 memory=     144 bytes
 => total time = 1.0703 seconds unserialize+free time=0.6584 reserialize time = 0.3305 search time=0.0814 result=3000000
 => serialized memory=      90 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.7309 seconds unserialize+free time=0.4539 reserialize time = 0.2131 search time=0.0639 result=3000000
 => serialized memory=      68 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.7318 seconds unserialize+free time=0.4500 reserialize time = 0.2173 search time=0.0645 result=3000000
 => serialized memory=      62 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.7374 seconds unserialize+free time=0.4577 reserialize time = 0.2147 search time=0.0650 result=3000000
 => serialized memory=      71 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.7367 seconds unserialize+free time=0.4564 reserialize time = 0.2155 search time=0.0648 result=3000000
 => serialized memory=      74 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=       8 iterations=  500000 memory=     376 bytes
 => total time = 0.4642 seconds unserialize+free time=0.2691 reserialize time = 0.1654 search time=0.0297 result=3500000
 => serialized memory=     140 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       8 iterations=  500000 memory=     376 bytes
 => total time = 0.4738 seconds unserialize+free time=0.2689 reserialize time = 0.1802 search time=0.0247 result=3500000
 => serialized memory=     140 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       8 iterations=  500000 memory=     208 bytes
 => total time = 0.6076 seconds unserialize+free time=0.3650 reserialize time = 0.2051 search time=0.0376 result=3500000
 => serialized memory=     157 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.3869 seconds unserialize+free time=0.2405 reserialize time = 0.1121 search time=0.0344 result=3500000
 => serialized memory=      84 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.3818 seconds unserialize+free time=0.2343 reserialize time = 0.1129 search time=0.0346 result=3500000
 => serialized memory=      78 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.3865 seconds unserialize+free time=0.2401 reserialize time = 0.1118 search time=0.0345 result=3500000
 => serialized memory=      87 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.3855 seconds unserialize+free time=0.2389 reserialize time = 0.1124 search time=0.0342 result=3500000
 => serialized memory=      90 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=    1024 iterations=    8000 memory=   41016 bytes
 => total time = 0.7397 seconds unserialize+free time=0.3968 reserialize time = 0.3272 search time=0.0156 result=8184000
 => serialized memory=   18850 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=    1024 iterations=    8000 memory=   41016 bytes
 => total time = 0.7487 seconds unserialize+free time=0.4134 reserialize time = 0.3348 search time=0.0005 result=8184000
 => serialized memory=   18850 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=    1024 iterations=    8000 memory=   16464 bytes
 => total time = 0.7269 seconds unserialize+free time=0.4181 reserialize time = 0.2977 search time=0.0111 result=8184000
 => serialized memory=   18867 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.0113 seconds unserialize+free time=0.0049 reserialize time = 0.0031 search time=0.0033 result=8184000
 => serialized memory=    4150 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.0112 seconds unserialize+free time=0.0048 reserialize time = 0.0031 search time=0.0033 result=8184000
 => serialized memory=    4144 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.0140 seconds unserialize+free time=0.0102 reserialize time = 0.0032 search time=0.0007 result=8184000
 => serialized memory=    4153 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.0124 seconds unserialize+free time=0.0085 reserialize time = 0.0032 search time=0.0006 result=8184000
 => serialized memory=    4156 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=    4096 iterations=    2000 memory=  163896 bytes
 => total time = 0.7584 seconds unserialize+free time=0.3993 reserialize time = 0.3437 search time=0.0153 result=8190000
 => serialized memory=   78683 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=    4096 iterations=    2000 memory=  163896 bytes
 => total time = 0.7704 seconds unserialize+free time=0.4283 reserialize time = 0.3419 search time=0.0001 result=8190000
 => serialized memory=   78683 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=    4096 iterations=    2000 memory=   65616 bytes
 => total time = 0.7386 seconds unserialize+free time=0.4222 reserialize time = 0.3058 search time=0.0105 result=8190000
 => serialized memory=   78700 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=    4096 iterations=    2000 memory=   16464 bytes
 => total time = 0.0080 seconds unserialize+free time=0.0025 reserialize time = 0.0028 search time=0.0027 result=8190000
 => serialized memory=   16439 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=    4096 iterations=    2000 memory=   16464 bytes
 => total time = 0.0079 seconds unserialize+free time=0.0025 reserialize time = 0.0027 search time=0.0027 result=8190000
 => serialized memory=   16433 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=    4096 iterations=    2000 memory=   16464 bytes
 => total time = 0.0103 seconds unserialize+free time=0.0076 reserialize time = 0.0025 search time=0.0002 result=8190000
 => serialized memory=   16442 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=    4096 iterations=    2000 memory=   20560 bytes
 => total time = 0.0072 seconds unserialize+free time=0.0053 reserialize time = 0.0017 search time=0.0002 result=8190000
 => serialized memory=   16445 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=  131072 iterations=     160 memory= 5242960 bytes
 => total time = 2.6148 seconds unserialize+free time=1.3130 reserialize time = 1.2493 search time=0.0525 result=20971360
 => serialized memory= 2704563 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=  131072 iterations=     160 memory= 5242960 bytes
 => total time = 2.8156 seconds unserialize+free time=1.5666 reserialize time = 1.2488 search time=0.0001 result=20971360
 => serialized memory= 2704523 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=  131072 iterations=     160 memory= 2097256 bytes
 => total time = 2.7951 seconds unserialize+free time=1.5007 reserialize time = 1.2599 search time=0.0345 result=20971360
 => serialized memory= 2704580 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=  131072 iterations=     160 memory=  524368 bytes
 => total time = 0.0188 seconds unserialize+free time=0.0062 reserialize time = 0.0062 search time=0.0064 result=20971360
 => serialized memory=  524344 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=  131072 iterations=     160 memory=  524368 bytes
 => total time = 0.0188 seconds unserialize+free time=0.0062 reserialize time = 0.0062 search time=0.0064 result=20971360
 => serialized memory=  524338 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=  131072 iterations=     160 memory=  524368 bytes
 => total time = 0.0251 seconds unserialize+free time=0.0187 reserialize time = 0.0063 search time=0.0000 result=20971040
 => serialized memory=  524339 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=  131072 iterations=     160 memory=  528464 bytes
 => total time = 0.0153 seconds unserialize+free time=0.0120 reserialize time = 0.0033 search time=0.0000 result=20971040
 => serialized memory=  524342 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=  262144 iterations=      80 memory=10485840 bytes
 => total time = 2.7217 seconds unserialize+free time=1.3314 reserialize time = 1.3348 search time=0.0555 result=20971440
 => serialized memory= 5520026 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=  262144 iterations=      80 memory=10485840 bytes
 => total time = 2.9587 seconds unserialize+free time=1.7862 reserialize time = 1.1724 search time=0.0001 result=20971440
 => serialized memory= 5519692 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=  262144 iterations=      80 memory= 4194408 bytes
 => total time = 2.7740 seconds unserialize+free time=1.5357 reserialize time = 1.2060 search time=0.0323 result=20971440
 => serialized memory= 5520043 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=  262144 iterations=      80 memory= 1048656 bytes
 => total time = 0.0756 seconds unserialize+free time=0.0345 reserialize time = 0.0348 search time=0.0063 result=20971440
 => serialized memory= 1048633 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=  262144 iterations=      80 memory= 1048656 bytes
 => total time = 0.0762 seconds unserialize+free time=0.0346 reserialize time = 0.0352 search time=0.0064 result=20971440
 => serialized memory= 1048627 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=  262144 iterations=      80 memory= 1048656 bytes
 => total time = 0.0831 seconds unserialize+free time=0.0474 reserialize time = 0.0357 search time=0.0000 result=20970160
 => serialized memory= 1048572 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n=  262144 iterations=      80 memory= 1048656 bytes
 => total time = 0.0166 seconds unserialize+free time=0.0118 reserialize time = 0.0048 search time=0.0000 result=20970160
 => serialized memory= 1048575 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n= 1048576 iterations=      20 memory=41943120 bytes
 => total time = 2.9284 seconds unserialize+free time=1.3898 reserialize time = 1.4831 search time=0.0555 result=20971500
 => serialized memory=22462741 bytes
Repeatedly unserializing, searching, reserialize associative array:   n= 1048576 iterations=      20 memory=41943120 bytes
 => total time = 4.3191 seconds unserialize+free time=2.8770 reserialize time = 1.4421 search time=0.0000 result=20971500
 => serialized memory=22456955 bytes
Repeatedly unserializing, searching, reserializing Vector:            n= 1048576 iterations=      20 memory=16777320 bytes
 => total time = 3.3039 seconds unserialize+free time=1.6027 reserialize time = 1.6689 search time=0.0323 result=20971500
 => serialized memory=22462758 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.1607 seconds unserialize+free time=0.0762 reserialize time = 0.0758 search time=0.0087 result=20971500
 => serialized memory= 4194361 bytes
Repeatedly unserializing, searching, reserializing IntVector          n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.1605 seconds unserialize+free time=0.0761 reserialize time = 0.0759 search time=0.0084 result=20971500
 => serialized memory= 4194355 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.1651 seconds unserialize+free time=0.0887 reserialize time = 0.0764 search time=0.0000 result=20966060
 => serialized memory= 4193276 bytes
Repeatedly unserializing, searching, reser... ImmutableSortedIntSet   n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.0851 seconds unserialize+free time=0.0477 reserialize time = 0.0374 search time=0.0000 result=20966060
 => serialized memory= 4193279 bytes

*/
