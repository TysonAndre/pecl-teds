--TEST--
Teds\BitSet can be cloned
--FILE--
<?php

$it = new Teds\BitSet([true]);
$it2 = clone $it;
unset($it);
foreach ($it2 as $key => $value) {
    echo "Saw entry:\n";
    var_dump($key, $value);
}
$it2[] = false;
$it2[] = true;
var_dump($it2);

?>
--EXPECT--
Saw entry:
int(0)
bool(true)
object(Teds\BitSet)#2 (3) {
  [0]=>
  bool(true)
  [1]=>
  bool(false)
  [2]=>
  bool(true)
}