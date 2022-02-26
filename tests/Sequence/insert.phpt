--TEST--
Teds\Sequence insert()
--FILE--
<?php
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
    Teds\IntVector::class,
    Teds\ImmutableSequence::class,
] as $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name();
    try {
        $it->insert(PHP_INT_MIN, 123);
    } catch (Exception $e) {
        printf("Insert invalid caught %s: %s\n", $e::class, $e->getMessage());
    }

    try {
        $it->insert(0, 101);
        $it->insert(0, 100);
        $it->insert(2, 104);
        $it->insert(2, 102, 103);
        $it->insert(2);
    } catch (Exception $e) {
        printf("Insert in range threw %s: %s\n", $e::class, $e->getMessage());
    }
    try {
        $it->insert(1, new stdClass());
        $it->insert(1, new ArrayObject());
    } catch (Throwable $e) {
        printf("Insert object threw %s: %s\n", $e::class, $e->getMessage());
    }
    printf("New size=%d\n", count($it));
    echo json_encode($it), "\n";
}
?>
--EXPECT--
Test Teds\Deque
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
Test Teds\Vector
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
Test Teds\LowMemoryVector
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
Test Teds\IntVector
Insert invalid caught OutOfBoundsException: Index out of range
Insert object threw TypeError: Illegal Teds\IntVector value type stdClass
New size=5
[100,101,102,103,104]
Test Teds\ImmutableSequence
Insert invalid caught Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
Insert in range threw Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
Insert object threw Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
New size=0
[]