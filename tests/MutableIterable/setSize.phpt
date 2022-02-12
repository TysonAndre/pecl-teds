--TEST--
Teds\MutableIterable setSize
--FILE--
<?php
function show(Teds\MutableIterable $it) {
    printf("count=%d capacity=%d\n", $it->count(), $it->capacity());
    var_dump($it->toPairs());
}

$it = new Teds\MutableIterable();
show($it);
$it->setSize(2);
$it->setValueAt(0, new stdClass());
$it->setKeyAt(0, new ArrayObject());
show($it);
$it->setSize(0);
show($it);
$it->setSize(2);
$it->setValueAt(1, new stdClass());
show($it);
$it->clear();
show($it);
?>
--EXPECT--
count=0 capacity=0
array(0) {
}
count=2 capacity=2
array(2) {
  [0]=>
  array(2) {
    [0]=>
    object(ArrayObject)#3 (1) {
      ["storage":"ArrayObject":private]=>
      array(0) {
      }
    }
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
  [1]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
}
count=0 capacity=0
array(0) {
}
count=2 capacity=2
array(2) {
  [0]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    NULL
  }
  [1]=>
  array(2) {
    [0]=>
    NULL
    [1]=>
    object(stdClass)#2 (0) {
    }
  }
}
count=0 capacity=0
array(0) {
}