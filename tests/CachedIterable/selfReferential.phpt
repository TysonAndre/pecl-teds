--TEST--
Teds\CachedIterable Self-referential evaluation
--FILE--
<?php
function yield_values() {
    echo "Started yield_values call\n";
    for ($i = 0; $i < 3; $i++) {
        yield "a$i" => "b$i";
        echo "After yielding i=$i\n";
    }
}

$primes = new Teds\CachedIterable((function () use (&$primes) {
    echo "Yielding 2\n";
    yield 2;
    echo "Yielded 2\n";
    for ($i = 3; true; $i += 2) {
        foreach ($primes as $p) {
            if ($p * $p > $i) {
                echo "Generator found prime $i\n";
                yield $i;
                break;
            }
            if ($i % $p === 0) {
                continue 2;
            }
        }
    }
})());
printf("The 2nd prime is %d\n", $primes->valueAt(1));
printf("The 11th prime is %d\n", $primes->valueAt(10));
printf("The 3rd prime is %d\n", $primes->valueAt(2));
printf("The 1st prime is %d\n", $primes->valueAt(0));
--EXPECT--
Yielding 2
Yielded 2
Generator found prime 3
The 2nd prime is 3
Generator found prime 5
Generator found prime 7
Generator found prime 11
Generator found prime 13
Generator found prime 17
Generator found prime 19
Generator found prime 23
Generator found prime 29
Generator found prime 31
The 11th prime is 31
The 3rd prime is 5
The 1st prime is 2