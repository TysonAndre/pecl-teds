--TEST--
Teds\Sequence insert()/offsetUnset with iterators
--FILE--
<?php
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
    Teds\IntVector::class,
    Teds\BitVector::class,
] as $class_name) {
    echo "Test $class_name\n";
    if ($class_name !== Teds\BitVector::class) {
        $sequence = new $class_name([1000, 2000, 3000, 4000]);
    } else {
        $sequence = new $class_name([true, false, true, false]);
    }
    $it1 = $sequence->getIterator();
    $it1->next();
    $it2 = $sequence->getIterator();
    $it2->next();
    $it2->next();
    printf("it1 key=%s value=%s\n", $it1->key(), $it1->current());
    printf("it2 key=%s value=%s\n", $it2->key(), $it2->current());
    $sequence->offsetUnset(0);
    printf("after offsetUnset 0 it1 key=%s value=%s\n", $it1->key(), $it1->current());
    printf("after offsetUnset 0 it2 key=%s value=%s\n", $it2->key(), $it2->current());
    $sequence->offsetUnset(2);
    printf("after offsetUnset 2 it1 key=%s value=%s\n", $it1->key(), $it1->current());
    try {
        printf("after offsetUnset 2 it2 key=%s value=%s\n", $it2->key(), $it2->current());
    } catch (OutOfBoundsException $e) {
        printf("Caught %s\n", $e->getMessage());
    }
    $sequence->offsetUnset(0);
    try {
        printf("after offsetUnset 0 it1 key=%s value=%s\n", $it1->key(), $it1->current());
    } catch (OutOfBoundsException $e) {
        printf("Caught %s\n", $e->getMessage());
    }
    printf("after offsetUnset 0 it2 key=%s value=%s\n", $it2->key(), $it2->current());
    echo json_encode($sequence), "\n";
}
?>
--EXPECT--
Test Teds\Deque
it1 key=1 value=2000
it2 key=2 value=3000
after offsetUnset 0 it1 key=0 value=2000
after offsetUnset 0 it2 key=1 value=3000
after offsetUnset 2 it1 key=0 value=2000
after offsetUnset 2 it2 key=1 value=3000
Caught Index out of range
after offsetUnset 0 it2 key=0 value=3000
[3000]
Test Teds\Vector
it1 key=1 value=2000
it2 key=2 value=3000
after offsetUnset 0 it1 key=0 value=2000
after offsetUnset 0 it2 key=1 value=3000
after offsetUnset 2 it1 key=0 value=2000
after offsetUnset 2 it2 key=1 value=3000
Caught Index out of range
after offsetUnset 0 it2 key=0 value=3000
[3000]
Test Teds\LowMemoryVector
it1 key=1 value=2000
it2 key=2 value=3000
after offsetUnset 0 it1 key=0 value=2000
after offsetUnset 0 it2 key=1 value=3000
after offsetUnset 2 it1 key=0 value=2000
after offsetUnset 2 it2 key=1 value=3000
Caught Index out of range
after offsetUnset 0 it2 key=0 value=3000
[3000]
Test Teds\IntVector
it1 key=1 value=2000
it2 key=2 value=3000
after offsetUnset 0 it1 key=0 value=2000
after offsetUnset 0 it2 key=1 value=3000
after offsetUnset 2 it1 key=0 value=2000
after offsetUnset 2 it2 key=1 value=3000
Caught Index out of range
after offsetUnset 0 it2 key=0 value=3000
[3000]
Test Teds\BitVector
it1 key=1 value=
it2 key=2 value=1
after offsetUnset 0 it1 key=0 value=
after offsetUnset 0 it2 key=1 value=1
after offsetUnset 2 it1 key=0 value=
after offsetUnset 2 it2 key=1 value=1
Caught Index out of range
after offsetUnset 0 it2 key=0 value=1
[true]