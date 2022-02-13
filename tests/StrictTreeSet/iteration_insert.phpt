--TEST--
Teds\StrictTreeSet insert during iteration
--FILE--
<?php

function create_key(int $i) {
    return "x$i";
}
$map = new Teds\StrictTreeSet([create_key(0), create_key(2), create_key(5)]);
foreach ($map as $key => $value) {
    var_dump(
        $map->add(create_key(1)),
        $map->add(create_key(2)),
        $map->add(create_key(9)),
    );
    printf("Key: %s\nValue: %s\n", $key, $value);
}
var_dump($map);
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
Key: x0
Value: x0
bool(false)
bool(false)
bool(false)
Key: x1
Value: x1
bool(false)
bool(false)
bool(false)
Key: x2
Value: x2
bool(false)
bool(false)
bool(false)
Key: x5
Value: x5
bool(false)
bool(false)
bool(false)
Key: x9
Value: x9
object(Teds\StrictTreeSet)#1 (5) {
  [0]=>
  string(2) "x0"
  [1]=>
  string(2) "x1"
  [2]=>
  string(2) "x2"
  [3]=>
  string(2) "x5"
  [4]=>
  string(2) "x9"
}