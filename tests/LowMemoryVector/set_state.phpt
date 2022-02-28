--TEST--
Teds\LowMemoryVector::__set_state
--FILE--
<?php

// Deliberately use empty representation for var_export because there's no way to do that without
// significantly increasing the memory usage of LowMemoryVector, which defeats the point.
// https://github.com/php/php-src/issues/8044
//
// function dump_repr($obj) {
//     echo str_replace(" \n", "\n", var_export($obj, true)), "\n";
// }
// dump_repr(Teds\LowMemoryVector::__set_state([]));
Teds\LowMemoryVector::__set_state(['first' => 'x']);
$it = Teds\LowMemoryVector::__set_state([strtoupper('a literal'), ['first', 'x'], [(object)['key' => 'value'], null]]);
foreach ($it as $key => $value) {
    printf("key=%s value=%s\n", json_encode($key), json_encode($value));
}
// dump_repr($it);
var_dump($it);
var_dump((array)$it);

?>
--EXPECT--
key=0 value="A LITERAL"
key=1 value=["first","x"]
key=2 value=[{"key":"value"},null]
object(Teds\LowMemoryVector)#2 (0) {
}
array(3) {
  [0]=>
  string(9) "A LITERAL"
  [1]=>
  array(2) {
    [0]=>
    string(5) "first"
    [1]=>
    string(1) "x"
  }
  [2]=>
  array(2) {
    [0]=>
    object(stdClass)#1 (1) {
      ["key"]=>
      string(5) "value"
    }
    [1]=>
    NULL
  }
}