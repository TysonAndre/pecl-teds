--TEST--
Teds\ImmutableSequence map()
--FILE--
<?php
use Teds\ImmutableSequence;
function dump_vector(ImmutableSequence $v) {
    printf("count=%d\n", $v->count());
    foreach ($v as $i => $value) {
        printf("Value #%d: %s\n", $i, json_encode($value));
    }
}
$create_array = fn($x) => [$x];
dump_vector((new ImmutableSequence([]))->map($create_array));
dump_vector((new ImmutableSequence([strtoupper('test'), true, false, new stdClass(), []]))->map($create_array));
$identity = fn($x) => $x;
dump_vector((new ImmutableSequence([]))->map($identity));
dump_vector((new ImmutableSequence([strtoupper('test'), true, false, new stdClass(), []]))->map($identity));
$false = fn() => false;
dump_vector((new ImmutableSequence([strtoupper('test'), true, false, new stdClass(), []]))->map($false));
?>
--EXPECT--
count=0
count=5
Value #0: ["TEST"]
Value #1: [true]
Value #2: [false]
Value #3: [{}]
Value #4: [[]]
count=0
count=5
Value #0: "TEST"
Value #1: true
Value #2: false
Value #3: {}
Value #4: []
count=5
Value #0: false
Value #1: false
Value #2: false
Value #3: false
Value #4: false