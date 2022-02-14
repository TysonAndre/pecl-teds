--TEST--
Test strict_hash() function on array references
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
--FILE--
<?php

function dump_hash($value) {
    var_dump($value);
    printf("=>%016x\n", Teds\strict_hash($value));
}
$arr = [1, 2];
dump_hash($arr);
$a = &$arr[0];
dump_hash($arr);  // same hash
dump_hash([1 => 2, 0 => 1]); // different hash for different key order
function create_array(int $i) {
    return [$i];
}
$arr = [create_array(0), create_array(0)];
dump_hash($arr);
$arr[0] = &$arr[1];
dump_hash($arr);

?>
--EXPECT--
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
=>00000000a6b43259
array(2) {
  [0]=>
  &int(1)
  [1]=>
  int(2)
}
=>00000000a6b43259
array(2) {
  [1]=>
  int(2)
  [0]=>
  int(1)
}
=>000000006343e9c9
array(2) {
  [0]=>
  array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  array(1) {
    [0]=>
    int(0)
  }
}
=>0000000070108f71
array(2) {
  [0]=>
  &array(1) {
    [0]=>
    int(0)
  }
  [1]=>
  &array(1) {
    [0]=>
    int(0)
  }
}
=>0000000070108f71