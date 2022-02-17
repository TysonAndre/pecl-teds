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
echo "Testing repeated search for a missing signed **byte** value in various collection types. Larger integers will be slower.\n\n";
foreach ([
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

Repeated search LowMemoryVector    n=10000 iterations=10000 0.0010 seconds
Repeated search Teds\StrictTreeSet n=10000 iterations=10000 0.0003 seconds
Repeated search Teds\StrictSet     n=10000 iterations=10000 0.0003 seconds
Repeated search array              n=10000 iterations=10000 0.1620 seconds

Repeated search LowMemoryVector    n=1000 iterations=100000 0.0032 seconds
Repeated search Teds\StrictTreeSet n=1000 iterations=100000 0.0025 seconds
Repeated search Teds\StrictSet     n=1000 iterations=100000 0.0024 seconds
Repeated search array              n=1000 iterations=100000 0.1616 seconds

Repeated search LowMemoryVector    n=100 iterations=1000000 0.0254 seconds
Repeated search Teds\StrictTreeSet n=100 iterations=1000000 0.0255 seconds
Repeated search Teds\StrictSet     n=100 iterations=1000000 0.0244 seconds
Repeated search array              n=100 iterations=1000000 0.1855 seconds

Repeated search LowMemoryVector    n=10 iterations=10000000 0.2545 seconds
Repeated search Teds\StrictTreeSet n=10 iterations=10000000 0.2579 seconds
Repeated search Teds\StrictSet     n=10 iterations=10000000 0.2449 seconds
Repeated search array              n=10 iterations=10000000 0.3609 seconds
 */
