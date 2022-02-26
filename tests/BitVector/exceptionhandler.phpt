--TEST--
Teds\BitVector constructed from Traversable throwing
--FILE--
<?php

class HasDestructor {
    public function __construct(public string $value) {}
    public function __destruct() {
        echo "in " . __METHOD__ . " $this->value\n";
    }
}

function yields_and_throws() {
    yield new HasDestructor('in key') => true;
    yield 'first' => new HasDestructor('in invalid value');  // throws TypeError
    echo "Not reached\n";
}
try {
    $it = new Teds\BitVector(yields_and_throws());
} catch (TypeError $e) {
    echo "Caught " . $e->getMessage() . "\n";
}
gc_collect_cycles();
echo "Done\n";
?>
--EXPECT--
in HasDestructor::__destruct in key
Caught Illegal Teds\BitVector value type HasDestructor
Done
in HasDestructor::__destruct in invalid value