<?php
function show_memory_usage(string $class, array $values, string $description) {
    gc_collect_cycles();
    $startMemory = memory_get_usage();
    $v = new $class($values);
    $endMemory = memory_get_usage();
    printf("memory=%5d bytes for %40s(%s): %s\n", $endMemory - $startMemory, "new $class", count($values) <= 10 ? json_encode($values) : '[...]', $description);
}
show_memory_usage(Teds\IntVector::class, [1], 'single-element Teds\IntVector (1-byte int)');
show_memory_usage(Teds\SortedIntVectorSet::class, [1], 'single-element Teds\SortedIntVectorSet (1-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, [1], 'single-element Teds\LowMemoryVector (1-byte int)');
show_memory_usage(Teds\Vector::class, [1], 'single-element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, [1], 'single-element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, [1], 'single-element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, [1], 'single-element Teds\MutableIterable');
show_memory_usage(Teds\StrictMinHeap::class, [1], 'single-element Teds\StrictMinHeap');
show_memory_usage(Teds\StrictHashSet::class, [1], 'single-element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, [1], 'single-element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, [1], 'single-element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictTreeMap::class, [1], 'single-element Teds\ImmutableIterable');
echo "\n";
$values = range(0, 7);
show_memory_usage(Teds\IntVector::class, $values, '8 element Teds\IntVector (1-byte int)');
show_memory_usage(Teds\SortedIntVectorSet::class, $values, '8 element Teds\SortedIntVectorSet (1-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, $values, '8 element Teds\LowMemoryVector (1-byte int)');
show_memory_usage(Teds\Vector::class, $values, '8 element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, $values, '8 element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, $values, '8 element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, $values, '8 element Teds\MutableIterable');
show_memory_usage(Teds\StrictMinHeap::class, $values, '8 element Teds\StrictMinHeap');
show_memory_usage(Teds\StrictHashSet::class, $values, '8 element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, $values, '8 element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, $values, '8 element Teds\StrictHashMap');
show_memory_usage(Teds\StrictTreeMap::class, $values, '8 element Teds\StrictTreeMap');

echo "\n";
$values = range(0, 1023);
show_memory_usage(Teds\IntVector::class, $values, '1024 element Teds\IntVector (2-byte int)');
show_memory_usage(Teds\SortedIntVectorSet::class, $values, '1024 element Teds\SortedIntVectorSet (2-byte int)');
show_memory_usage(Teds\LowMemoryVector::class, $values, '1024 element Teds\LowMemoryVector (2-byte int)');
show_memory_usage(Teds\Vector::class, $values, '1024 element Teds\Vector');
show_memory_usage(Teds\ImmutableSequence::class, $values, '1024 element Teds\ImmutableSequence');
show_memory_usage(Teds\Deque::class, $values, '1024 element Teds\Deque');
show_memory_usage(Teds\MutableIterable::class, $values, '1024 element Teds\MutableIterable');
show_memory_usage(Teds\StrictMinHeap::class, $values, '1024 element Teds\StrictMinHeap');
show_memory_usage(Teds\StrictHashSet::class, $values, '1024 element Teds\StrictHashSet');
show_memory_usage(Teds\ImmutableIterable::class, $values, '1024 element Teds\ImmutableIterable');
show_memory_usage(Teds\StrictHashMap::class, $values, '1024 element Teds\StrictHashMap');
show_memory_usage(Teds\StrictTreeMap::class, $values, '1024 element Teds\StrictTreeMap');
/*
memory=   88 bytes for                       new Teds\IntVector([1]): single-element Teds\IntVector (1-byte int)
memory=   88 bytes for              new Teds\SortedIntVectorSet([1]): single-element Teds\SortedIntVectorSet (1-byte int)
memory=   88 bytes for                 new Teds\LowMemoryVector([1]): single-element Teds\LowMemoryVector (1-byte int)
memory=   96 bytes for                          new Teds\Vector([1]): single-element Teds\Vector
memory=   72 bytes for               new Teds\ImmutableSequence([1]): single-element Teds\ImmutableSequence
memory=  144 bytes for                           new Teds\Deque([1]): single-element Teds\Deque
memory=   96 bytes for                 new Teds\MutableIterable([1]): single-element Teds\MutableIterable
memory=   96 bytes for                   new Teds\StrictMinHeap([1]): single-element Teds\StrictMinHeap
memory=  336 bytes for                   new Teds\StrictHashSet([1]): single-element Teds\StrictHashSet
memory=   88 bytes for               new Teds\ImmutableIterable([1]): single-element Teds\ImmutableIterable
memory=  400 bytes for                   new Teds\StrictHashMap([1]): single-element Teds\ImmutableIterable
memory=  120 bytes for                   new Teds\StrictTreeMap([1]): single-element Teds\ImmutableIterable

memory=   88 bytes for                       new Teds\IntVector([0,1,2,3,4,5,6,7]): 8 element Teds\IntVector (1-byte int)
memory=   88 bytes for              new Teds\SortedIntVectorSet([0,1,2,3,4,5,6,7]): 8 element Teds\SortedIntVectorSet (1-byte int)
memory=   88 bytes for                 new Teds\LowMemoryVector([0,1,2,3,4,5,6,7]): 8 element Teds\LowMemoryVector (1-byte int)
memory=  208 bytes for                          new Teds\Vector([0,1,2,3,4,5,6,7]): 8 element Teds\Vector
memory=  184 bytes for               new Teds\ImmutableSequence([0,1,2,3,4,5,6,7]): 8 element Teds\ImmutableSequence
memory=  208 bytes for                           new Teds\Deque([0,1,2,3,4,5,6,7]): 8 element Teds\Deque
memory=  320 bytes for                 new Teds\MutableIterable([0,1,2,3,4,5,6,7]): 8 element Teds\MutableIterable
memory=  208 bytes for                   new Teds\StrictMinHeap([0,1,2,3,4,5,6,7]): 8 element Teds\StrictMinHeap
memory=  336 bytes for                   new Teds\StrictHashSet([0,1,2,3,4,5,6,7]): 8 element Teds\StrictHashSet
memory=  312 bytes for               new Teds\ImmutableIterable([0,1,2,3,4,5,6,7]): 8 element Teds\ImmutableIterable
memory=  400 bytes for                   new Teds\StrictHashMap([0,1,2,3,4,5,6,7]): 8 element Teds\StrictHashMap
memory=  512 bytes for                   new Teds\StrictTreeMap([0,1,2,3,4,5,6,7]): 8 element Teds\StrictTreeMap

memory= 2128 bytes for                       new Teds\IntVector([...]): 1024 element Teds\IntVector (2-byte int)
memory= 2128 bytes for              new Teds\SortedIntVectorSet([...]): 1024 element Teds\SortedIntVectorSet (2-byte int)
memory= 2128 bytes for                 new Teds\LowMemoryVector([...]): 1024 element Teds\LowMemoryVector (2-byte int)
memory=16464 bytes for                          new Teds\Vector([...]): 1024 element Teds\Vector
memory=16440 bytes for               new Teds\ImmutableSequence([...]): 1024 element Teds\ImmutableSequence
memory=16464 bytes for                           new Teds\Deque([...]): 1024 element Teds\Deque
memory=32832 bytes for                 new Teds\MutableIterable([...]): 1024 element Teds\MutableIterable
memory=16464 bytes for                   new Teds\StrictMinHeap([...]): 1024 element Teds\StrictMinHeap
memory=32848 bytes for                   new Teds\StrictHashSet([...]): 1024 element Teds\StrictHashSet
memory=32824 bytes for               new Teds\ImmutableIterable([...]): 1024 element Teds\ImmutableIterable
memory=41040 bytes for                   new Teds\StrictHashMap([...]): 1024 element Teds\StrictHashMap
memory=57408 bytes for                   new Teds\StrictTreeMap([...]): 1024 element Teds\StrictTreeMap
 */
