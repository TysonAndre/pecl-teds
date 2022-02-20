<?php

use Teds\IntVector;
use Teds\LowMemoryVector;
use Teds\SortedIntVectorSet;
use Teds\Vector;

// @phan-file-suppress PhanPossiblyUndeclaredVariable

function create_string_keys(int $n) {
    srand(1234);
    $values = [];
    for ($i = 0; $i < $n; $i++) {
        $values[] = rand() . '_' . rand() . '.' . rand();
    }
    return $values;
}

function bench_array(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    $values = create_string_keys($n);
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
        $total += in_array($lastValue, $values, true);
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

    printf("Repeatedly unserializing, searching, reserializing with in_array():         n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}
function bench_associative_array(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = array_flip(create_string_keys($n));
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
        $total += isset($values[$lastValue]) ? 1 : 0;
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

    printf("Repeatedly unserializing, searching, reserialize associative array:         n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}
function bench_vector(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    srand(1234);
    $values = new Vector(create_string_keys($n));
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
        $total += $values->contains($lastValue) ? 1 : 0;
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

    printf("Repeatedly unserializing, searching, reserializing Vector:                  n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}
function bench_hashset(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    $rawValues = create_string_keys($n);
    $values = new Teds\StrictHashSet($rawValues);
    $lastValue = end($rawValues);
    unset($rawValues);
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
        $total += $values->contains($lastValue) ? 1 : 0;
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

    printf("Repeatedly unserializing, searching, reserializing StrictHashSet            n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}
function bench_treeset(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    $rawValues = create_string_keys($n);
    $values = new Teds\StrictTreeSet($rawValues);
    $lastValue = end($rawValues);
    unset($rawValues);
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
        $total += $values->contains($lastValue) ? 1 : 0;
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

    printf("Repeatedly unserializing, searching, reserializing StrictTreeSet            n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}

function bench_immutablesortedstringset(int $n, int $iterations) {
    $totalSearchTime = 0.0;
    $total = 0;
    $values = new Teds\ImmutableSortedStringSet(create_string_keys($n));
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
        $total += $values->contains($lastValue) ? 1 : 0;
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

    printf("Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=%8d iterations=%8d memory=%8d bytes\n => total time = %.3f seconds unserialize+free time=%.3f reserialize time = %.3f search time=%.4f result=%d\n => serialized memory=%8d bytes (%8d compressed bytes)\n",
        $n, $iterations, $endMemory - $startMemory,
        $totalTimeSeconds, $unserializeAndFreeTimeSeconds, $totalReserializeTimeSeconds, $totalSearchTimeSeconds, $total, strlen($ser), strlen(gzcompress($ser)));
}

$n = 2**20;
$iterations = 10;
$sizes = [
    [1, 4000000],
    [4, 1000000],
    [8, 500000],
    [1024, 8000],
    [2**18, 20],
];
echo "Test efficiency of different representations of sets of strings\n\n";
printf(
    "Results for php %s debug=%s with opcache enabled=%s\n",
    PHP_VERSION,
    PHP_DEBUG ? 'true' : 'false',
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "(Note that ImmutableSortedStringSet is optimized for fast unserialization and removes duplicates)\n";
echo "(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)\n";
echo "(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)\n";
printf("(This is creating sets of keys such as %s and looking up the last element in the set once)\n", create_string_keys(1)[0]);

ini_set('memory_limit', '2G');
foreach ($sizes as [$n, $iterations]) {
    bench_array($n, $iterations);
    bench_associative_array($n, $iterations);
    bench_vector($n, $iterations);
    bench_hashset($n, $iterations);
    bench_treeset($n, $iterations);
    bench_immutablesortedstringset($n, $iterations);
    echo "\n";
}
/*

Test efficiency of different representations of sets of strings

Results for php 8.2.0-dev debug=false with opcache enabled=true
(Note that ImmutableSortedStringSet is optimized for fast unserialization and removes duplicates)
(Note that there are faster serializations than unserialize() available as PECLs, e.g. igbinary/msgpack.)
(Note that unserializers pass the array of mixed values to Vector::__unserialize so it is slower to unserialize than the array of values)
(This is creating sets of keys such as 411284887_1068724585.1335968403 and looking up the last element in the set once)
Repeatedly unserializing, searching, reserializing with in_array():         n=       1 iterations= 4000000 memory=     432 bytes
 => total time = 1.593 seconds unserialize+free time=0.907 reserialize time = 0.378 search time=0.3080 result=4000000
 => serialized memory=      49 bytes (      57 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       1 iterations= 4000000 memory=     432 bytes
 => total time = 1.584 seconds unserialize+free time=0.984 reserialize time = 0.386 search time=0.2140 result=4000000
 => serialized memory=      49 bytes (      57 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       1 iterations= 4000000 memory=     128 bytes
 => total time = 2.411 seconds unserialize+free time=1.482 reserialize time = 0.679 search time=0.2510 result=4000000
 => serialized memory=      66 bytes (      73 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       1 iterations= 4000000 memory=     376 bytes
 => total time = 2.657 seconds unserialize+free time=1.678 reserialize time = 0.703 search time=0.2758 result=4000000
 => serialized memory=      73 bytes (      81 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       1 iterations= 4000000 memory=     160 bytes
 => total time = 2.514 seconds unserialize+free time=1.569 reserialize time = 0.682 search time=0.2630 result=4000000
 => serialized memory=      73 bytes (      79 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       1 iterations= 4000000 memory=     136 bytes
 => total time = 2.572 seconds unserialize+free time=1.592 reserialize time = 0.721 search time=0.2584 result=4000000
 => serialized memory=      89 bytes (      97 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=       4 iterations= 1000000 memory=     608 bytes
 => total time = 0.611 seconds unserialize+free time=0.352 reserialize time = 0.164 search time=0.0951 result=1000000
 => serialized memory=     178 bytes (     121 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       4 iterations= 1000000 memory=     608 bytes
 => total time = 0.641 seconds unserialize+free time=0.419 reserialize time = 0.167 search time=0.0555 result=1000000
 => serialized memory=     178 bytes (     119 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       4 iterations= 1000000 memory=     352 bytes
 => total time = 0.830 seconds unserialize+free time=0.509 reserialize time = 0.244 search time=0.0762 result=1000000
 => serialized memory=     195 bytes (     142 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       4 iterations= 1000000 memory=     552 bytes
 => total time = 0.911 seconds unserialize+free time=0.595 reserialize time = 0.246 search time=0.0693 result=1000000
 => serialized memory=     202 bytes (     150 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       4 iterations= 1000000 memory=     480 bytes
 => total time = 0.914 seconds unserialize+free time=0.593 reserialize time = 0.250 search time=0.0716 result=1000000
 => serialized memory=     202 bytes (     146 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       4 iterations= 1000000 memory=     256 bytes
 => total time = 0.675 seconds unserialize+free time=0.420 reserialize time = 0.183 search time=0.0713 result=1000000
 => serialized memory=     186 bytes (     157 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=       8 iterations=  500000 memory=     832 bytes
 => total time = 0.455 seconds unserialize+free time=0.259 reserialize time = 0.136 search time=0.0593 result=500000
 => serialized memory=     347 bytes (     194 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       8 iterations=  500000 memory=     832 bytes
 => total time = 0.493 seconds unserialize+free time=0.321 reserialize time = 0.144 search time=0.0279 result=500000
 => serialized memory=     347 bytes (     194 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       8 iterations=  500000 memory=     640 bytes
 => total time = 0.571 seconds unserialize+free time=0.341 reserialize time = 0.183 search time=0.0468 result=500000
 => serialized memory=     364 bytes (     216 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       8 iterations=  500000 memory=     776 bytes
 => total time = 0.653 seconds unserialize+free time=0.425 reserialize time = 0.191 search time=0.0364 result=500000
 => serialized memory=     371 bytes (     223 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       8 iterations=  500000 memory=     896 bytes
 => total time = 0.664 seconds unserialize+free time=0.420 reserialize time = 0.191 search time=0.0542 result=500000
 => serialized memory=     371 bytes (     223 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       8 iterations=  500000 memory=     448 bytes
 => total time = 0.369 seconds unserialize+free time=0.223 reserialize time = 0.108 search time=0.0380 result=500000
 => serialized memory=     311 bytes (     223 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=    1024 iterations=    8000 memory=   99512 bytes
 => total time = 0.571 seconds unserialize+free time=0.314 reserialize time = 0.208 search time=0.0495 result=8000
 => serialized memory=   45435 bytes (   20838 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=    1024 iterations=    8000 memory=   99512 bytes
 => total time = 0.715 seconds unserialize+free time=0.507 reserialize time = 0.207 search time=0.0006 result=8000
 => serialized memory=   45435 bytes (   20826 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=    1024 iterations=    8000 memory=   74936 bytes
 => total time = 0.603 seconds unserialize+free time=0.340 reserialize time = 0.220 search time=0.0441 result=8000
 => serialized memory=   45452 bytes (   20870 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=    1024 iterations=    8000 memory=   91328 bytes
 => total time = 0.708 seconds unserialize+free time=0.490 reserialize time = 0.217 search time=0.0012 result=8000
 => serialized memory=   45459 bytes (   20885 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=    1024 iterations=    8000 memory=  107704 bytes
 => total time = 0.764 seconds unserialize+free time=0.511 reserialize time = 0.251 search time=0.0019 result=8000
 => serialized memory=   45459 bytes (   20579 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=    1024 iterations=    8000 memory=   41024 bytes
 => total time = 0.074 seconds unserialize+free time=0.059 reserialize time = 0.014 search time=0.0009 result=8000
 => serialized memory=   32260 bytes (   17058 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=  262144 iterations=      20 memory=25483880 bytes
 => total time = 0.793 seconds unserialize+free time=0.323 reserialize time = 0.414 search time=0.0559 result=20
 => serialized memory=12326247 bytes ( 5159034 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=  262144 iterations=      20 memory=25483880 bytes
 => total time = 1.251 seconds unserialize+free time=0.707 reserialize time = 0.544 search time=0.0000 result=20
 => serialized memory=12326247 bytes ( 5158975 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=  262144 iterations=      20 memory=19192424 bytes
 => total time = 0.994 seconds unserialize+free time=0.437 reserialize time = 0.509 search time=0.0486 result=20
 => serialized memory=12326264 bytes ( 5159066 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=  262144 iterations=      20 memory=23386736 bytes
 => total time = 1.151 seconds unserialize+free time=0.665 reserialize time = 0.487 search time=0.0000 result=20
 => serialized memory=12326271 bytes ( 5159076 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=  262144 iterations=      20 memory=27581008 bytes
 => total time = 3.166 seconds unserialize+free time=1.856 reserialize time = 1.310 search time=0.0001 result=20
 => serialized memory=12326271 bytes ( 4847122 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=  262144 iterations=      20 memory=10342512 bytes
 => total time = 0.202 seconds unserialize+free time=0.125 reserialize time = 0.077 search time=0.0000 result=20
 => serialized memory= 8243104 bytes ( 3940007 compressed bytes)

*/
