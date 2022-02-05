--TEST--
Teds\LowMemoryVector json encoding recursive data
--FILE--
<?php
// discards keys
function test() {
    $vec = new Teds\LowMemoryVector();
    $vec[] = $vec;
    echo "JSON: " . json_encode($vec, JSON_PARTIAL_OUTPUT_ON_ERROR), "\n";
    foreach ($vec as $key => $value) {
        echo "Saw key $key\n";
        var_dump($value);
    }
    var_dump((array)$vec);
    // Harmless but warns in debug builds. See https://github.com/php/php-src/issues/8044
    // var_export($vec); echo "\n";
    // debug_zval_dump($vec); echo "\n";
    $result = serialize($vec);
    echo "$result\n";
    var_dump($vec->pop());
    var_dump((array)$vec);
    echo "JSON: " . json_encode($vec), "\n";
    var_export($vec); echo "\n";
}
test();
?>
--EXPECTF--
JSON: [null]
Saw key 0
object(Teds\LowMemoryVector)#1 (1) {
  [0]=>
  *RECURSION*
}
array(1) {
  [0]=>
  object(Teds\LowMemoryVector)#1 (1) {
    [0]=>
    *RECURSION*
  }
}

Warning: Teds\LowMemoryVector::__serialize(): LowMemoryVector::__serialize is a work in progress and the serialization format will change. Unserialization is not implemented in %s on line 15
O:20:"Teds\LowMemoryVector":2:{i:0;i:5;i:1;a:1:{i:0;r:1;}}
object(Teds\LowMemoryVector)#1 (0) {
}
array(0) {
}
JSON: []
Teds\LowMemoryVector::__set_state(array(
))