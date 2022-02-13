<?php
function show_memory_usage(string $class, array $values, string $description) {
    $startMemory = memory_get_usage();
    $v = new $class($values);
    $endMemory = memory_get_usage();
    printf("memory=%5d bytes for %40s(%s): %s\n", $endMemory - $startMemory, "new $class", count($values) <= 10 ? json_encode($values) : '[...]', $description);
}
show_memory_usage(Teds\IntVector::class, [1], 'single-element Teds\IntVector (1-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, [1], 'single-element Teds\LowMemoryVector (1-byte int)');
show_memory_usage(Teds\Vector::class, [1], 'single-element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, [1], 'single-element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, [1], 'single-element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, [1], 'single-element Teds\MutableIterable');
show_memory_usage(Teds\StableMinHeap::class, [1], 'single-element Teds\StableMinHeap');
show_memory_usage(Teds\StrictHashSet::class, [1], 'single-element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, [1], 'single-element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, [1], 'single-element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictTreeMap::class, [1], 'single-element Teds\ImmutableIterable');
echo "\n";
$values = range(0, 7);
show_memory_usage(Teds\IntVector::class, $values, '8 element Teds\IntVector (1-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, $values, '8 element Teds\LowMemoryVector (1-byte int)');
show_memory_usage(Teds\Vector::class, $values, '8 element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, $values, '8 element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, $values, '8 element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, $values, '8 element Teds\MutableIterable');
show_memory_usage(Teds\StableMinHeap::class, $values, '8 element Teds\StableMinHeap');
show_memory_usage(Teds\StrictHashSet::class, $values, '8 element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, $values, '8 element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, $values, '8 element Teds\StrictHashMap');
show_memory_usage(Teds\StrictTreeMap::class, $values, '8 element Teds\StrictTreeMap');

echo "\n";
$values = range(0, 1023);
show_memory_usage(Teds\IntVector::class, $values, '1024 element Teds\IntVector (2-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, $values, '1024 element Teds\LowMemoryVector (2-byte int)');
show_memory_usage(Teds\Vector::class, $values, '1024 element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, $values, '1024 element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, $values, '1024 element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, $values, '1024 element Teds\MutableIterable');
show_memory_usage(Teds\StableMinHeap::class, $values, '1024 Teds\StableMinHeap');
show_memory_usage(Teds\StrictHashSet::class, $values, '1024 element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, $values, '1024 element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, $values, '1024 element Teds\StrictHashMap');
show_memory_usage(Teds\StrictTreeMap::class, $values, '1024 element Teds\StrictTreeMap');
