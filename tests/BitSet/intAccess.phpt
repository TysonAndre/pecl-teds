--TEST--
Teds\BitSet getInt* implementation
--FILE--
<?php
$x = new Teds\BitSet();
$x->setSize(64);
$x->setInt8(0, 0x9f);
$x->setInt8(1, 0x13);
$x->setInt16(1, 0x7bcd); // byte 2..3, bit 16..31, avoid negative numbers in this test to also reuse test for 32-bit
$x->setInt32(1, 0x76543210); // bytes 4..7, avoid negative numbers in this test to also reuse test for 32-bit
echo "int8: ";
var_dump($x->getInt8(0));
echo "uint8: ";
var_dump($x->getUInt8(0));
echo "Fetching by int8:\n";
for ($i = 0; $i < 8; $i++) {
    $v = $x->getInt8($i);
    printf("%3d: %02x\n", $v, $v & 0xff);
}
try {
    $x->getInt8(8);
    echo "getInt8 should have thrown\n";
} catch (Exception $e) {
    printf("getInt8 caught %s: %s\n", $e::class, $e->getMessage());
}
try {
    $x->getInt32(2);
    echo "getInt32 should have thrown\n";
} catch (Exception $e) {
    printf("getInt32 caught %s: %s\n", $e::class, $e->getMessage());
}
try {
    $x->setInt8(8, 0);
    echo "setInt8 should have thrown\n";
} catch (Exception $e) {
    printf("setInt8 caught %s: %s\n", $e::class, $e->getMessage());
}
echo "Fetching by int16:\n";
for ($i = 0; $i < 4; $i++) {
    $v = $x->getInt16($i);
    printf("%6d: %04x\n", $v, $v & 0xffff);
}
echo "Fetching by int32\n";
for ($i = 0; $i < 2; $i++) {
    $v = $x->getInt32($i);
    printf("%11d: %08x\n", $v, $v);
}
--EXPECT--
int8: int(-97)
uint8: int(159)
Fetching by int8:
-97: 9f
 19: 13
-51: cd
123: 7b
 16: 10
 50: 32
 84: 54
118: 76
getInt8 caught OutOfBoundsException: Teds\BitSet int8 index invalid or out of range
getInt32 caught OutOfBoundsException: Teds\BitSet int32 index invalid or out of range
setInt8 caught OutOfBoundsException: Teds\BitSet int8 index invalid or out of range
Fetching by int16:
  5023: 139f
 31693: 7bcd
 12816: 3210
 30292: 7654
Fetching by int32
 2077037471: 7bcd139f
 1985229328: 76543210