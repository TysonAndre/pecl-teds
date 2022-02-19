--TEST--
Teds\Collection constructed from Traversable rewind error
--FILE--
<?php

class MyIterator implements Iterator  {
    public function __construct(
        public bool $rewindThrows = false,
        public bool $currentThrows = false,
        public bool $nextThrows = false,
        public bool $keyThrows = false,
        public bool $validThrows = false,
        public bool $destructThrows = false,
        public bool $valid = true,
    ) {
    }

    public function rewind(): void {
        if ($this->rewindThrows) { throw new RuntimeException(__FUNCTION__ . " threw"); }
    }

    public function current(): mixed {
        if ($this->currentThrows) { ;throw new RuntimeException(__FUNCTION__ . " threw"); }
        return new stdClass();
    }

    public function next(): void {
        if ($this->nextThrows) { throw new RuntimeException(__FUNCTION__ . " threw"); }
        if ($this->destructThrows) {
            $this->valid = false;
        }
    }

    public function key(): mixed {
        if ($this->keyThrows) { throw new RuntimeException(__FUNCTION__ . " threw"); }
        return new stdClass();
    }

    public function valid(): bool {
        if ($this->validThrows) { throw new RuntimeException(__FUNCTION__ . " threw"); }
        return $this->valid;
    }

    public function __destruct() {
        if ($this->destructThrows) { throw new RuntimeException(__FUNCTION__ . " threw"); }
    }
}

