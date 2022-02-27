--TEST--
Teds\StrictMinHeap and StrictMaxHeap
--FILE--
<?php
function create_key(string $suffix) {
    return "k$suffix";
}
call_user_func(function () {
    $minHeap = new Teds\StrictMinHeap([1]);
    var_dump($minHeap->current());
});
call_user_func(function () {
    $minHeap = new Teds\StrictMinHeap([create_key('x')]);
    var_dump($minHeap->current());
});
call_user_func(function () {
    echo "Test StrictMinHeap\n";
    $minHeap = new Teds\StrictMinHeap();
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
    echo "Test StrictMinHeap\n";
    $minHeap = new Teds\StrictMinHeap();
    foreach (['19', '9', '6', '1a', '2b', '3', '2', false, true, null, 1, 1.5, 2, [1], [0], [2]] as $v) {
        $minHeap->add($v);
    }
    var_dump($minHeap);
    foreach ($minHeap as $value) { echo "foreach: "; var_dump($value); }
    var_dump($minHeap);
    echo "Test StrictMaxHeap\n";
    $maxHeap = new Teds\StrictMaxHeap();
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
Test StrictMinHeap
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
Test StrictMinHeap
object(Teds\StrictMinHeap)#2 (16) {
  [0]=>
  NULL
  [1]=>
  bool(false)
  [2]=>
  bool(true)
  [3]=>
  int(2)
  [4]=>
  string(2) "19"
  [5]=>
  int(1)
  [6]=>
  string(2) "1a"
  [7]=>
  string(1) "2"
  [8]=>
  string(1) "9"
  [9]=>
  string(2) "2b"
  [10]=>
  string(1) "3"
  [11]=>
  float(1.5)
  [12]=>
  string(1) "6"
  [13]=>
  array(1) {
    [0]=>
    int(1)
  }
  [14]=>
  array(1) {
    [0]=>
    int(0)
  }
  [15]=>
  array(1) {
    [0]=>
    int(2)
  }
}
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
object(Teds\StrictMinHeap)#2 (0) {
}
Test StrictMaxHeap
object(Teds\StrictMaxHeap)#3 (16) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
  [1]=>
  array(1) {
    [0]=>
    int(1)
  }
  [2]=>
  string(1) "3"
  [3]=>
  array(1) {
    [0]=>
    int(0)
  }
  [4]=>
  string(2) "19"
  [5]=>
  string(2) "2b"
  [6]=>
  string(1) "2"
  [7]=>
  string(1) "9"
  [8]=>
  bool(true)
  [9]=>
  NULL
  [10]=>
  int(1)
  [11]=>
  float(1.5)
  [12]=>
  int(2)
  [13]=>
  string(2) "1a"
  [14]=>
  bool(false)
  [15]=>
  string(1) "6"
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
object(Teds\StrictMaxHeap)#3 (0) {
}