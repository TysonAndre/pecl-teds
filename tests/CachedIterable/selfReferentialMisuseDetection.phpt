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

call_user_func(function () {
    $primes = new Teds\CachedIterable((function () use (&$primes) {
        echo "Yielding 2\n";
        yield 2;
        echo "Yielded 2\n";
        for ($i = 3; true; $i += 2) {
            $it = $primes->getIterator();
            foreach ($primes as $p) {
                // Buggy example: Failing to yield anything
                echo "Check $i % $p === 0\n";
                if ($i % $p === 0) {
                    continue 2;
                }
            }
        }
    })());
    var_dump($primes->isEmpty());  // This yielded at least one element before throwing. It isn't empty.
    try {
        printf("The 2nd prime is %d\n", $primes->valueAt(1));
    } catch (RuntimeException $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    try {
        printf("The 11th prime is %d\n", $primes->valueAt(12));
    } catch (RuntimeException $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    printf("The 1st prime is %d\n", $primes->valueAt(0));
    foreach (['count', 'toArray', 'toPairs', 'values', 'keys'] as $method) {
        try {
            var_dump($primes->$method());
            echo "$method unexpectedly succeeded\n";
        } catch (Exception $e) {
            printf("%s: caught %s: %s\n", $method, $e::class, $e->getMessage());
        }
    }
});

--EXPECT--
Yielding 2
bool(false)
Yielded 2
Check 3 % 2 === 0
Caught RuntimeException: Teds\CachedIterable reentry detected while already fetching next element
Caught OutOfBoundsException: Offset out of range
The 1st prime is 2
count: caught RuntimeException: Teds\CachedIterable failed due to exception from underlying Traversable
toArray: caught RuntimeException: Teds\CachedIterable failed due to exception from underlying Traversable
toPairs: caught RuntimeException: Teds\CachedIterable failed due to exception from underlying Traversable
values: caught RuntimeException: Teds\CachedIterable failed due to exception from underlying Traversable
keys: caught RuntimeException: Teds\CachedIterable failed due to exception from underlying Traversable