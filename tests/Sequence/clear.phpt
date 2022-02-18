--TEST--
Teds\Deque clear
--FILE--
<?php

function test_sequence_clear(string $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(['first' => new stdClass()]);
    var_dump($it->toArray());
    printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
    $it->clear();
    foreach ($it as $value) {
        echo "Not reached\n";
    }
    var_dump($it->toArray());
    var_dump($it);
    printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
}
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
] as $class_name) {
    test_sequence_clear($class_name);
}
?>
--EXPECT--
Test Teds\Deque
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
count=1 capacity=4
array(0) {
}
object(Teds\Deque)#1 (0) {
}
count=0 capacity=0
Test Teds\Vector
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
count=1 capacity=1
array(0) {
}
object(Teds\Vector)#1 (0) {
}
count=0 capacity=0
Test Teds\LowMemoryVector
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
count=1 capacity=4
array(0) {
}
object(Teds\LowMemoryVector)#1 (0) {
}
count=0 capacity=0