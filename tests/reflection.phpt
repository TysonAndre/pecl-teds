--TEST--
Test reflection information
--FILE--
<?php
$x = new ReflectionExtension('teds');
echo "Classes:\n";
$classes = $x->getClasses();
uksort($classes, 'strcmp');
foreach ($classes as $name => $info) {
    if (PHP_VERSION_ID >= 80100 && $info->isEnum()) {
        continue;
    }
    foreach ([
        ReflectionClass::IS_IMPLICIT_ABSTRACT => 'IS_IMPLICIT_ABSTRACT',
        ReflectionClass::IS_EXPLICIT_ABSTRACT => 'abstract',
        ReflectionClass::IS_FINAL => 'final',
    ] as $modifier => $modifier_name) {
        if ($info->getModifiers() & $modifier) {
            echo "$modifier_name ";
        }
    }
    if ($info->isTrait()) {
        echo "trait ";
    } elseif ($info->isInterface()) {
        echo "interface ";
    } else {
        echo "class ";
    }
    echo $name;
    $parent = $info->getParentClass();
    if ($parent) {
        echo ' extends ' . $parent->name;
    }

    $interfaces = $info->getInterfaceNames();
    usort($interfaces, 'strcmp');
    if ($interfaces) {
        echo ($info->isInterface() ? ' extends ' : ' implements ') . implode(', ', $interfaces);
    }
    $modifiers = [];
    if ($name !== $info->name) {
        echo " (alias of $info->name)";
    }

    echo "\n";
}
echo "\nFunctions:\n";
$functions = $x->getFunctions();
uksort($functions, 'strcmp');
foreach ($functions as $name => $info) {
    echo "$name\n";
}
?>
--EXPECT--
Classes:
final class Teds\BitVector implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
final class Teds\CachedIterable implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Traversable
interface Teds\Collection extends Countable, Traversable
final class Teds\Deque implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
final class Teds\ImmutableIterable implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Traversable
final class Teds\ImmutableSequence implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
final class Teds\ImmutableSortedIntSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
final class Teds\ImmutableSortedStringSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
final class Teds\IntVector implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
final class Teds\LowMemoryVector implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
interface Teds\Map extends ArrayAccess, Countable, Teds\Collection, Traversable
final class Teds\MutableIterable implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Traversable
interface Teds\Sequence extends ArrayAccess, Countable, Teds\Collection, Traversable
interface Teds\Set extends Countable, Teds\Collection, Traversable
final class Teds\SortedIntVectorSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
final class Teds\StrictHashMap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable
final class Teds\StrictHashSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
final class Teds\StrictMaxHeap implements Countable, Iterator, Teds\Collection, Traversable
final class Teds\StrictMinHeap implements Countable, Iterator, Teds\Collection, Traversable
final class Teds\StrictSortedVectorMap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable
final class Teds\StrictSortedVectorSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
final class Teds\StrictTreeMap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable
final class Teds\StrictTreeSet implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable
class Teds\UnsupportedOperationException extends RuntimeException implements Stringable, Throwable
final class Teds\Vector implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable
final class teds\immutablekeyvaluesequence implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Traversable (alias of Teds\ImmutableIterable)
final class teds\keyvaluesequence implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Traversable (alias of Teds\MutableIterable)
final class teds\stablemaxheap implements Countable, Iterator, Teds\Collection, Traversable (alias of Teds\StrictMaxHeap)
final class teds\stableminheap implements Countable, Iterator, Teds\Collection, Traversable (alias of Teds\StrictMinHeap)
final class teds\stablesortedlistmap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable (alias of Teds\StrictSortedVectorMap)
final class teds\stablesortedmap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable (alias of Teds\StrictTreeMap)
final class teds\stablesortedset implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable (alias of Teds\StrictTreeSet)
final class teds\strictmap implements ArrayAccess, Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Map, Traversable (alias of Teds\StrictHashMap)
final class teds\strictset implements Countable, IteratorAggregate, JsonSerializable, Teds\Collection, Teds\Set, Traversable (alias of Teds\StrictHashSet)
interface teds\values extends Countable, Traversable (alias of Teds\Collection)

Functions:
Teds\all
Teds\any
Teds\array_value_first
Teds\array_value_last
Teds\binary_search
Teds\find
Teds\fold
Teds\includes_value
Teds\is_same_array_handle
Teds\none
Teds\stable_compare
Teds\strict_equals
Teds\strict_hash
Teds\unique_values
