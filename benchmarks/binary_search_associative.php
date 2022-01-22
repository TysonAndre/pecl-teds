<?php

use function Teds\binary_search;

function create_key(int $i): string {
    return sprintf("k%12d", $i);
}

function bench_binary_search_associative(int $n, int $iterations) {
    gc_collect_cycles();
    $haystack = [];
    srand(1234);
    for ($i = 0; $i < $n; $i++) {
        $haystack[$i * 100] = rand();
    }
    asort($haystack);
    $result1 = 0;
    $result2 = 0;

    $t1 = hrtime(true);
    srand(1234567);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = rand();
        // key is the key of the first value <= $needle
        $result1 += (binary_search($haystack, $needle)['key']) ?? 0;
    }
    $t2 = hrtime(true);
    srand(1234567);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = rand();
        $closest = 0;
        // key is the key of the first value <= $needle
        foreach ($haystack as $id => $value) {
            if ($value > $needle) {
                break;
            }
            $closest = $id;
        }
        $result2 += $closest;
    }
    $t3 = hrtime(true);
    $duration1 = ($t2 - $t1)/1e9;
    $duration2 = ($t3 - $t2)/1e9;
    printf("binary_search associative search n=%8d iterations=%8d results=[%10d, %10d] binary_search=%.4f manual search=%.4f time=%.4fx\n", $n, $iterations, $result1, $result2, $duration1, $duration2, $duration1/$duration2);
}
$iterations = 10;
$sizes = [
    [2**20,  100],
    [1000, 80000],
    [100, 800000],
    [8,  5000000],
    [4, 10000000],
    [2, 20000000],
];
printf(
    "Results for php %s debug=%s with opcache enabled=%s\n\n",
    PHP_VERSION,
    json_encode(PHP_DEBUG),
    json_encode(function_exists('opcache_get_status') && (opcache_get_status(false)['opcache_enabled'] ?? false))
);
echo "Testing repeatedly looking for the key of the first value <= needle in an array<int,int> sorted with uasort\n";
ini_set('memory_limit', '1G');

foreach ($sizes as [$n, $iterations]) {
    bench_binary_search_associative($n, $iterations);
    echo "\n";
}

/*
Results for php 8.2.0-dev debug=0 with opcache enabled=true

Testing repeatedly looking for the key of the first value <= needle in an array<int,int> sorted with uasort
binary_search associative search n= 1048576 iterations=     100 results=[5598280400, 5598280400] binary_search=0.0001 manual search=0.5694 time=0.0002x

binary_search associative search n=    1000 iterations=   80000 results=[4053813000, 4053813000] binary_search=0.0139 manual search=0.4099 time=0.0338x

binary_search associative search n=     100 iterations=  800000 results=[3518360400, 3518360400] binary_search=0.1069 manual search=0.4321 time=0.2474x

binary_search associative search n=       8 iterations= 5000000 results=[ 770797800,  770797800] binary_search=0.4933 manual search=0.3749 time=1.3157x

binary_search associative search n=       4 iterations=10000000 results=[1062882900, 1062882900] binary_search=0.9216 manual search=0.5693 time=1.6188x

binary_search associative search n=       2 iterations=20000000 results=[1004622000, 1004622000] binary_search=1.7038 manual search=0.9195 time=1.8530x

 */
