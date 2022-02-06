--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';

function test_serialize_values(...$values) {
    $vec = new Teds\LowMemoryVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    echo encode_raw_string(serialize($vec)), "\n";
}
test_serialize_values();
test_serialize_values(0);
test_serialize_values(null);
test_serialize_values(127, -128);
test_serialize_values(256, 0x7fff, -0x8000);
test_serialize_values(0x7fff, (int)-0x8000000);
test_serialize_values(0x7fff, (int)-0x8000000, []);
test_serialize_values('first', 'second');
/*
$it = new Teds\LowMemoryVector([new stdClass()]);
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
$it = new Teds\LowMemoryVector([]);
var_dump($it->__serialize());
 */
?>
--EXPECTF--
[]
"O:20:\"Teds\\LowMemoryVector\":0:{}"
[0]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:2;i:1;s:1:\"\x00\";}"
[null]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:1:\"\x01\";}"
[127,-128]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:2;i:1;s:2:\"\x7f\x80\";}"
[256,32767,-32768]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:3;i:1;s:12:\"\x00\x01\x00\x00\xff\x7f\x00\x00\x00\x80\xff\xff\";}"
[32767,-134217728]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:3;i:1;s:8:\"\xff\x7f\x00\x00\x00\x00\x00\xf8\";}"
[32767,-134217728,[]]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;a:3:{i:0;i:32767;i:1;i:-134217728;i:2;a:0:{}}}"
["first","second"]

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 8
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:5;i:1;a:2:{i:0;s:5:\"first\";i:1;s:6:\"second\";}}"