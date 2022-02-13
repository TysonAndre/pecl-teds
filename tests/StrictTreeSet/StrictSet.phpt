--TEST--
Teds\StrictTreeSet constructed from array
--FILE--
<?php
// discards keys
$it = new Teds\StrictTreeSet(['first' => 'x', 'second' => new stdClass()]);
foreach ($it as $key => $value) {
    printf("Foreach Key: %s\nForeach Value: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
var_dump((array)$it);

$it = new Teds\StrictTreeSet([]);
var_dump($it);
var_dump((array)$it);
foreach ($it as $key => $value) {
    echo "Unreachable\n";
}

?>
--EXPECT--
Foreach Key: 'x'
Foreach Value: 'x'
Foreach Key: (object) array(
)
Foreach Value: (object) array(
)
object(Teds\StrictTreeSet)#1 (2) {
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
object(Teds\StrictTreeSet)#3 (0) {
}
array(0) {
}