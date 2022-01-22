--TEST--
Teds\StableSortedListSet add and remove
--FILE--
<?php

$it = new Teds\StableSortedListSet();
$add = function ($value) use ($it) {
    var_dump($it->add($value));
};
$remove = function ($value) use ($it) {
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
0
'0'
'EXTRA'
(object) array(
)
(object) array(
)
object(Teds\StableSortedListSet)#1 (5) {
  [0]=>
  int(0)
  [1]=>
  string(1) "0"
  [2]=>
  string(5) "EXTRA"
  [3]=>
  object(stdClass)#4 (0) {
  }
  [4]=>
  object(stdClass)#5 (0) {
  }
}
After removing key
bool(true)
count=4
object(Teds\StableSortedListSet)#1 (4) {
  [0]=>
  string(1) "0"
  [1]=>
  string(5) "EXTRA"
  [2]=>
  object(stdClass)#4 (0) {
  }
  [3]=>
  object(stdClass)#5 (0) {
  }
}
Remove results
bool(true)
bool(true)
bool(true)
bool(true)
object(Teds\StableSortedListSet)#1 (0) {
}