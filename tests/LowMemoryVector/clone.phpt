--TEST--
Teds\LowMemoryVector can be cloned
--FILE--
<?php

$it = new Teds\LowMemoryVector([new stdClass(), new ArrayObject()]);
var_dump($it);
$it2 = clone $it;
unset($it);
foreach ($it2 as $key => $value) {
    echo "Saw entry:\n";
    var_dump($key, $value);
}

$it = new Teds\LowMemoryVector();
var_dump($it);
$it2 = clone($it);
echo json_encode($it2), "\n";
?>
--EXPECT--
object(Teds\LowMemoryVector)#1 (0) {
}
Saw entry:
int(0)
object(stdClass)#2 (0) {
}
Saw entry:
int(1)
object(ArrayObject)#3 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
object(Teds\LowMemoryVector)#1 (0) {
}
[]