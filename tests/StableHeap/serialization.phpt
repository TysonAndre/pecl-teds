--TEST--
Teds\StableMinHeap can be serialized and unserialized
--FILE--
<?php
function create_val(string $k) {
    return "val$k";
}

$it = new Teds\StableMinHeap(array_map('create_val', ['a', 'c', 'b', 'id', 'ue', 'd']));
try {
    $it->dynamicProp = 123;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
$ser = serialize($it);
echo $ser, "\n";
foreach ($it as $key => $value) {
    echo "Entry:\n";
    var_dump($key, $value);
}
var_dump(unserialize($ser));
foreach (unserialize($ser) as $key => $value) {
    echo "Entry:\n";
    var_dump($key, $value);
}
?>
--EXPECT--
Caught Error: Cannot create dynamic property Teds\StableMinHeap::$dynamicProp
O:18:"Teds\StableMinHeap":6:{i:0;s:4:"vala";i:1;s:4:"valb";i:2;s:4:"valc";i:3;s:5:"valid";i:4;s:5:"value";i:5;s:4:"vald";}
Entry:
string(4) "vala"
string(4) "vala"
Entry:
string(4) "valb"
string(4) "valb"
Entry:
string(4) "valc"
string(4) "valc"
Entry:
string(4) "vald"
string(4) "vald"
Entry:
string(5) "valid"
string(5) "valid"
Entry:
string(5) "value"
string(5) "value"
object(Teds\StableMinHeap)#3 (6) {
  [0]=>
  string(4) "vala"
  [1]=>
  string(4) "valb"
  [2]=>
  string(4) "valc"
  [3]=>
  string(5) "valid"
  [4]=>
  string(5) "value"
  [5]=>
  string(4) "vald"
}
Entry:
string(4) "vala"
string(4) "vala"
Entry:
string(4) "valb"
string(4) "valb"
Entry:
string(4) "valc"
string(4) "valc"
Entry:
string(4) "vald"
string(4) "vald"
Entry:
string(5) "valid"
string(5) "valid"
Entry:
string(5) "value"
string(5) "value"