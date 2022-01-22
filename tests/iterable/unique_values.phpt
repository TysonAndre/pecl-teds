--TEST--
Test unique_values() function on arrays
--FILE--
<?php

use function Teds\unique_values;

function dump_unique_values(iterable $input) {
    echo "For " . json_encode($input) . ": ";
    var_dump(unique_values($input));
}
/* Create a reference counted key */
function create_key(int $x) {
    return "k$x";
}
dump_unique_values([]);
dump_unique_values([true]);
dump_unique_values([create_key(1)]);
dump_unique_values([create_key(1), create_key(2), create_key(1)]);
?>
--EXPECT--
For []: array(0) {
}
For [true]: array(1) {
  [0]=>
  bool(true)
}
For ["k1"]: array(1) {
  [0]=>
  string(2) "k1"
}
For ["k1","k2","k1"]: array(2) {
  [0]=>
  string(2) "k1"
  [1]=>
  string(2) "k2"
}
