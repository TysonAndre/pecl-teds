--TEST--
Test unique_values() function on arrays with/without references
--FILE--
<?php

use function Teds\unique_values;

function dump_unique_values(iterable $input) {
    echo "For " . json_encode($input) . ": ";
    var_dump(unique_values($input));
}
/* Create a reference counted key */
$create_key = function (int $x): string {
    return "k$x";
};
dump_unique_values([]);
$x = $create_key(1);
$y = $create_key(2);
dump_unique_values([true]);
$orig = [&$x];
dump_unique_values($orig);
var_dump($orig);
$orig = [&$x, &$x];
dump_unique_values($orig);
var_dump($orig);

dump_unique_values([$x, &$y, $y, &$x]);
$original = [NAN];
dump_unique_values($original);
echo "Returns original array: ";
var_dump(Teds\is_same_array_handle($original, Teds\unique_values($original)));
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
array(1) {
  [0]=>
  &string(2) "k1"
}
For ["k1","k1"]: array(1) {
  [0]=>
  string(2) "k1"
}
array(2) {
  [0]=>
  &string(2) "k1"
  [1]=>
  &string(2) "k1"
}
For ["k1","k2","k2","k1"]: array(2) {
  [0]=>
  string(2) "k1"
  [1]=>
  string(2) "k2"
}
For : array(1) {
  [0]=>
  float(NAN)
}
Returns original array: bool(true)