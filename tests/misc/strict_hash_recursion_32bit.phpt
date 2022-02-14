--TEST--
Test strict_hash() function edge cases
--SKIPIF--
<?php if (PHP_INT_SIZE >= 8) echo "skip 32-bit only\n"; ?>
--FILE--
<?php

class Dumper {
    public function __debugInfo() {
        return ['hash(a)' => Teds\strict_hash($GLOBALS['a'])];
    }
}
$a = [new Dumper()];
echo "hash(a)=" . Teds\strict_hash($a), "\n";
var_dump($a);
echo "hash(a)=" . Teds\strict_hash($a), "\n";
?>
--EXPECT--
hash(a)=180824612
array(1) {
  [0]=>
  object(Dumper)#1 (1) {
    ["hash(a)"]=>
    int(180824612)
  }
}
hash(a)=180824612
