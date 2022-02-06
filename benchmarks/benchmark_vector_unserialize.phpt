<?php

use Teds\LowMemoryVector;
use Teds\Vector;

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
    $totalReserializeTimeSeconds = $totalSearchTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing array:           n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.3f result=%d\n => serialized memory=%8d bytes\n",
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
    $totalReserializeTimeSeconds = $totalSearchTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing Vector:          n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.3f result=%d\n => serialized memory=%8d bytes\n",
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
    $totalReserializeTimeSeconds = $totalSearchTime / 1000000000;
    $unserializeAndFreeTimeSeconds = $totalTimeSeconds - $totalSearchTimeSeconds - $totalReserializeTimeSeconds;

    printf("Repeatedly unserializing, searching, reserializing LowMemoryVector: n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.3f result=%d\n => serialized memory=%8d bytes\n",
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
echo "(Note that LowMemoryVector has specialized representations for collections of int with smaller in-memory and serialized representations, and can skip garbage collection if using a specialized type where none of the elements are reference counted)\n";
echo "(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)\n";
echo "(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)\n\n";

foreach ($sizes as [$n, $iterations]) {
    bench_array($n, $iterations);
    bench_vector($n, $iterations);
    bench_low_memory_vector($n, $iterations);
    echo "\n";
}
/*
Test efficiency of different representations of lists of 32-bit integers

Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that LowMemoryVector has specialized representations for collections of int with smaller in-memory and serialized representations, and can skip garbage collection if using a specialized type where none of the elements are reference counted)
(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)
(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)

Repeatedly unserializing, searching, reserializing array:           n=       1 iterations= 4000000 memory=     376 bytes
 => total time = 1.436 seconds unserialize+free time=1.084 reserialize time = 0.176 search time=0.176 result=0
 => serialized memory=      22 bytes
Repeatedly unserializing, searching, reserializing Vector:          n=       1 iterations= 4000000 memory=      80 bytes
 => total time = 2.406 seconds unserialize+free time=1.948 reserialize time = 0.229 search time=0.229 result=0
 => serialized memory=      39 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector: n=       1 iterations= 4000000 memory=      88 bytes
 => total time = 2.694 seconds unserialize+free time=2.233 reserialize time = 0.231 search time=0.231 result=0
 => serialized memory=      55 bytes

Repeatedly unserializing, searching, reserializing array:           n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.564 seconds unserialize+free time=0.466 reserialize time = 0.049 search time=0.049 result=3000000
 => serialized memory=      73 bytes
Repeatedly unserializing, searching, reserializing Vector:          n=       4 iterations= 1000000 memory=     128 bytes
 => total time = 0.823 seconds unserialize+free time=0.691 reserialize time = 0.066 search time=0.066 result=3000000
 => serialized memory=      90 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector: n=       4 iterations= 1000000 memory=      96 bytes
 => total time = 0.664 seconds unserialize+free time=0.548 reserialize time = 0.058 search time=0.058 result=3000000
 => serialized memory=      68 bytes

Repeatedly unserializing, searching, reserializing array:           n=       8 iterations=  500000 memory=     376 bytes
 => total time = 0.426 seconds unserialize+free time=0.371 reserialize time = 0.028 search time=0.028 result=3500000
 => serialized memory=     140 bytes
Repeatedly unserializing, searching, reserializing Vector:          n=       8 iterations=  500000 memory=     192 bytes
 => total time = 0.569 seconds unserialize+free time=0.489 reserialize time = 0.040 search time=0.040 result=3500000
 => serialized memory=     157 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector: n=       8 iterations=  500000 memory=     112 bytes
 => total time = 0.346 seconds unserialize+free time=0.283 reserialize time = 0.032 search time=0.032 result=3500000
 => serialized memory=      84 bytes

Repeatedly unserializing, searching, reserializing array:           n=    1024 iterations=    8000 memory=   41016 bytes
 => total time = 0.682 seconds unserialize+free time=0.653 reserialize time = 0.014 search time=0.014 result=8184000
 => serialized memory=   18850 bytes
Repeatedly unserializing, searching, reserializing Vector:          n=    1024 iterations=    8000 memory=   16448 bytes
 => total time = 0.688 seconds unserialize+free time=0.636 reserialize time = 0.026 search time=0.026 result=8184000
 => serialized memory=   18867 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector: n=    1024 iterations=    8000 memory=    4176 bytes
 => total time = 0.013 seconds unserialize+free time=0.002 reserialize time = 0.005 search time=0.005 result=8184000
 => serialized memory=    4150 bytes

Repeatedly unserializing, searching, reserializing array:           n= 1048576 iterations=      20 memory=41943120 bytes
 => total time = 2.555 seconds unserialize+free time=2.455 reserialize time = 0.050 search time=0.050 result=20971500
 => serialized memory=22462741 bytes
Repeatedly unserializing, searching, reserializing Vector:          n= 1048576 iterations=      20 memory=16777304 bytes
 => total time = 3.074 seconds unserialize+free time=2.936 reserialize time = 0.069 search time=0.069 result=20971500
 => serialized memory=22462758 bytes
Repeatedly unserializing, searching, reserializing LowMemoryVector: n= 1048576 iterations=      20 memory= 4194408 bytes
 => total time = 0.152 seconds unserialize+free time=0.126 reserialize time = 0.013 search time=0.013 result=20971500
 => serialized memory= 4194361 bytes

*/
