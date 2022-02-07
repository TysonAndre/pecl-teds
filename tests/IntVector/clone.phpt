--TEST--
Teds\IntVector can be cloned
--FILE--
<?php

$it = new Teds\IntVector([-128]);
$it2 = clone $it;
unset($it);
foreach ($it2 as $key => $value) {
    echo "Saw entry:\n";
    var_dump($key, $value);
}
$it2[] = 10501;
var_dump($it2);

?>
--EXPECT--
Saw entry:
int(0)
int(-128)
object(Teds\IntVector)#2 (2) {
  [0]=>
  int(-128)
  [1]=>
  int(10501)
}