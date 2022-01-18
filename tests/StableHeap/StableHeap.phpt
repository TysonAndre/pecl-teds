--TEST--
Teds\StableMinHeap and StableMaxHeap
--FILE--
<?php
echo "Test StableMinHeap\n";
$minHeap = new Teds\StableMinHeap();
foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
    $minHeap->insert($v);
}
foreach ($minHeap as $value) { var_dump($value); }
echo "Test StableMaxHeap\n";
$maxHeap = new Teds\StableMaxHeap();
foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
    $maxHeap->insert($v);
}
foreach ($maxHeap as $value) { var_dump($value); }
var_dump($maxHeap);
echo "Comparison results\n";
var_dump($minHeap->compare('10', '9'));
var_dump($maxHeap->compare('10', '9'));
?>
--EXPECT--
Test StableMinHeap
NULL
bool(false)
bool(true)
int(1)
float(1.5)
int(2)
string(2) "19"
string(2) "1a"
string(1) "2"
string(2) "2b"
string(1) "3"
string(1) "6"
string(1) "9"
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
Test StableMaxHeap
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(0)
}
string(1) "9"
string(1) "6"
string(1) "3"
string(2) "2b"
string(1) "2"
string(2) "1a"
string(2) "19"
int(2)
float(1.5)
int(1)
bool(true)
bool(false)
NULL
object(Teds\StableMaxHeap)#2 (3) {
  ["flags":"SplHeap":private]=>
  int(0)
  ["isCorrupted":"SplHeap":private]=>
  bool(false)
  ["heap":"SplHeap":private]=>
  array(0) {
  }
}
Comparison results
int(1)
int(-1)