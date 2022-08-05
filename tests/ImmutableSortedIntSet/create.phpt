--TEST--
Teds\ImmutableSortedIntSet create
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function test_create(iterable $values) {
    $it = new Teds\ImmutableSortedIntSet($values);
    echo json_encode($it), "\n";
    $ser = serialize($it);
    echo "Serialized: ", encode_raw_string($ser), "\n";;
    $result = unserialize($ser);
    var_export($result);
    echo "\n";
}
test_create([]);
test_create([1]);
test_create([1, 1]);
test_create([3, 1, 2, 4, 0, 0]);
test_create([0, 1, 2, 3, 4, 5, 6, 7, 8]);
test_create([10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0]);
test_create([0, 123456, 0, -61234232]);
test_create([0, -129, 70000, 17920000]);
--EXPECTF--
[]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":0:{}"
%STeds\ImmutableSortedIntSet::__set_state(array(
))
[1]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:1;i:1;s:1:\"\x01\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => 1,
))
[1]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:1;i:1;s:1:\"\x01\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => 1,
))
[0,1,2,3,4]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:1;i:1;s:5:\"\x00\x01\x02\x03\x04\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => 0,
   1 => 1,
   2 => 2,
   3 => 3,
   4 => 4,
))
[0,1,2,3,4,5,6,7,8]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:1;i:1;s:9:\"\x00\x01\x02\x03\x04\x05\x06\x07\x08\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => 0,
   1 => 1,
   2 => 2,
   3 => 3,
   4 => 4,
   5 => 5,
   6 => 6,
   7 => 7,
   8 => 8,
))
[0,1,2,3,4,5,6,7,8,9,10]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:1;i:1;s:11:\"\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => 0,
   1 => 1,
   2 => 2,
   3 => 3,
   4 => 4,
   5 => 5,
   6 => 6,
   7 => 7,
   8 => 8,
   9 => 9,
   10 => 10,
))
[-61234232,0,123456]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:3;i:1;s:12:\"\xc8\xa3Y\xfc\x00\x00\x00\x00@\xe2\x01\x00\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => -61234232,
   1 => 0,
   2 => 123456,
))
[-129,0,70000,17920000]
Serialized: "O:26:\"Teds\\ImmutableSortedIntSet\":2:{i:0;i:3;i:1;s:16:\"\x7f\xff\xff\xff\x00\x00\x00\x00p\x11\x01\x00\x00p\x11\x01\";}"
%STeds\ImmutableSortedIntSet::__set_state(array(
   0 => -129,
   1 => 0,
   2 => 70000,
   3 => 17920000,
))