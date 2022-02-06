--TEST--
Teds\IntVector::__set_state
--FILE--
<?php

// Deliberately use empty representation for var_export because there's no way to do that without
// significantly increasing the memory usage of IntVector, which defeats the point.
// https://github.com/php/php-src/issues/8044
//
// function dump_repr($obj) {
//     echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
// }
// dump_repr(Teds\IntVector::__set_state([]));
Teds\IntVector::__set_state(['first' => 1]);
$it = Teds\IntVector::__set_state([55, 129]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
// dump_repr($it);
var_dump($it);
var_dump((array)$it);

?>
--EXPECT--
key=0 value=55
key=1 value=129
object(Teds\IntVector)#1 (2) {
  [0]=>
  int(55)
  [1]=>
  int(129)
}
array(2) {
  [0]=>
  int(55)
  [1]=>
  int(129)
}