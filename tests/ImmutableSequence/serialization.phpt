--TEST--
Teds\ImmutableSequence can be serialized and unserialized
--FILE--
<?php

$it = new Teds\ImmutableSequence(['first' => new stdClass()]);
try {
    $it->dynamicProp = 123;
} catch (Throwable $t) {
    printf("Caught %s: %s\n", $t::class, $t->getMessage());
}
$ser = serialize($it);
echo $ser, "\n";
foreach (unserialize($ser) as $key => $value) {
    echo "Entry:\n";
    var_dump($key, $value);
}
var_dump($ser === serialize($it));
echo "Done\n";
$x = 123;
$it = new Teds\ImmutableSequence([]);
var_dump($it->__serialize());
?>
--EXPECT--
Caught Error: Cannot create dynamic property Teds\ImmutableSequence::$dynamicProp
O:22:"Teds\ImmutableSequence":1:{i:0;O:8:"stdClass":0:{}}
Entry:
int(0)
object(stdClass)#5 (0) {
}
bool(true)
Done
array(0) {
}