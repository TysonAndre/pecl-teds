--TEST--
Teds\SortedIntVectorSet create
--FILE--
<?php
require_once __DIR__ . '/../encode_raw_string.inc';
function test_create(iterable $values) {
    $it = new Teds\SortedIntVectorSet($values);
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
--EXPECTF--
[]
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":0:{}"
%STeds\SortedIntVectorSet::__set_state(array(
))
[1]
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:1;i:1;s:1:\"\x01\";}"
%STeds\SortedIntVectorSet::__set_state(array(
   0 => 1,
))
[1]
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:1;i:1;s:1:\"\x01\";}"
%STeds\SortedIntVectorSet::__set_state(array(
   0 => 1,
))
[0,1,2,3,4]
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:1;i:1;s:5:\"\x00\x01\x02\x03\x04\";}"
%STeds\SortedIntVectorSet::__set_state(array(
   0 => 0,
   1 => 1,
   2 => 2,
   3 => 3,
   4 => 4,
))
[0,1,2,3,4,5,6,7,8]
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:1;i:1;s:9:\"\x00\x01\x02\x03\x04\x05\x06\x07\x08\";}"
%STeds\SortedIntVectorSet::__set_state(array(
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
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:1;i:1;s:11:\"\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\";}"
%STeds\SortedIntVectorSet::__set_state(array(
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
Serialized: "O:23:\"Teds\\SortedIntVectorSet\":2:{i:0;i:3;i:1;s:12:\"\xc8\xa3Y\xfc\x00\x00\x00\x00@\xe2\x01\x00\";}"
%STeds\SortedIntVectorSet::__set_state(array(
   0 => -61234232,
   1 => 0,
   2 => 123456,
))