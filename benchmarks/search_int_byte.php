<?php
function bench_lmv(int $n, int $iterations) {
    $t1=microtime(true);
    $x = new Teds\LowMemoryVector(array_fill(0, $n, 0));
    for ($i = 0; $i < $iterations; $i++) {
        $x->contains($i % 128);
    }
    $t2 = microtime(true);
    unset($x);
    $t3 = microtime(true);
    printf("Repeated search LowMemoryVector    n=%d iterations=%d %.4f seconds\n", $n, $iterations, $t2 - $t1, $t3-$t2);
}
function bench_array(int $n, int $iterations) {
    $t1=microtime(true);
    $x = array_fill(0, $n, 0);
    for ($i = 0; $i < $iterations; $i++) {
        in_array($i % 128, $x);
    }
    $t2 = microtime(true);
    printf("Repeated search array              n=%d iterations=%d %.4f seconds\n", $n, $iterations, $t2 - $t1);
}
function bench_set(int $n, int $iterations) {
    $t1=microtime(true);
    $x = new Teds\StrictSet(array_fill(0, $n, 0));
    for ($i = 0; $i < $iterations; $i++) {
        $x->contains($i % 128);
    }
    $t2 = microtime(true);
    printf("Repeated search Teds\StrictSet     n=%d iterations=%d %.4f seconds\n", $n, $iterations, $t2 - $t1);
}
function bench_sorted_set(int $n, int $iterations) {
    $t1=microtime(true);
    $x = new Teds\StrictTreeSet(array_fill(0, $n, 0));
    for ($i = 0; $i < $iterations; $i++) {
        $x->contains($i % 128);
    }
    $t2 = microtime(true);
    printf("Repeated search Teds\StrictTreeSet n=%d iterations=%d %.4f seconds\n", $n, $iterations, $t2 - $t1);
}
echo "Testing repeated search for a missing signed **byte** value in various collection types. Larger integers will be slower.\n";
echo "NOTE: The Set types will remove duplicate values\n\n";
foreach ([
    [100000, 1000],
    [10000, 10000],
    [1000, 100000],
    [100, 1000000],
    [10, 10000000],
] as [$n, $iterations]) {
    bench_lmv($n, $iterations);
    bench_sorted_set($n, $iterations);
    bench_set($n, $iterations);
    bench_array($n, $iterations);
    echo "\n";
}

/*
Testing repeated search for a missing signed **byte** value in various collection types. Larger integers will be slower.

Repeated search LowMemoryVector    n=100000 iterations=1000 0.0022 seconds
Repeated search Teds\StrictTreeSet n=100000 iterations=1000 0.0011 seconds
Repeated search Teds\StrictSet     n=100000 iterations=1000 0.0011 seconds
Repeated search array              n=100000 iterations=1000 0.1670 seconds

Repeated search LowMemoryVector    n=10000 iterations=10000 0.0010 seconds
Repeated search Teds\StrictTreeSet n=10000 iterations=10000 0.0003 seconds
Repeated search Teds\StrictSet     n=10000 iterations=10000 0.0003 seconds
Repeated search array              n=10000 iterations=10000 0.1587 seconds

Repeated search LowMemoryVector    n=1000 iterations=100000 0.0034 seconds
Repeated search Teds\StrictTreeSet n=1000 iterations=100000 0.0026 seconds
Repeated search Teds\StrictSet     n=1000 iterations=100000 0.0025 seconds
Repeated search array              n=1000 iterations=100000 0.1609 seconds

Repeated search LowMemoryVector    n=100 iterations=1000000 0.0271 seconds
Repeated search Teds\StrictTreeSet n=100 iterations=1000000 0.0259 seconds
Repeated search Teds\StrictSet     n=100 iterations=1000000 0.0249 seconds
Repeated search array              n=100 iterations=1000000 0.1907 seconds

Repeated search LowMemoryVector    n=10 iterations=10000000 0.2679 seconds
Repeated search Teds\StrictTreeSet n=10 iterations=10000000 0.2592 seconds
Repeated search Teds\StrictSet     n=10 iterations=10000000 0.2507 seconds
Repeated search array              n=10 iterations=10000000 0.3715 seconds
 */
