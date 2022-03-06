<?php

use function Teds\binary_search;

function create_key(int $i): string {
    return sprintf("k%12d", $i);
}

function bench_binary_search_packed(int $n, int $iterations) {
    gc_collect_cycles();
    $haystack = [];
    for ($i = 0; $i < $n; $i++) {
        $haystack[] = create_key($i);
    }
    $result1 = 0;
    $result2 = 0;

    $t1 = hrtime(true);
    srand(1234);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = create_key(rand(0, $n - 1));
        $result1 += binary_search($haystack, $needle)['key'];
    }
    $t2 = hrtime(true);
    srand(1234);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = create_key(rand(0, $n - 1));
        $result2 += array_search($needle, $haystack);
    }
    $t3 = hrtime(true);
    $duration1 = ($t2 - $t1)/1e9;
    $duration2 = ($t3 - $t2)/1e9;
    printf("binary_search(packed array) n=%8d iterations=%8d results=[%10d, %10d] binary_search=%.4f array_search=%.4f time=%.4fx\n", $n, $iterations, $result1, $result2, $duration1, $duration2, $duration1/$duration2);
}
function bench_binary_search_gaps(int $n, int $iterations) {
    gc_collect_cycles();
    $haystack = [];
    for ($i = 0; $i < $n; $i++) {
        $haystack[] = create_key($i);
    }
    for ($i = 1; $i < $n; $i += 2) {
        unset($haystack[$i]);
    }
    $result1 = 0;
    $result2 = 0;

    $t1 = hrtime(true);
    srand(1234);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = create_key(rand(0, ($n >> 1) - 1) * 2);
        $result1 += binary_search($haystack, $needle)['key'];
    }
    $t2 = hrtime(true);
    srand(1234);
    for ($i = 0; $i < $iterations; $i++) {
        $needle = create_key(rand(0, ($n >> 1) - 1) * 2);
        $result2 += array_search($needle, $haystack);
    }
    $t3 = hrtime(true);
    $duration1 = ($t2 - $t1)/1e9;
    $duration2 = ($t3 - $t2)/1e9;
    printf("binary_search(has gaps    ) n=%8d iterations=%8d results=[%10d, %10d] binary_search=%.4f array_search=%.4f time=%.4fx\n", $n, $iterations, $result1, $result2, $duration1, $duration2, $duration1/$duration2);
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
echo "Testing repeated lookup of index on a sorted list of strings.\n";
ini_set('memory_limit', '1G');

foreach ($sizes as [$n, $iterations]) {
    bench_binary_search_packed($n, $iterations);
    bench_binary_search_gaps($n, $iterations);
    echo "\n";
}

/*
Results for php 8.2.0-dev debug=0 with opcache enabled=true

Testing repeated lookup of index on a sorted list of strings.
binary_search(packed array) n= 1048576 iterations=     100 results=[  50119711,   50119711] binary_search=0.0003 array_search=0.7623 time=0.0003x
binary_search(has gaps    ) n= 1048576 iterations=     100 results=[  50956350,   50956350] binary_search=0.0002 array_search=0.4814 time=0.0005x

binary_search(packed array) n=    1000 iterations=   80000 results=[  39999666,   39999666] binary_search=0.0339 array_search=0.2793 time=0.1214x
binary_search(has gaps    ) n=    1000 iterations=   80000 results=[  39952332,   39952332] binary_search=0.0332 array_search=0.1721 time=0.1931x

binary_search(packed array) n=     100 iterations=  800000 results=[  39634882,   39634882] binary_search=0.2738 array_search=0.3858 time=0.7097x
binary_search(has gaps    ) n=     100 iterations=  800000 results=[  39179664,   39179664] binary_search=0.2608 array_search=0.2875 time=0.9073x

binary_search(packed array) n=       8 iterations= 5000000 results=[  17493655,   17493655] binary_search=1.3227 array_search=1.0059 time=1.3150x
binary_search(has gaps    ) n=       8 iterations= 5000000 results=[  14995758,   14995758] binary_search=1.2898 array_search=0.9596 time=1.3440x

binary_search(packed array) n=       4 iterations=10000000 results=[  14999145,   14999145] binary_search=2.4758 array_search=1.6601 time=1.4914x
binary_search(has gaps    ) n=       4 iterations=10000000 results=[  10001454,   10001454] binary_search=2.4051 array_search=1.6115 time=1.4924x

binary_search(packed array) n=       2 iterations=20000000 results=[  10004089,   10004089] binary_search=4.7000 array_search=3.1302 time=1.5015x
binary_search(has gaps    ) n=       2 iterations=20000000 results=[         0,          0] binary_search=4.4987 array_search=3.0574 time=1.4714x

 */
