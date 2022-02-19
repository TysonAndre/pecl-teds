--TEST--
Teds\Collection constructed from Traversable throwing for key
--FILE--
<?php

class HasDestructor {
    public function __construct(public string $value) {}
    public function __destruct() {
        echo "in " . __METHOD__ . " $this->value\n";
    }
}

function yields_and_throws() {
    yield 123 => new HasDestructor('in value1');
    yield new HasDestructor('in key') => 123;
    yield 123 => new HasDestructor('in value2');
    echo "Overridden in maps\n";
    yield 'first' => 'second';

    throw new RuntimeException('test');
}
function test_collection_exceptionhandler(string $class) {
    try {
        $it = new $class(yields_and_throws());
    } catch (Throwable $e) {
        printf("Caught %s: %s\n", $e::class, $e->getMessage());
    }
    gc_collect_cycles();
    echo "Done\n";
}
foreach ([
    Teds\BitSet::class,
    Teds\Deque::class,
    Teds\CachedIterable::class,  // does not throw because it does not evaluate unless requested.
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
    Teds\IntVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\StrictMinHeap::class,
    Teds\StrictMaxHeap::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
] as $class) {
    echo "Test $class\n";
    test_collection_exceptionhandler($class);
    echo "\n";
}
?>
--EXPECT--
Test Teds\BitSet
Caught TypeError: Illegal Teds\BitSet value type HasDestructor
Done
in HasDestructor::__destruct in value1

Test Teds\Deque
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\CachedIterable
Done
in HasDestructor::__destruct in value1

Test Teds\ImmutableIterable
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in key
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\ImmutableSequence
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\LowMemoryVector
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\IntVector
Caught TypeError: Illegal Teds\IntVector value type HasDestructor
Done
in HasDestructor::__destruct in value1

Test Teds\MutableIterable
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in key
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\StrictHashMap
in HasDestructor::__destruct in value1
Overridden in maps
in HasDestructor::__destruct in value2
in HasDestructor::__destruct in key
Caught RuntimeException: test
Done

Test Teds\StrictHashSet
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\StrictMinHeap
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\StrictMaxHeap
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value2
in HasDestructor::__destruct in value1
Caught RuntimeException: test
Done

Test Teds\StrictSortedVectorMap
in HasDestructor::__destruct in value1
Overridden in maps
in HasDestructor::__destruct in value2
in HasDestructor::__destruct in key
Caught RuntimeException: test
Done

Test Teds\StrictTreeMap
in HasDestructor::__destruct in value1
Overridden in maps
in HasDestructor::__destruct in value2
in HasDestructor::__destruct in key
Caught RuntimeException: test
Done

Test Teds\StrictTreeSet
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done

Test Teds\Vector
in HasDestructor::__destruct in key
Overridden in maps
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
Caught RuntimeException: test
Done