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
        $sequence = new $class_name([true, false, true, true]);
    }
    $i = 0;
    foreach ($sequence as $offset => $value) {
        printf("offset=%s value=%s\n", $offset, json_encode($value));
        if ($i === 0 || $i === 2) {
            $sequence->offsetUnset(0);
        }
        if ($i === 3) {
            $sequence->insert($offset + 1, $class_name !== Teds\BitVector::class ? 5000 : false);
        }
        $i++;
    }
}
?>
--EXPECT--
Test Teds\Deque
offset=0 value=1000
offset=0 value=2000
offset=1 value=3000
offset=1 value=4000
offset=2 value=5000
Test Teds\Vector
offset=0 value=1000
offset=0 value=2000
offset=1 value=3000
offset=1 value=4000
offset=2 value=5000
Test Teds\LowMemoryVector
offset=0 value=1000
offset=0 value=2000
offset=1 value=3000
offset=1 value=4000
offset=2 value=5000
Test Teds\IntVector
offset=0 value=1000
offset=0 value=2000
offset=1 value=3000
offset=1 value=4000
offset=2 value=5000
Test Teds\BitVector
offset=0 value=true
offset=0 value=false
offset=1 value=true
offset=1 value=true
offset=2 value=false