--TEST--
Teds\StrictHashSet add and grow
--FILE--
<?php
// discards keys
function create_key(int $x) {
    return "_$x";
}
$it = new Teds\StrictHashSet();
for ($i = 0; $i < 10; $i++) {
    $add = $it->add(create_key($i));
    printf("%d: %s\n", $i, json_encode($add));
}
var_dump($it);
echo "Contains\n";
for ($i = 0; $i < 10; $i++) {
    $contains = $it->contains(create_key($i));
    printf("%d: %s\n", $i, json_encode($contains));
}
echo "Remove\n";
for ($i = 0; $i < 10; $i++) {
    $remove = $it->remove(create_key($i));
    printf("%d: %s\n", $i, json_encode($remove));
}
var_dump($it);
?>
--EXPECT--
0: true
1: true
2: true
3: true
4: true
5: true
6: true
7: true
8: true
9: true
object(Teds\StrictHashSet)#1 (10) {
  [0]=>
  string(2) "_0"
  [1]=>
  string(2) "_1"
  [2]=>
  string(2) "_2"
  [3]=>
  string(2) "_3"
  [4]=>
  string(2) "_4"
  [5]=>
  string(2) "_5"
  [6]=>
  string(2) "_6"
  [7]=>
  string(2) "_7"
  [8]=>
  string(2) "_8"
  [9]=>
  string(2) "_9"
}
Contains
0: true
1: true
2: true
3: true
4: true
5: true
6: true
7: true
8: true
9: true
Remove
0: true
1: true
2: true
3: true
4: true
5: true
6: true
7: true
8: true
9: true
object(Teds\StrictHashSet)#1 (0) {
}