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
 => total time = 1.751 seconds unserialize+free time=0.993 reserialize time = 0.416 search time=0.3414 result=4000000
 => serialized memory=      49 bytes (      57 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       1 iterations= 4000000 memory=     432 bytes
 => total time = 1.702 seconds unserialize+free time=1.060 reserialize time = 0.412 search time=0.2302 result=4000000
 => serialized memory=      49 bytes (      57 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       1 iterations= 4000000 memory=     152 bytes
 => total time = 2.674 seconds unserialize+free time=1.657 reserialize time = 0.739 search time=0.2788 result=4000000
 => serialized memory=      66 bytes (      73 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       1 iterations= 4000000 memory=     392 bytes
 => total time = 2.946 seconds unserialize+free time=1.863 reserialize time = 0.791 search time=0.2924 result=4000000
 => serialized memory=      73 bytes (      81 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       1 iterations= 4000000 memory=     160 bytes
 => total time = 2.726 seconds unserialize+free time=1.689 reserialize time = 0.759 search time=0.2778 result=4000000
 => serialized memory=      73 bytes (      79 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       1 iterations= 4000000 memory=     136 bytes
 => total time = 2.854 seconds unserialize+free time=1.767 reserialize time = 0.798 search time=0.2890 result=4000000
 => serialized memory=      89 bytes (      97 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=       4 iterations= 1000000 memory=     608 bytes
 => total time = 0.667 seconds unserialize+free time=0.383 reserialize time = 0.178 search time=0.1054 result=1000000
 => serialized memory=     178 bytes (     121 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       4 iterations= 1000000 memory=     608 bytes
 => total time = 0.706 seconds unserialize+free time=0.460 reserialize time = 0.184 search time=0.0615 result=1000000
 => serialized memory=     178 bytes (     119 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       4 iterations= 1000000 memory=     376 bytes
 => total time = 0.914 seconds unserialize+free time=0.562 reserialize time = 0.273 search time=0.0791 result=1000000
 => serialized memory=     195 bytes (     142 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       4 iterations= 1000000 memory=     568 bytes
 => total time = 1.017 seconds unserialize+free time=0.657 reserialize time = 0.287 search time=0.0736 result=1000000
 => serialized memory=     202 bytes (     150 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       4 iterations= 1000000 memory=     456 bytes
 => total time = 1.009 seconds unserialize+free time=0.647 reserialize time = 0.289 search time=0.0734 result=1000000
 => serialized memory=     202 bytes (     146 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       4 iterations= 1000000 memory=     256 bytes
 => total time = 0.750 seconds unserialize+free time=0.468 reserialize time = 0.204 search time=0.0784 result=1000000
 => serialized memory=     186 bytes (     157 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=       8 iterations=  500000 memory=     832 bytes
 => total time = 0.512 seconds unserialize+free time=0.288 reserialize time = 0.156 search time=0.0684 result=500000
 => serialized memory=     347 bytes (     194 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=       8 iterations=  500000 memory=     832 bytes
 => total time = 0.631 seconds unserialize+free time=0.410 reserialize time = 0.185 search time=0.0361 result=500000
 => serialized memory=     347 bytes (     194 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=       8 iterations=  500000 memory=     664 bytes
 => total time = 0.651 seconds unserialize+free time=0.388 reserialize time = 0.212 search time=0.0503 result=500000
 => serialized memory=     364 bytes (     216 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=       8 iterations=  500000 memory=     792 bytes
 => total time = 0.704 seconds unserialize+free time=0.455 reserialize time = 0.212 search time=0.0369 result=500000
 => serialized memory=     371 bytes (     223 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=       8 iterations=  500000 memory=     840 bytes
 => total time = 0.723 seconds unserialize+free time=0.457 reserialize time = 0.217 search time=0.0491 result=500000
 => serialized memory=     371 bytes (     223 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=       8 iterations=  500000 memory=     448 bytes
 => total time = 0.408 seconds unserialize+free time=0.246 reserialize time = 0.120 search time=0.0422 result=500000
 => serialized memory=     311 bytes (     223 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=    1024 iterations=    8000 memory=   99512 bytes
 => total time = 0.623 seconds unserialize+free time=0.342 reserialize time = 0.227 search time=0.0534 result=8000
 => serialized memory=   45435 bytes (   20838 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=    1024 iterations=    8000 memory=   99512 bytes
 => total time = 0.784 seconds unserialize+free time=0.556 reserialize time = 0.228 search time=0.0007 result=8000
 => serialized memory=   45435 bytes (   20826 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=    1024 iterations=    8000 memory=   74960 bytes
 => total time = 0.641 seconds unserialize+free time=0.374 reserialize time = 0.241 search time=0.0255 result=8000
 => serialized memory=   45452 bytes (   20870 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=    1024 iterations=    8000 memory=   91344 bytes
 => total time = 0.785 seconds unserialize+free time=0.545 reserialize time = 0.239 search time=0.0012 result=8000
 => serialized memory=   45459 bytes (   20885 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=    1024 iterations=    8000 memory=   99520 bytes
 => total time = 0.833 seconds unserialize+free time=0.557 reserialize time = 0.274 search time=0.0022 result=8000
 => serialized memory=   45459 bytes (   20579 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=    1024 iterations=    8000 memory=   41024 bytes
 => total time = 0.074 seconds unserialize+free time=0.062 reserialize time = 0.011 search time=0.0010 result=8000
 => serialized memory=   32260 bytes (   17058 compressed bytes)

Repeatedly unserializing, searching, reserializing with in_array():         n=  262144 iterations=      20 memory=25483880 bytes
 => total time = 0.863 seconds unserialize+free time=0.346 reserialize time = 0.457 search time=0.0596 result=20
 => serialized memory=12326247 bytes ( 5159034 compressed bytes)
Repeatedly unserializing, searching, reserialize associative array:         n=  262144 iterations=      20 memory=25483880 bytes
 => total time = 1.338 seconds unserialize+free time=0.740 reserialize time = 0.598 search time=0.0000 result=20
 => serialized memory=12326247 bytes ( 5158975 compressed bytes)
Repeatedly unserializing, searching, reserializing Vector:                  n=  262144 iterations=      20 memory=19192448 bytes
 => total time = 1.052 seconds unserialize+free time=0.468 reserialize time = 0.545 search time=0.0390 result=20
 => serialized memory=12326264 bytes ( 5159066 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictHashSet            n=  262144 iterations=      20 memory=23386752 bytes
 => total time = 1.207 seconds unserialize+free time=0.704 reserialize time = 0.503 search time=0.0000 result=20
 => serialized memory=12326271 bytes ( 5159076 compressed bytes)
Repeatedly unserializing, searching, reserializing StrictTreeSet            n=  262144 iterations=      20 memory=25483864 bytes
 => total time = 3.087 seconds unserialize+free time=1.787 reserialize time = 1.300 search time=0.0001 result=20
 => serialized memory=12326271 bytes ( 4847122 compressed bytes)
Repeatedly unserializing, searching, reserializing ImmutableSortedStringSet n=  262144 iterations=      20 memory=10342512 bytes
 => total time = 0.216 seconds unserialize+free time=0.132 reserialize time = 0.084 search time=0.0000 result=20
 => serialized memory= 8243104 bytes ( 3940007 compressed bytes)


*/
