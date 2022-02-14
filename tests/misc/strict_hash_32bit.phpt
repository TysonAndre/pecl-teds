--TEST--
Test strict_hash() function
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
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
=> 0x000000005a0a0214
bool(false)
=> 0x0000000033632f72
bool(true)
=> 0x000000000bbc5cd0
int(-1)
=> 0x0000000027a7d2a1
int(0)
=> 0x0000000000000000
int(1)
=> 0x00000000d8582d5e
int(2)
=> 0x00000000b1b15abc
int(3)
=> 0x000000008a0a881a
int(4)
=> 0x000000006263b578
int(5)
=> 0x000000003bbce2d6
int(6)
=> 0x0000000014151035
int(7)
=> 0x00000000ec6d3d93
int(8)
=> 0x00000000c5c66af1
int(16)
=> 0x000000008b8dd5e2
int(24)
=> 0x00000000505440d4
int(2147483647)
=> 0x00000000931bb1fb
int(-2147483648)
=> 0x00000000938b21a6
float(0)
=> 0x0000000095c6e4ea
float(-0)
=> 0x0000000095c6e4ea (Already saw)
array(1) {
  [0]=>
  float(0)
}
=> 0x0000000073121d71
array(1) {
  [0]=>
  float(-0)
}
=> 0x0000000073121d71 (Already saw)
float(1.5)
=> 0x0000000009921f06
float(1.2)
=> 0x000000008cc75d02
float(2)
=> 0x00000000c2fc3a76
float(3)
=> 0x0000000060c201e1
float(1234567)
=> 0x00000000929ef487
string(0) ""
=> 0x000000008d0e4e35
string(1) "a"
=> 0x00000000e9170dca
string(1) "b"
=> 0x00000000c2703a28
array(0) {
}
=> 0x00000000e4148a2e
array(1) {
  [0]=>
  int(1)
}
=> 0x00000000aa55ec95
array(1) {
  [1]=>
  int(1)
}
=> 0x000000005da54c37
array(1) {
  ["a"]=>
  int(1)
}
=> 0x00000000fe809758
object(stdClass)#1 (0) {
}
=> 0x00000000322d2cab
object(stdClass)#2 (0) {
}
=> 0x000000000b865909
object(Closure)#3 (0) {
}
=> 0x00000000e3de8667
object(Closure)#4 (0) {
}
=> 0x00000000bc37b4c5
resource(1) of type (stream)
=> 0x00000000ac0e6043
float(INF)
=> 0x000000009802af26
float(-INF)
=> 0x00000000f26e5b3d
float(NAN)
=> 0x00000000903422a8
float(NAN)
=> 0x00000000903422a8 (Already saw)
float(NAN)
=> 0x00000000903422a8 (Already saw)