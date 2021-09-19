--TEST--
Teds\Deque clear
--FILE--
<?php

$it = new Teds\Deque(['first' => new stdClass()]);
var_dump($it->toArray());
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
$it->clear();
foreach ($it as $value) {
    echo "Not reached\n";
}
var_dump($it->toArray());
printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
count=1 capacity=4
array(0) {
}
count=0 capacity=0