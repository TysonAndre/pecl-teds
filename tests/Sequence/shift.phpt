--TEST--
Teds\Sequence implementations - test shift/unshift
--FILE--
<?php

function test_shift_string(Teds\Sequence $sequence) {
    echo __FUNCTION__, ": ", get_class($sequence) . ":\n";
    for ($i = 0; $i < 2; $i++) {
        $sequence->unshift("v$i");
    }
    $sequence->unshift("v$i", "extra");
    var_dump($sequence);
    while (!$sequence->isEmpty()) {
        var_dump($sequence->shift());
    }
    echo "\n";
}
function test_shift_int(Teds\Sequence $sequence) {
    echo __FUNCTION__, ": ", get_class($sequence) . ":\n";
    $sequence->unshift(100, 10);
    $sequence->unshift(11);
    var_dump($sequence);
    while (!$sequence->isEmpty()) {
        var_dump($sequence->shift());
    }
    echo "\n";
}

$impls = [Teds\Vector::class, Teds\LowMemoryVector::class, Teds\Deque::class];
foreach ($impls as $impl) {
    test_shift_string(new $impl());
}
$impls[] = Teds\IntVector::class;
foreach ($impls as $impl) {
    test_shift_int(new $impl());
}
?>
--EXPECT--
test_shift_string: Teds\Vector:
object(Teds\Vector)#1 (4) {
  [0]=>
  string(5) "extra"
  [1]=>
  string(2) "v2"
  [2]=>
  string(2) "v1"
  [3]=>
  string(2) "v0"
}
string(5) "extra"
string(2) "v2"
string(2) "v1"
string(2) "v0"

test_shift_string: Teds\LowMemoryVector:
object(Teds\LowMemoryVector)#1 (0) {
}
string(5) "extra"
string(2) "v2"
string(2) "v1"
string(2) "v0"

test_shift_string: Teds\Deque:
object(Teds\Deque)#1 (4) {
  [0]=>
  string(5) "extra"
  [1]=>
  string(2) "v2"
  [2]=>
  string(2) "v1"
  [3]=>
  string(2) "v0"
}
string(5) "extra"
string(2) "v2"
string(2) "v1"
string(2) "v0"

test_shift_int: Teds\Vector:
object(Teds\Vector)#1 (3) {
  [0]=>
  int(11)
  [1]=>
  int(10)
  [2]=>
  int(100)
}
int(11)
int(10)
int(100)

test_shift_int: Teds\LowMemoryVector:
object(Teds\LowMemoryVector)#1 (0) {
}
int(11)
int(10)
int(100)

test_shift_int: Teds\Deque:
object(Teds\Deque)#1 (3) {
  [0]=>
  int(11)
  [1]=>
  int(10)
  [2]=>
  int(100)
}
int(11)
int(10)
int(100)

test_shift_int: Teds\IntVector:
object(Teds\IntVector)#1 (3) {
  [0]=>
  int(11)
  [1]=>
  int(10)
  [2]=>
  int(100)
}
int(11)
int(10)
int(100)