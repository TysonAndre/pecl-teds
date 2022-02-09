--TEST--
Teds\StableMinHeap and StableMaxHeap
--FILE--
<?php
function create_key(string $suffix) {
    return "k$suffix";
}
call_user_func(function () {
    $minHeap = new Teds\StableMinHeap([1]);
    var_dump($minHeap->current());
});
call_user_func(function () {
    $minHeap = new Teds\StableMinHeap([create_key('x')]);
    var_dump($minHeap->current());
});
call_user_func(function () {
    echo "Test StableMinHeap\n";
    $minHeap = new Teds\StableMinHeap();
    foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
        $minHeap->add($v);
    }
    while ($minHeap->valid()) {
        echo "next: ";
        var_dump($minHeap->current());
        $minHeap->next();
    }
});
call_user_func(function () {
    echo "Test StableMinHeap\n";
    $minHeap = new Teds\StableMinHeap();
    foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
        $minHeap->add($v);
    }
    foreach ($minHeap as $value) { echo "foreach: "; var_dump($value); }
    echo "Test StableMaxHeap\n";
    $maxHeap = new Teds\StableMaxHeap();
    foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
        $maxHeap->add($v);
    }
    var_dump($maxHeap);
    foreach ($maxHeap as $value) { var_dump($value); }
    var_dump($maxHeap);
});
?>
--EXPECT--
int(1)
string(2) "kx"
Test StableMinHeap
next: NULL
next: bool(false)
next: bool(true)
next: int(1)
next: float(1.5)
next: int(2)
next: string(2) "19"
next: string(2) "1a"
next: string(1) "2"
next: string(2) "2b"
next: string(1) "3"
next: string(1) "6"
next: string(1) "9"
next: array(1) {
  [0]=>
  int(0)
}
next: array(1) {
  [0]=>
  int(1)
}
next: array(1) {
  [0]=>
  int(2)
}
Test StableMinHeap
foreach: NULL
foreach: bool(false)
foreach: bool(true)
foreach: int(1)
foreach: float(1.5)
foreach: int(2)
foreach: string(2) "19"
foreach: string(2) "1a"
foreach: string(1) "2"
foreach: string(2) "2b"
foreach: string(1) "3"
foreach: string(1) "6"
foreach: string(1) "9"
foreach: array(1) {
  [0]=>
  int(0)
}
foreach: array(1) {
  [0]=>
  int(1)
}
foreach: array(1) {
  [0]=>
  int(2)
}
Test StableMaxHeap
object(Teds\StableMaxHeap)#3 (0) {
}
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
object(Teds\StableMaxHeap)#3 (0) {
}