--TEST--
Teds\LowMemoryVector can be serialized and unserialized
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
set_error_handler(function ($errno, $msg) { echo "Warning: $msg\n"; });

function test_serialize_values(...$values) {
    $vec = new Teds\LowMemoryVector();
    foreach ($values as $v) { $vec[] = $v; }
    echo json_encode($vec), "\n";
    $ser = serialize($vec);
    echo encode_raw_string($ser), "\n\n";
    echo "Unserialized: ";var_dump(unserialize($ser));
}
test_serialize_values();
test_serialize_values(0);
test_serialize_values(null);
test_serialize_values(0x7f, -0x80);
test_serialize_values(256, 0x7fff, -0x8000);
test_serialize_values(0x7fff, (int)-0x8000_0000);  // -0x8000_0001 is not an int in 32-bit php builds
test_serialize_values(0x7fff, (int)-0x8000_0000, []);
test_serialize_values('first', 'second');

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
?>
--EXPECT--
[]
"O:20:\"Teds\\LowMemoryVector\":0:{}"

Unserialized: object(Teds\LowMemoryVector)#3 (0) {
}
[0]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:2;i:1;s:1:\"\x00\";}"

Unserialized: object(Teds\LowMemoryVector)#3 (1) {
  [0]=>
  int(0)
}
[null]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:1;i:1;s:2:\"\x01\x03\";}"

Unserialized: object(Teds\LowMemoryVector)#3 (1) {
  [0]=>
  NULL
}
[127,-128]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:2;i:1;s:2:\"\x7f\x80\";}"

Unserialized: object(Teds\LowMemoryVector)#3 (2) {
  [0]=>
  int(127)
  [1]=>
  int(-128)
}
[256,32767,-32768]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:3;i:1;s:6:\"\x00\x01\xff\x7f\x00\x80\";}"

Unserialized: object(Teds\LowMemoryVector)#3 (3) {
  [0]=>
  int(256)
  [1]=>
  int(32767)
  [2]=>
  int(-32768)
}
[32767,-2147483648]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:4;i:1;s:8:\"\xff\x7f\x00\x00\x00\x00\x00\x80\";}"

Unserialized: object(Teds\LowMemoryVector)#3 (2) {
  [0]=>
  int(32767)
  [1]=>
  int(-2147483648)
}
[32767,-2147483648,[]]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:3:{i:0;i:32767;i:1;i:-2147483648;i:2;a:0:{}}}"

Unserialized: object(Teds\LowMemoryVector)#3 (3) {
  [0]=>
  int(32767)
  [1]=>
  int(-2147483648)
  [2]=>
  array(0) {
  }
}
["first","second"]
"O:20:\"Teds\\LowMemoryVector\":2:{i:0;i:7;i:1;a:2:{i:0;s:5:\"first\";i:1;s:6:\"second\";}}"

Unserialized: object(Teds\LowMemoryVector)#3 (2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
Caught Error: Cannot create dynamic property Teds\LowMemoryVector::$dynamicProp
O:20:"Teds\LowMemoryVector":2:{i:0;i:7;i:1;a:1:{i:0;O:8:"stdClass":0:{}}}
Entry:
int(0)
object(stdClass)#6 (0) {
}
bool(true)
Done
array(0) {
}