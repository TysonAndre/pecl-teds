<?php

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

    printf("Repeatedly unserializing, searching, reserializing with in_array():   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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

    printf("Repeatedly unserializing, searching, reserialize associative array:   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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

    printf("Repeatedly unserializing, searching, reserializing Vector:            n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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

    printf("Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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

    printf("Repeatedly unserializing, searching, reserializing IntVector          n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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

    printf("Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes\n",
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
 => total time = 1.415 seconds unserialize+free time=0.854 reserialize time = 0.391 search time=0.1702 result=0
 => serialized memory=      22 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       1 iterations= 4000000 memory=     376 bytes
 => total time = 1.464 seconds unserialize+free time=0.891 reserialize time = 0.395 search time=0.1780 result=0
 => serialized memory=      22 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       1 iterations= 4000000 memory=      72 bytes
 => total time = 2.383 seconds unserialize+free time=1.462 reserialize time = 0.694 search time=0.2272 result=0
 => serialized memory=      39 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       1 iterations= 4000000 memory=      72 bytes
 => total time = 2.607 seconds unserialize+free time=1.620 reserialize time = 0.767 search time=0.2204 result=0
 => serialized memory=      55 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.593 seconds unserialize+free time=1.597 reserialize time = 0.770 search time=0.2262 result=0
 => serialized memory=      49 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.606 seconds unserialize+free time=1.609 reserialize time = 0.767 search time=0.2304 result=0
 => serialized memory=      58 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.557 seconds unserialize+free time=0.333 reserialize time = 0.176 search time=0.0480 result=3000000
 => serialized memory=      73 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.572 seconds unserialize+free time=0.343 reserialize time = 0.184 search time=0.0453 result=3000000
 => serialized memory=      73 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       4 iterations= 1000000 memory=     120 bytes
 => total time = 0.829 seconds unserialize+free time=0.494 reserialize time = 0.260 search time=0.0741 result=3000000
 => serialized memory=      90 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       4 iterations= 1000000 memory=      80 bytes
 => total time = 0.656 seconds unserialize+free time=0.406 reserialize time = 0.193 search time=0.0571 result=3000000
 => serialized memory=      68 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.652 seconds unserialize+free time=0.400 reserialize time = 0.195 search time=0.0580 result=3000000
 => serialized memory=      62 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.656 seconds unserialize+free time=0.404 reserialize time = 0.194 search time=0.0585 result=3000000
 => serialized memory=      71 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=       8 iterations=  500000 memory=     376 bytes
 => total time = 0.433 seconds unserialize+free time=0.250 reserialize time = 0.155 search time=0.0280 result=3500000
 => serialized memory=     140 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=       8 iterations=  500000 memory=     376 bytes
 => total time = 0.461 seconds unserialize+free time=0.263 reserialize time = 0.175 search time=0.0239 result=3500000
 => serialized memory=     140 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=       8 iterations=  500000 memory=     184 bytes
 => total time = 0.558 seconds unserialize+free time=0.331 reserialize time = 0.189 search time=0.0382 result=3500000
 => serialized memory=     157 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=       8 iterations=  500000 memory=      96 bytes
 => total time = 0.342 seconds unserialize+free time=0.211 reserialize time = 0.101 search time=0.0306 result=3500000
 => serialized memory=      84 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.340 seconds unserialize+free time=0.207 reserialize time = 0.101 search time=0.0314 result=3500000
 => serialized memory=      78 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.339 seconds unserialize+free time=0.208 reserialize time = 0.100 search time=0.0308 result=3500000
 => serialized memory=      87 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n=    1024 iterations=    8000 memory=   41016 bytes
 => total time = 0.670 seconds unserialize+free time=0.361 reserialize time = 0.295 search time=0.0143 result=8184000
 => serialized memory=   18850 bytes
Repeatedly unserializing, searching, reserialize associative array:   n=    1024 iterations=    8000 memory=   41016 bytes
 => total time = 0.685 seconds unserialize+free time=0.379 reserialize time = 0.305 search time=0.0004 result=8184000
 => serialized memory=   18850 bytes
Repeatedly unserializing, searching, reserializing Vector:            n=    1024 iterations=    8000 memory=   16440 bytes
 => total time = 0.674 seconds unserialize+free time=0.379 reserialize time = 0.271 search time=0.0237 result=8184000
 => serialized memory=   18867 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n=    1024 iterations=    8000 memory=    4160 bytes
 => total time = 0.010 seconds unserialize+free time=0.004 reserialize time = 0.003 search time=0.0030 result=8184000
 => serialized memory=    4150 bytes
Repeatedly unserializing, searching, reserializing IntVector          n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.012 seconds unserialize+free time=0.004 reserialize time = 0.003 search time=0.0052 result=8184000
 => serialized memory=    4144 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.008 seconds unserialize+free time=0.004 reserialize time = 0.003 search time=0.0006 result=8184000
 => serialized memory=    4153 bytes

Repeatedly unserializing, searching, reserializing with in_array():   n= 1048576 iterations=      20 memory=41943120 bytes
 => total time = 2.546 seconds unserialize+free time=1.259 reserialize time = 1.236 search time=0.0504 result=20971500
 => serialized memory=22462741 bytes
Repeatedly unserializing, searching, reserialize associative array:   n= 1048576 iterations=      20 memory=41943120 bytes
 => total time = 3.924 seconds unserialize+free time=2.662 reserialize time = 1.263 search time=0.0000 result=20971500
 => serialized memory=22456955 bytes
Repeatedly unserializing, searching, reserializing Vector:            n= 1048576 iterations=      20 memory=16777296 bytes
 => total time = 3.032 seconds unserialize+free time=1.448 reserialize time = 1.521 search time=0.0629 result=20971500
 => serialized memory=22462758 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector:   n= 1048576 iterations=      20 memory= 4194392 bytes
 => total time = 0.147 seconds unserialize+free time=0.070 reserialize time = 0.069 search time=0.0076 result=20971500
 => serialized memory= 4194361 bytes
Repeatedly unserializing, searching, reserializing IntVector          n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.153 seconds unserialize+free time=0.070 reserialize time = 0.070 search time=0.0125 result=20971500
 => serialized memory= 4194355 bytes
Repeatedly unserializing, searching, reserializing SortedIntVectorSet n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.140 seconds unserialize+free time=0.070 reserialize time = 0.069 search time=0.0000 result=20966060
 => serialized memory= 4193276 bytes

*/
