--TEST--
Teds\StrictSet single element
--FILE--
<?php
// discards keys
function create_key(string $x) {
    return "_$x";
}
$it = new Teds\StrictSet();
var_dump($it->add(create_key('1')));
var_dump($it->contains(create_key('1')));
var_dump($it->add(create_key('2')));
var_dump($it->add(create_key('2')));
echo "Remove\n";
var_dump($it->remove(create_key('1')));
var_dump($it->remove(create_key('1')));
var_dump($it);
var_dump($it->contains(create_key('1')));
var_dump($it->contains(create_key('2')));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
Remove
bool(true)
bool(false)
object(Teds\StrictSet)#1 (1) {
  [0]=>
  string(2) "_2"
}
bool(false)
bool(true)
