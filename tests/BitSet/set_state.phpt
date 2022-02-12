--TEST--
Teds\BitSet::__set_state
--FILE--
<?php

// Deliberately use empty representation for var_export because there's no way to do that without
// significantly increasing the memory usage of BitSet, which defeats the point.
// https://github.com/php/php-src/issues/8044
//
// function dump_repr($obj) {
//     echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
// }
// dump_repr(Teds\BitSet::__set_state([]));
$it = Teds\BitSet::__set_state([true, false]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
// dump_repr($it);
var_dump($it);
var_dump((array)$it);

?>
--EXPECT--
key=0 value=true
key=1 value=false
object(Teds\BitSet)#1 (2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}
array(2) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
}