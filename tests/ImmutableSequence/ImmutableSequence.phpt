--TEST--
Teds\ImmutableSequence constructed from array
--FILE--
<?php
// discards keys
$it = new Teds\ImmutableSequence(['first' => 'x', 'second' => new stdClass()]);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Teds\ImmutableSequence([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

?>
--EXPECT--
Key: 0
Value: 'x'
Key: 1
Value: (object) array(
)
object(Teds\ImmutableSequence)#1 (2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
array(2) {
  [0]=>
  string(1) "x"
  [1]=>
  object(stdClass)#2 (0) {
  }
}
object(Teds\ImmutableSequence)#3 (0) {
}
array(0) {
}