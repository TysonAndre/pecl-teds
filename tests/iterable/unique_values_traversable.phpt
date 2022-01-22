--TEST--
Test unique_values() function on Traversables
--FILE--
<?php

use function Teds\unique_values;

class ThrowsInDestructor {
    public function __destruct() {
        throw new RuntimeException('In ThrowsInDestructor');
    }
}

function yield_values(bool $throw) {
    yield create_key(1) => create_key(2);
    yield 1 => 2;
    yield create_key(1) => create_key(2);
    if ($throw) {
        yield new ThrowsInDestructor(1) => create_key(3);
    }
    yield create_key(4) => create_key(5);
}

function dump_unique_values(iterable $input) {
    echo "For " . json_encode($input) . ": ";
    var_dump(unique_values($input));
}
/* Create a reference counted key */
function create_key(int $x) {
    return "k$x";
}
dump_unique_values(new ArrayObject());
dump_unique_values(new ArrayObject([true]));
dump_unique_values(new ArrayObject([create_key(1)]));
dump_unique_values(new ArrayObject([create_key(1), create_key(2), create_key(1)]));
dump_unique_values(yield_values(false));
try {
    dump_unique_values(yield_values(true));
} catch (Exception $e) {
    echo "Caught: {$e->getMessage()}\n";
}
?>
--EXPECT--
For {}: array(0) {
}
For {"0":true}: array(1) {
  [0]=>
  bool(true)
}
For {"0":"k1"}: array(1) {
  [0]=>
  string(2) "k1"
}
For {"0":"k1","1":"k2","2":"k1"}: array(2) {
  [0]=>
  string(2) "k1"
  [1]=>
  string(2) "k2"
}
For {}: array(3) {
  [0]=>
  string(2) "k2"
  [1]=>
  int(2)
  [2]=>
  string(2) "k5"
}
For {}: Caught: In ThrowsInDestructor
