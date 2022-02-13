--TEST--
Teds\StrictTreeMap remove single value
--FILE--
<?php

$map = new Teds\StrictTreeMap();
$o1 = new stdClass();
$map[$o1] = new stdClass();
var_dump($map);
printf("count=%d\n", count($map));
unset($map[$o1]);
var_dump($map);
printf("count=%d\n", count($map));

?>
--EXPECT--
object(Teds\StrictTreeMap)#1 (1) {
  [0]=>
  array(2) {
    [0]=>
    object(stdClass)#2 (0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
}
count=1
object(Teds\StrictTreeMap)#1 (0) {
}
count=0