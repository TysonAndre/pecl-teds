--TEST--
Teds\SortedStrictMap gc
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
    $it = new Teds\SortedStrictMap();
    $it->offsetSet($it, "v$i");
    $it->offsetSet($key, "vstr$i");
    $it->offsetSet(new HasDestructor(), new stdClass());
    $it->offsetUnset($key);
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