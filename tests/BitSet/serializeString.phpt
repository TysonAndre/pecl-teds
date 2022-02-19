--TEST--
Teds\BitSet serialize/unserialize
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function test_serialize(array $bits) {
    $bitset = new Teds\BitSet($bits);
    $ser = $bitset->serialize();
    printf("Serialize %s: %s\n", json_encode($bits), encode_raw_string($ser));
    echo json_encode(Teds\BitSet::unserialize($ser)), "\n";

    $asPaddedBinaryString = $bitset->toBinaryString();
    printf("toBinaryString %s: %s\n", json_encode($bits), encode_raw_string($asPaddedBinaryString));
    echo json_encode(Teds\BitSet::fromBinaryString($asPaddedBinaryString)), "\n";  // Decoding a padded binary string will have length rounded up to a multiple of 8 with missing bits replaced with false (8 bits per byte)
}

test_serialize([]);
test_serialize([true, false, true, true, false]);
test_serialize([false, false, true, true, false, true, false, false]);
--EXPECT--
Serialize []: ""
[]
toBinaryString []: ""
[]
Serialize [true,false,true,true,false]: "\r\x03"
[true,false,true,true,false]
toBinaryString [true,false,true,true,false]: "\r"
[true,false,true,true,false,false,false,false]
Serialize [false,false,true,true,false,true,false,false]: ",\x00"
[false,false,true,true,false,true,false,false]
toBinaryString [false,false,true,true,false,true,false,false]: ","
[false,false,true,true,false,true,false,false]