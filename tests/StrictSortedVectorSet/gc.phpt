--TEST--
Teds\StrictSortedVectorSet gc
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In " . __METHOD__ . "\n";
    }
}
// discards keys
for ($i = 0; $i < 3; $i++) {
    echo "Iteration $i\n";
    $key = "k$i";
    $it = new Teds\StrictSortedVectorSet();
    $it->add($it);
    $it->add($key);
    $it->add(new HasDestructor());
    $it->remove($key);
    unset($it);
    gc_collect_cycles();
}
?>
--EXPECT--
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct