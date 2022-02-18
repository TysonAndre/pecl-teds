--TEST--
Teds\StrictHashMap gc
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In " . __METHOD__ . "\n";
    }
}
function test_map_gc(string $class_name) {
    echo "Test $class_name\n";
    // discards keys
    for ($i = 0; $i < 3; $i++) {
        echo "Iteration $i\n";
        $key = "k$i";
        $it = new $class_name();
        $it->offsetSet($it, "v$i");
        $it->offsetSet($key, "vstr$i");
        $it->offsetSet(new HasDestructor(), new stdClass());
        $it->offsetUnset($key);
        if ($i === 1) {
            $it->clear();
        }
        unset($it);
        gc_collect_cycles();
    }
}
foreach ([
    Teds\StrictTreeMap::class,
    Teds\StrictHashMap::class,
    Teds\StrictSortedVectorMap::class,
] as $class_name) {
    test_map_gc($class_name);
}
?>
--EXPECT--
Test Teds\StrictTreeMap
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct
Test Teds\StrictHashMap
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct
Test Teds\StrictSortedVectorMap
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct