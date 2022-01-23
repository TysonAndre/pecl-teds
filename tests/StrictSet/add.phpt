--TEST--
Teds\StrictSet add and remove
--FILE--
<?php

$it = new Teds\StrictSet();
$add = function ($value) use ($it) {
    var_dump($it->add($value));
};
$remove = function ($value) use ($it) {
    echo "Remove: ";
    var_dump($value);
    var_dump($it->remove($value));
    var_dump($it->remove($value));
};
$add(new stdClass());
$add(new stdClass());
$add(0);
$add(0);
$add('0');
$add(strtoupper('extra'));
foreach ($it as $value) {
    var_export($value);
    echo "\n";
}
var_dump($it);
echo "After removing key\n";
$remove(0);
printf("count=%d\n", count($it));
var_dump($it);
echo "Remove results\n";
foreach ($it->values() as $v) {
    $remove($v);
}
var_dump($it);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
(object) array(
)
(object) array(
)
0
'0'
'EXTRA'
object(Teds\StrictSet)#1 (5) {
  [0]=>
  object(stdClass)#4 (0) {
  }
  [1]=>
  object(stdClass)#5 (0) {
  }
  [2]=>
  int(0)
  [3]=>
  string(1) "0"
  [4]=>
  string(5) "EXTRA"
}
After removing key
Remove: int(0)
bool(true)
bool(false)
count=4
object(Teds\StrictSet)#1 (4) {
  [0]=>
  object(stdClass)#4 (0) {
  }
  [1]=>
  object(stdClass)#5 (0) {
  }
  [2]=>
  string(1) "0"
  [3]=>
  string(5) "EXTRA"
}
Remove results
Remove: object(stdClass)#4 (0) {
}
bool(true)
bool(false)
Remove: object(stdClass)#5 (0) {
}
bool(true)
bool(false)
Remove: string(1) "0"
bool(true)
bool(false)
Remove: string(5) "EXTRA"
bool(true)
bool(false)
object(Teds\StrictSet)#1 (0) {
}