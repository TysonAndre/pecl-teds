--TEST--
Teds\StrictMap constructed from Traversable throwing
--FILE--
<?php

class HasDestructor {
    public function __construct(public string $value) {}
    public function __destruct() {
        echo "in " . __METHOD__ . " $this->value\n";
    }
}

function yields_and_throws() {
    yield 123 => new HasDestructor('in value1');
    yield new HasDestructor('in key') => 123;
    yield 123 => new HasDestructor('in value2'); // replaces value1
    yield 'first' => 'second';

    throw new RuntimeException('test');

    echo "Unreachable\n";
}
try {
    $it = new Teds\StrictMap(yields_and_throws());
} catch (RuntimeException $e) {
    echo "Caught " . $e->getMessage() . "\n";
}
gc_collect_cycles();
echo "Done\n";
?>
--EXPECT--
in HasDestructor::__destruct in value1
in HasDestructor::__destruct in value2
in HasDestructor::__destruct in key
Caught test
Done
