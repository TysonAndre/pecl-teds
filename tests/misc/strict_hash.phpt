--TEST--
Test strict_hash() function
--SKIPIF--
<?php if (PHP_INT_SIZE < 8) echo "skip 64-bit only\n"; ?>
--FILE--
<?php

$zero = 0.0;
$values = [
    null,
    false,
    true,
    -1,
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    16,
    24,
    PHP_INT_MAX,
    PHP_INT_MIN,
    0.0,
    -0.0,  // Deliberately return the same hash as 0.0, because php has 0.0 === -0.0 and it's equivalent in *almost* all ways, apart from 1 / -0.0
    [0.0],
    [-0.0],  // Deliberately return the same hash as 0.0, because php has 0.0 === -0.0 and it's equivalent in *almost* all ways, apart from 1 / -0.0
    1.5,
    1.2,
    2.0,
    3.0,
    1234567.0,
    '',
    'a',
    'b',
    [],
    [1],
    [1 => 1],
    ['a' => 1],
    new stdClass(),
    new stdClass(),
    function () {},
    function () {},
    STDIN,
    INF,
    -INF,
    NAN,
    -NAN,
    fdiv($zero, $zero),
];

$all = [];
foreach ($values as $value) {
    var_dump($value);
    $hash = Teds\strict_hash($value);
    printf("=> 0x%016x", $hash);
    if (isset($all[$hash])) {
        echo " (Already saw)";
    }
    echo "\n";
    $all[$hash] = $value;
}
?>
--EXPECT--
NULL
=> 0x06742e765a0a0214
bool(false)
=> 0xdf5ceb2933632f72
bool(true)
=> 0xb845a8dd0bbc5cd0
int(-1)
=> 0x2717434c27a7d2a1
int(0)
=> 0x0000000000000000
int(1)
=> 0xd9e8bcb3d8582d5e
int(2)
=> 0xb2d17967b1b15abc
int(3)
=> 0x8bba361b8a0a881a
int(4)
=> 0x64a3f3ce6263b578
int(5)
=> 0x3d8cb0823bbce2d6
int(6)
=> 0x16756d3614151035
int(7)
=> 0xef5d2aeaec6d3d93
int(8)
=> 0xc846e79dc5c66af1
int(16)
=> 0x908dce3b8b8dd5e2
int(24)
=> 0x58d4b5d9505440d4
int(9223372036854775807)
=> 0x2717434c27a7d221
int(-9223372036854775808)
=> 0x0000000000000080
float(0)
=> 0x4300dff895c6e4ea
float(-0)
=> 0x4300dff895c6e4ea (Already saw)
array(1) {
  [0]=>
  float(0)
}
=> 0x8776bedd1ff90617
array(1) {
  [0]=>
  float(-0)
}
=> 0x8776bedd1ff90617 (Already saw)
float(1.5)
=> 0xaa85ee3509921f06
float(1.2)
=> 0xaa489d23c7659e11
float(2)
=> 0x833619e8c2fc3a76
float(3)
=> 0x83fe5fcf60c201e1
float(1234567)
=> 0x5c81b6956b87b13b
string(0) ""
=> 0x3d59ca01fa822c0f
string(1) "a"
=> 0x16bba955568ceba3
string(1) "b"
=> 0xefa366092fe51802
array(0) {
}
=> 0x912e6591e4148a2e
array(1) {
  [0]=>
  int(1)
}
=> 0xdbd97feb27d6de6e
array(1) {
  [1]=>
  int(1)
}
=> 0x2f748d5275637e46
array(1) {
  ["a"]=>
  int(1)
}
=> 0x8c339b0310248bd2
object(stdClass)#1 (0) {
}
=> 0xdfea5698322d2cab
object(stdClass)#2 (0) {
}
=> 0xb8d3134c0b865909
object(Closure)#3 (0) {
}
=> 0x91bcd0ffe3de8667
object(Closure)#4 (0) {
}
=> 0x6aa58db3bc37b4c5
resource(1) of type (stream)
=> 0x6b4e880bac0e6043
float(INF)
=> 0xeaf3e13d9802af26
float(-INF)
=> 0x6a60561cf26e5b3d
float(NAN)
=> 0x6a289d03903422a8
float(NAN)
=> 0x6a289d03903422a8 (Already saw)
float(NAN)
=> 0x6a289d03903422a8 (Already saw)