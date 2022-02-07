--TEST--
Teds\IntVector can be serialized and unserialized
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
set_error_handler(function ($errno, $msg) { echo "Warning: $msg\n"; });

function test_serialize_values(...$values) {
    $vec = new Teds\IntVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n\n";
    echo "Unserialized: ";var_dump(unserialize($ser));
}
test_serialize_values();
test_serialize_values(0);
test_serialize_values(0x7f, -0x80);
test_serialize_values(256, 0x7fff, -0x8000);
test_serialize_values(0x7fff, (int)-0x8000_0000);  // -0x8000_0001 is not an int in 32-bit php builds
try {
    test_serialize_values(0x7fff, (int)-0x8000_0000, []);
    echo "Should throw\n";
} catch (TypeError $e) {
    echo "Caught: {$e->getMessage()}\n";
}
try {
    $i = 123;
    test_serialize_values("dynamic$i");
} catch (TypeError $e) {
    echo "Caught: {$e->getMessage()}\n";
}
?>
--EXPECT--
[]
"O:14:\"Teds\\IntVector\":0:{}"

Unserialized: object(Teds\IntVector)#3 (0) {
}
[0]
"O:14:\"Teds\\IntVector\":2:{i:0;i:1;i:1;s:1:\"\x00\";}"

Unserialized: object(Teds\IntVector)#3 (1) {
  [0]=>
  int(0)
}
[127,-128]
"O:14:\"Teds\\IntVector\":2:{i:0;i:1;i:1;s:2:\"\x7f\x80\";}"

Unserialized: object(Teds\IntVector)#3 (2) {
  [0]=>
  int(127)
  [1]=>
  int(-128)
}
[256,32767,-32768]
"O:14:\"Teds\\IntVector\":2:{i:0;i:2;i:1;s:6:\"\x00\x01\xff\x7f\x00\x80\";}"

Unserialized: object(Teds\IntVector)#3 (3) {
  [0]=>
  int(256)
  [1]=>
  int(32767)
  [2]=>
  int(-32768)
}
[32767,-2147483648]
"O:14:\"Teds\\IntVector\":2:{i:0;i:3;i:1;s:8:\"\xff\x7f\x00\x00\x00\x00\x00\x80\";}"

Unserialized: object(Teds\IntVector)#3 (2) {
  [0]=>
  int(32767)
  [1]=>
  int(-2147483648)
}
Caught: Illegal Teds\IntVector value type array
Caught: Illegal Teds\IntVector value type string