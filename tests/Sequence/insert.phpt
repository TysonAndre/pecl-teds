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
    try {
        $it->offsetUnset(1); // Remove the second element
        $it->offsetUnset(2); // Remove the third element (previously the fourth element)
    } catch (Throwable $e) {
        printf("offsetUnset threw %s: %s\n", $e::class, $e->getMessage());
    }
    echo json_encode($it), "\n";
    var_dump($it);
}
?>
--EXPECTF--
Test Teds\Deque
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
[100,{},102,103,104]
object(Teds\Deque)#%d (5) {
  [0]=>
  int(100)
  [1]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  int(102)
  [3]=>
  int(103)
  [4]=>
  int(104)
}
Test Teds\Vector
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
[100,{},102,103,104]
object(Teds\Vector)#%d (5) {
  [0]=>
  int(100)
  [1]=>
  object(stdClass)#%d (0) {
  }
  [2]=>
  int(102)
  [3]=>
  int(103)
  [4]=>
  int(104)
}
Test Teds\LowMemoryVector
Insert invalid caught OutOfBoundsException: Index out of range
New size=7
[100,{},{},101,102,103,104]
[100,{},102,103,104]
object(Teds\LowMemoryVector)#%d (0) {
}
Test Teds\IntVector
Insert invalid caught OutOfBoundsException: Index out of range
Insert object threw TypeError: Illegal Teds\IntVector value type stdClass
New size=5
[100,101,102,103,104]
[100,102,104]
object(Teds\IntVector)#%d (3) {
  [0]=>
  int(100)
  [1]=>
  int(102)
  [2]=>
  int(104)
}
Test Teds\ImmutableSequence
Insert invalid caught Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
Insert in range threw Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
Insert object threw Teds\UnsupportedOperationException: Teds\ImmutableSequence is immutable
New size=0
[]
offsetUnset threw Teds\UnsupportedOperationException: Teds\ImmutableSequence does not support offsetUnset - it is immutable
[]
object(Teds\ImmutableSequence)#%d (0) {
}