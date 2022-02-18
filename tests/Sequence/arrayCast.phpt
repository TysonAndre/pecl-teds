--TEST--
Teds\Sequence to array
--FILE--
<?php
// discards keys
foreach ([
    Teds\Deque::class,
    Teds\Vector::class,
    Teds\LowMemoryVector::class,
] as $class_name) {
    echo "Test $class_name\n";
    $it = new $class_name(["v_$class_name"]);
    var_dump((array)$it);
    $it[0] = strtoupper('test2');
    var_dump((array)$it);
    var_dump($it);
    $it->pop();
    var_dump($it);
}


?>
--EXPECT--
Test Teds\Deque
array(1) {
  [0]=>
  string(12) "v_Teds\Deque"
}
array(1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\Deque)#1 (1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\Deque)#1 (0) {
}
Test Teds\Vector
array(1) {
  [0]=>
  string(13) "v_Teds\Vector"
}
array(1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\Vector)#2 (1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\Vector)#2 (0) {
}
Test Teds\LowMemoryVector
array(1) {
  [0]=>
  string(22) "v_Teds\LowMemoryVector"
}
array(1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\LowMemoryVector)#1 (1) {
  [0]=>
  string(5) "TEST2"
}
object(Teds\LowMemoryVector)#1 (0) {
}