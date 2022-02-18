--TEST--
Teds\Set gc
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In " . __METHOD__ . "\n";
    }
}
function test_set_gc(string $class_name) {
    echo "Test $class_name\n";
    // discards keys
    for ($i = 0; $i < 3; $i++) {
        echo "Iteration $i\n";
        $key = "k$i";
        $it = new $class_name();
        $it->add($it);
        $it->add($key);
        $it->add(new HasDestructor());
        $it->remove($key);
        if ($i === 1) {
            $it->clear();
        }
        unset($it);
        gc_collect_cycles();
    }
}
foreach ([
    Teds\StrictHashSet::class,
    Teds\StrictTreeSet::class,
    Teds\StrictSortedVectorSet::class,
] as $class_name) {
    test_set_gc($class_name);
}
?>
--EXPECT--
Test Teds\StrictHashSet
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct
Test Teds\StrictTreeSet
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct
Test Teds\StrictSortedVectorSet
Iteration 0
In HasDestructor::__destruct
Iteration 1
In HasDestructor::__destruct
Iteration 2
In HasDestructor::__destruct