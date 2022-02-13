--TEST--
Teds\StrictHashSet add and remove repeatedly
--FILE--
<?php
// discards keys
function create_key(int $x) {
    return "_$x";
}
$it = new Teds\StrictHashSet();
$it->add(create_key(-1));
for ($i = 0; $i < 30; $i++) {
    $add = $it->add(create_key($i + 1));
    $remove = $it->remove(create_key($i));
    printf("%d: %s %s\n", $i, json_encode($add), json_encode($remove));
}
var_dump($it);
?>
--EXPECT--
0: true false
1: true true
2: true true
3: true true
4: true true
5: true true
6: true true
7: true true
8: true true
9: true true
10: true true
11: true true
12: true true
13: true true
14: true true
15: true true
16: true true
17: true true
18: true true
19: true true
20: true true
21: true true
22: true true
23: true true
24: true true
25: true true
26: true true
27: true true
28: true true
29: true true
object(Teds\StrictHashSet)#1 (2) {
  [0]=>
  string(3) "_-1"
  [1]=>
  string(3) "_30"
}
