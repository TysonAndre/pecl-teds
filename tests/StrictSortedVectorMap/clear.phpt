--TEST--
Teds\StrictSortedVectorMap clear
--FILE--
<?php

$it = new Teds\StrictSortedVectorMap();
for ($i = 0; $i < 10; $i++) {
    $it["k$i"] = "v$i";
}
var_dump($it);
$it->clear();
echo "After clear\n";
var_dump($it);
?>
--EXPECT--
object(Teds\StrictSortedVectorMap)#1 (10) {
  [0]=>
  array(2) {
    [0]=>
    string(2) "k0"
    [1]=>
    string(2) "v0"
  }
  [1]=>
  array(2) {
    [0]=>
    string(2) "k1"
    [1]=>
    string(2) "v1"
  }
  [2]=>
  array(2) {
    [0]=>
    string(2) "k2"
    [1]=>
    string(2) "v2"
  }
  [3]=>
  array(2) {
    [0]=>
    string(2) "k3"
    [1]=>
    string(2) "v3"
  }
  [4]=>
  array(2) {
    [0]=>
    string(2) "k4"
    [1]=>
    string(2) "v4"
  }
  [5]=>
  array(2) {
    [0]=>
    string(2) "k5"
    [1]=>
    string(2) "v5"
  }
  [6]=>
  array(2) {
    [0]=>
    string(2) "k6"
    [1]=>
    string(2) "v6"
  }
  [7]=>
  array(2) {
    [0]=>
    string(2) "k7"
    [1]=>
    string(2) "v7"
  }
  [8]=>
  array(2) {
    [0]=>
    string(2) "k8"
    [1]=>
    string(2) "v8"
  }
  [9]=>
  array(2) {
    [0]=>
    string(2) "k9"
    [1]=>
    string(2) "v9"
  }
}
After clear
object(Teds\StrictSortedVectorMap)#1 (0) {
}