--TEST--
Teds\Collection constructed from Traversable
--FILE--
<?php

function yields_values() {
    for ($i = 0; $i < 8; $i++) {
        yield "r$i" => "s$i";
    }
    $o = (object)['key' => 'value'];
    yield $o => $o;
    yield $o => $o;
    yield 0 => 1;
    yield 0 => 2;
    echo "Done evaluating the generator\n";
}

function test_traversable(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(yields_values());
    foreach ($it as $key => $value) {
        printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
    }
    echo "Rewind and iterate again starting from r0\n";
    foreach ($it as $key => $value) {
        printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
        echo "End early\n";
        break;
    }
    unset($it);

    $emptyIt = new $class_name(new ArrayObject());
    foreach ($emptyIt as $key => $value) {
        echo "Unreachable\n";
    }
    foreach ($emptyIt as $key => $value) {
        echo "Unreachable\n";
    }
    echo "\n";
}
foreach ([
    Teds\Deque::class,
    Teds\CachedIterable::class,
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
] as $class_name) {
    test_traversable($class_name);
}
?>
--EXPECT--
Test Teds\Deque
Done evaluating the generator
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: (object) array(
   'key' => 'value',
)
Key: 9
Value: (object) array(
   'key' => 'value',
)
Key: 10
Value: 1
Key: 11
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 's0'
End early

Test Teds\CachedIterable
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 1
Key: 0
Value: 2
Done evaluating the generator
Rewind and iterate again starting from r0
Key: 'r0'
Value: 's0'
End early

Test Teds\ImmutableIterable
Done evaluating the generator
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 1
Key: 0
Value: 2
Rewind and iterate again starting from r0
Key: 'r0'
Value: 's0'
End early

Test Teds\ImmutableSequence
Done evaluating the generator
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: (object) array(
   'key' => 'value',
)
Key: 9
Value: (object) array(
   'key' => 'value',
)
Key: 10
Value: 1
Key: 11
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 's0'
End early

Test Teds\LowMemoryVector
Done evaluating the generator
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: (object) array(
   'key' => 'value',
)
Key: 9
Value: (object) array(
   'key' => 'value',
)
Key: 10
Value: 1
Key: 11
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 's0'
End early

Test Teds\MutableIterable
Done evaluating the generator
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 1
Key: 0
Value: 2
Rewind and iterate again starting from r0
Key: 'r0'
Value: 's0'
End early

Test Teds\StrictHashMap
Done evaluating the generator
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 0
Value: 2
Rewind and iterate again starting from r0
Key: 'r0'
Value: 's0'
End early

Test Teds\StrictHashSet
Done evaluating the generator
Key: 's0'
Value: 's0'
Key: 's1'
Value: 's1'
Key: 's2'
Value: 's2'
Key: 's3'
Value: 's3'
Key: 's4'
Value: 's4'
Key: 's5'
Value: 's5'
Key: 's6'
Value: 's6'
Key: 's7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 1
Value: 1
Key: 2
Value: 2
Rewind and iterate again starting from r0
Key: 's0'
Value: 's0'
End early

Test Teds\StrictMaxHeap
Done evaluating the generator
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: 's7'
Value: 's7'
Key: 's6'
Value: 's6'
Key: 's5'
Value: 's5'
Key: 's4'
Value: 's4'
Key: 's3'
Value: 's3'
Key: 's2'
Value: 's2'
Key: 's1'
Value: 's1'
Key: 's0'
Value: 's0'
Key: 2
Value: 2
Key: 1
Value: 1
Rewind and iterate again starting from r0

Test Teds\StrictMinHeap
Done evaluating the generator
Key: 1
Value: 1
Key: 2
Value: 2
Key: 's0'
Value: 's0'
Key: 's1'
Value: 's1'
Key: 's2'
Value: 's2'
Key: 's3'
Value: 's3'
Key: 's4'
Value: 's4'
Key: 's5'
Value: 's5'
Key: 's6'
Value: 's6'
Key: 's7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0

Test Teds\StrictSortedVectorMap
Done evaluating the generator
Key: 0
Value: 2
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0
Key: 0
Value: 2
End early

Test Teds\StrictSortedVectorSet
Done evaluating the generator
Key: 1
Value: 1
Key: 2
Value: 2
Key: 's0'
Value: 's0'
Key: 's1'
Value: 's1'
Key: 's2'
Value: 's2'
Key: 's3'
Value: 's3'
Key: 's4'
Value: 's4'
Key: 's5'
Value: 's5'
Key: 's6'
Value: 's6'
Key: 's7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0
Key: 1
Value: 1
End early

Test Teds\StrictTreeMap
Done evaluating the generator
Key: 0
Value: 2
Key: 'r0'
Value: 's0'
Key: 'r1'
Value: 's1'
Key: 'r2'
Value: 's2'
Key: 'r3'
Value: 's3'
Key: 'r4'
Value: 's4'
Key: 'r5'
Value: 's5'
Key: 'r6'
Value: 's6'
Key: 'r7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0
Key: 0
Value: 2
End early

Test Teds\StrictTreeSet
Done evaluating the generator
Key: 1
Value: 1
Key: 2
Value: 2
Key: 's0'
Value: 's0'
Key: 's1'
Value: 's1'
Key: 's2'
Value: 's2'
Key: 's3'
Value: 's3'
Key: 's4'
Value: 's4'
Key: 's5'
Value: 's5'
Key: 's6'
Value: 's6'
Key: 's7'
Value: 's7'
Key: (object) array(
   'key' => 'value',
)
Value: (object) array(
   'key' => 'value',
)
Rewind and iterate again starting from r0
Key: 1
Value: 1
End early

Test Teds\Vector
Done evaluating the generator
Key: 0
Value: 's0'
Key: 1
Value: 's1'
Key: 2
Value: 's2'
Key: 3
Value: 's3'
Key: 4
Value: 's4'
Key: 5
Value: 's5'
Key: 6
Value: 's6'
Key: 7
Value: 's7'
Key: 8
Value: (object) array(
   'key' => 'value',
)
Key: 9
Value: (object) array(
   'key' => 'value',
)
Key: 10
Value: 1
Key: 11
Value: 2
Rewind and iterate again starting from r0
Key: 0
Value: 's0'
End early