--TEST--
Test stable_compare() function
--FILE--
<?php

function dump_stable_compare($a, $b) {
    printf("stable_compare(%s, %s): %d (opposite %d)\n", var_export($a, true), var_export($b, true), Teds\stable_compare($a, $b), Teds\stable_compare($b, $a));
}

$o1 = (object)['k' => 1];
$o2 = (object)['k' => 2];  // meaningless value to distinguish output, sorted by object handle
$r1 = fopen('php://memory', 'rw');
dump_stable_compare(null, null);
dump_stable_compare(false, false);
dump_stable_compare(true, true);
dump_stable_compare(null, false);
dump_stable_compare(false, null);
dump_stable_compare(false, true);
dump_stable_compare(true, 0);
dump_stable_compare(0.0, []);
dump_stable_compare([], $o1);
dump_stable_compare($o1, $r1);
dump_stable_compare(0, 0);
dump_stable_compare(0, 0.0);
dump_stable_compare(0.0, 0.0);
dump_stable_compare(0, 1);
dump_stable_compare(0, 1.0);
dump_stable_compare(1, 0);
dump_stable_compare(1.0, 0);
dump_stable_compare(0.0001, 0);
dump_stable_compare(0.0, 1.0);
dump_stable_compare(0, '0');  // number always less than string
dump_stable_compare(1, '0');
dump_stable_compare('10', '9');
dump_stable_compare("A", "A\0");
dump_stable_compare("A", "A");
dump_stable_compare(0.0, -0.0);
dump_stable_compare([], []);
dump_stable_compare([], [1]);
dump_stable_compare([1], [1]);
dump_stable_compare([1], ['a' => 1]);
dump_stable_compare([1], [2]);
dump_stable_compare(['a' => 1, 'b' => 1], ['b' => 1, 'a' => 1]);

echo "Test compare objects\n";
dump_stable_compare(spl_object_id($o1), spl_object_id($o2));
dump_stable_compare($o1, $o2);
dump_stable_compare($o1, $o1);
echo "Test sort\n";
$values = [
    $r1,
    $o2,
    $o1,
    [],
    [1],
    [2],
    ['a' => 1],
    "a",
    "b",
    "aa",
    "0",
    "!",
    0.0,
    0.5,
    0,
    1,
    true,
    false,
    null,
];
usort($values, 'Teds\stable_compare');
echo "After stable sort\n";
var_dump($values);
echo "\n";
echo "Test floating point edge cases\n";
$big = (int)(float)(PHP_INT_MAX - 1000000);
dump_stable_compare($big, (float)$big);
dump_stable_compare($big - 1, (float)$big);
dump_stable_compare($big + 1, (float)$big);
dump_stable_compare($big, NAN);
dump_stable_compare($big, INF);
dump_stable_compare($big, -INF);
dump_stable_compare(0.0, NAN);
dump_stable_compare(0, NAN);
dump_stable_compare(INF, NAN);
dump_stable_compare(NAN, NAN);
?>
--EXPECTF--
stable_compare(NULL, NULL): 0 (opposite 0)
stable_compare(false, false): 0 (opposite 0)
stable_compare(true, true): 0 (opposite 0)
stable_compare(NULL, false): -1 (opposite 1)
stable_compare(false, NULL): 1 (opposite -1)
stable_compare(false, true): -1 (opposite 1)
stable_compare(true, 0): -1 (opposite 1)
stable_compare(0.0, array (
)): -1 (opposite 1)
stable_compare(array (
), (object) array(
   'k' => 1,
)): -1 (opposite 1)
stable_compare((object) array(
   'k' => 1,
), NULL): -1 (opposite 1)
stable_compare(0, 0): 0 (opposite 0)
stable_compare(0, 0.0): -1 (opposite 1)
stable_compare(0.0, 0.0): 0 (opposite 0)
stable_compare(0, 1): -1 (opposite 1)
stable_compare(0, 1.0): -1 (opposite 1)
stable_compare(1, 0): 1 (opposite -1)
stable_compare(1.0, 0): 1 (opposite -1)
stable_compare(0.0001, 0): 1 (opposite -1)
stable_compare(0.0, 1.0): -1 (opposite 1)
stable_compare(0, '0'): -1 (opposite 1)
stable_compare(1, '0'): -1 (opposite 1)
stable_compare('10', '9'): -1 (opposite 1)
stable_compare('A', 'A' . "\0" . ''): -1 (opposite 1)
stable_compare('A', 'A'): 0 (opposite 0)
stable_compare(0.0, -0.0): 0 (opposite 0)
stable_compare(array (
), array (
)): 0 (opposite 0)
stable_compare(array (
), array (
  0 => 1,
)): -1 (opposite 1)
stable_compare(array (
  0 => 1,
), array (
  0 => 1,
)): 0 (opposite 0)
stable_compare(array (
  0 => 1,
), array (
  'a' => 1,
)): -1 (opposite 1)
stable_compare(array (
  0 => 1,
), array (
  0 => 2,
)): -1 (opposite 1)
stable_compare(array (
  'a' => 1,
  'b' => 1,
), array (
  'b' => 1,
  'a' => 1,
)): -1 (opposite 1)
Test compare objects
stable_compare(1, 2): -1 (opposite 1)
stable_compare((object) array(
   'k' => 1,
), (object) array(
   'k' => 2,
)): -1 (opposite 1)
stable_compare((object) array(
   'k' => 1,
), (object) array(
   'k' => 1,
)): 0 (opposite 0)
Test sort
After stable sort
array(19) {
  [0]=>
  NULL
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  int(0)
  [4]=>
  float(0)
  [5]=>
  float(0.5)
  [6]=>
  int(1)
  [7]=>
  string(1) "!"
  [8]=>
  string(1) "0"
  [9]=>
  string(1) "a"
  [10]=>
  string(2) "aa"
  [11]=>
  string(1) "b"
  [12]=>
  array(0) {
  }
  [13]=>
  array(1) {
    [0]=>
    int(1)
  }
  [14]=>
  array(1) {
    [0]=>
    int(2)
  }
  [15]=>
  array(1) {
    ["a"]=>
    int(1)
  }
  [16]=>
  object(stdClass)#1 (1) {
    ["k"]=>
    int(1)
  }
  [17]=>
  object(stdClass)#2 (1) {
    ["k"]=>
    int(2)
  }
  [18]=>
  resource(5) of type (stream)
}

Test floating point edge cases
stable_compare(%s, %s): -1 (opposite 1)
stable_compare(%s, %s): -1 (opposite 1)
stable_compare(%s, %s): 1 (opposite -1)
stable_compare(%s, NAN): -1 (opposite 1)
stable_compare(%s, INF): -1 (opposite 1)
stable_compare(%s, -INF): 1 (opposite -1)
stable_compare(0.0, NAN): -1 (opposite 1)
stable_compare(0, NAN): -1 (opposite 1)
stable_compare(INF, NAN): -1 (opposite 1)
stable_compare(NAN, NAN): 0 (opposite 0)