function test_rewind_throw(string $class_name) {
    echo "Test $class_name\n";
    foreach (['rewindThrows', 'validThrows', 'currentThrows', 'nextThrows', 'destructThrows', 'keyThrows'] as $param) {
        if ($param === 'keyThrows' && !preg_match('/Iterable|Map/i', $class_name)) {
            echo "skip $class_name keyThrows\n";
            continue;
        }
        try {
            $it = new $class_name(new MyIterator(...[$param => true]));
            echo "$class_name: succeeded despite $param: true\n";
            var_dump($it);
        } catch (Exception $e) {
            printf("%s: Caught %s: %s\n", $class_name, $e::class, $e->getMessage());
        }
    }
}
foreach ([
    Teds\Deque::class,
    Teds\ImmutableIterable::class,
    Teds\ImmutableSequence::class,
    Teds\LowMemoryVector::class,
    Teds\MutableIterable::class,
    Teds\StrictHashMap::class,
    Teds\StrictHashSet::class,
    Teds\StrictMaxHeap::class,
    Teds\StrictMinHeap::class,
    Teds\StrictSortedVectorMap::class,
    Teds\StrictSortedVectorSet::class,
    Teds\StrictTreeMap::class,
    Teds\StrictTreeSet::class,
    Teds\Vector::class,
    Teds\CachedIterable::class,
] as $class_name) {
    test_rewind_throw($class_name);
}
?>
--EXPECT--
Test Teds\Deque
Teds\Deque: Caught RuntimeException: rewind threw
Teds\Deque: Caught RuntimeException: valid threw
Teds\Deque: Caught RuntimeException: current threw
Teds\Deque: Caught RuntimeException: next threw
Teds\Deque: Caught RuntimeException: __destruct threw
skip Teds\Deque keyThrows
Test Teds\ImmutableIterable
Teds\ImmutableIterable: Caught RuntimeException: rewind threw
Teds\ImmutableIterable: Caught RuntimeException: valid threw
Teds\ImmutableIterable: Caught RuntimeException: current threw
Teds\ImmutableIterable: Caught RuntimeException: next threw
Teds\ImmutableIterable: Caught RuntimeException: __destruct threw
Teds\ImmutableIterable: Caught RuntimeException: key threw
Test Teds\ImmutableSequence
Teds\ImmutableSequence: Caught RuntimeException: rewind threw
Teds\ImmutableSequence: Caught RuntimeException: valid threw
Teds\ImmutableSequence: Caught RuntimeException: current threw
Teds\ImmutableSequence: Caught RuntimeException: next threw
Teds\ImmutableSequence: Caught RuntimeException: __destruct threw
skip Teds\ImmutableSequence keyThrows
Test Teds\LowMemoryVector
Teds\LowMemoryVector: Caught RuntimeException: rewind threw
Teds\LowMemoryVector: Caught RuntimeException: valid threw
Teds\LowMemoryVector: Caught RuntimeException: current threw
Teds\LowMemoryVector: Caught RuntimeException: next threw
Teds\LowMemoryVector: Caught RuntimeException: __destruct threw
skip Teds\LowMemoryVector keyThrows
Test Teds\MutableIterable
Teds\MutableIterable: Caught RuntimeException: rewind threw
Teds\MutableIterable: Caught RuntimeException: valid threw
Teds\MutableIterable: Caught RuntimeException: current threw
Teds\MutableIterable: Caught RuntimeException: next threw
Teds\MutableIterable: Caught RuntimeException: __destruct threw
Teds\MutableIterable: Caught RuntimeException: key threw
Test Teds\StrictHashMap
Teds\StrictHashMap: Caught RuntimeException: rewind threw
Teds\StrictHashMap: Caught RuntimeException: valid threw
Teds\StrictHashMap: Caught RuntimeException: current threw
Teds\StrictHashMap: Caught RuntimeException: next threw
Teds\StrictHashMap: Caught RuntimeException: __destruct threw
Teds\StrictHashMap: Caught RuntimeException: key threw
Test Teds\StrictHashSet
Teds\StrictHashSet: Caught RuntimeException: rewind threw
Teds\StrictHashSet: Caught RuntimeException: valid threw
Teds\StrictHashSet: Caught RuntimeException: current threw
Teds\StrictHashSet: Caught RuntimeException: next threw
Teds\StrictHashSet: Caught RuntimeException: __destruct threw
skip Teds\StrictHashSet keyThrows
Test Teds\StrictMaxHeap
Teds\StrictMaxHeap: Caught RuntimeException: rewind threw
Teds\StrictMaxHeap: Caught RuntimeException: valid threw
Teds\StrictMaxHeap: Caught RuntimeException: current threw
Teds\StrictMaxHeap: Caught RuntimeException: next threw
Teds\StrictMaxHeap: Caught RuntimeException: __destruct threw
skip Teds\StrictMaxHeap keyThrows
Test Teds\StrictMinHeap
Teds\StrictMinHeap: Caught RuntimeException: rewind threw
Teds\StrictMinHeap: Caught RuntimeException: valid threw
Teds\StrictMinHeap: Caught RuntimeException: current threw
Teds\StrictMinHeap: Caught RuntimeException: next threw
Teds\StrictMinHeap: Caught RuntimeException: __destruct threw
skip Teds\StrictMinHeap keyThrows
Test Teds\StrictSortedVectorMap
Teds\StrictSortedVectorMap: Caught RuntimeException: rewind threw
Teds\StrictSortedVectorMap: Caught RuntimeException: valid threw
Teds\StrictSortedVectorMap: Caught RuntimeException: current threw
Teds\StrictSortedVectorMap: Caught RuntimeException: next threw
Teds\StrictSortedVectorMap: Caught RuntimeException: __destruct threw
Teds\StrictSortedVectorMap: Caught RuntimeException: key threw
Test Teds\StrictSortedVectorSet
Teds\StrictSortedVectorSet: Caught RuntimeException: rewind threw
Teds\StrictSortedVectorSet: Caught RuntimeException: valid threw
Teds\StrictSortedVectorSet: Caught RuntimeException: current threw
Teds\StrictSortedVectorSet: Caught RuntimeException: next threw
Teds\StrictSortedVectorSet: Caught RuntimeException: __destruct threw
skip Teds\StrictSortedVectorSet keyThrows
Test Teds\StrictTreeMap
Teds\StrictTreeMap: Caught RuntimeException: rewind threw
Teds\StrictTreeMap: Caught RuntimeException: valid threw
Teds\StrictTreeMap: Caught RuntimeException: current threw
Teds\StrictTreeMap: Caught RuntimeException: next threw
Teds\StrictTreeMap: Caught RuntimeException: __destruct threw
Teds\StrictTreeMap: Caught RuntimeException: key threw
Test Teds\StrictTreeSet
Teds\StrictTreeSet: Caught RuntimeException: rewind threw
Teds\StrictTreeSet: Caught RuntimeException: valid threw
Teds\StrictTreeSet: Caught RuntimeException: current threw
Teds\StrictTreeSet: Caught RuntimeException: next threw
Teds\StrictTreeSet: Caught RuntimeException: __destruct threw
skip Teds\StrictTreeSet keyThrows
Test Teds\Vector
Teds\Vector: Caught RuntimeException: rewind threw
Teds\Vector: Caught RuntimeException: valid threw
Teds\Vector: Caught RuntimeException: current threw
Teds\Vector: Caught RuntimeException: next threw
Teds\Vector: Caught RuntimeException: __destruct threw
skip Teds\Vector keyThrows
Test Teds\CachedIterable
Teds\CachedIterable: Caught RuntimeException: rewind threw
Teds\CachedIterable: succeeded despite validThrows: true
object(Teds\CachedIterable)#4 (0) {
}
Teds\CachedIterable: Caught RuntimeException: valid threw
Teds\CachedIterable: succeeded despite currentThrows: true
object(Teds\CachedIterable)#5 (0) {
}
Teds\CachedIterable: Caught RuntimeException: current threw
Teds\CachedIterable: succeeded despite nextThrows: true
object(Teds\CachedIterable)#6 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#8 (0) {
    }
    [1]=>
    object(stdClass)#1 (0) {
    }
  }
}
Teds\CachedIterable: Caught RuntimeException: next threw
Teds\CachedIterable: succeeded despite destructThrows: true
object(Teds\CachedIterable)#3 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#9 (0) {
    }
    [1]=>
    object(stdClass)#4 (0) {
    }
  }
}
Teds\CachedIterable: Caught RuntimeException: __destruct threw
Teds\CachedIterable: succeeded despite keyThrows: true
object(Teds\CachedIterable)#7 (0) {
}
Teds\CachedIterable: Caught RuntimeException: key threw