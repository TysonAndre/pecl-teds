--TEST--
Test strict_equals() function
--FILE--
<?php

function dump_strict_equals(mixed $a, mixed $b) {
    printf("Teds\strict_equals(%s, %s): %s\n", var_export($a, true), var_export($b, true), var_export(Teds\strict_equals($a, $b), true));
}
$zero = 0.0;
$values = [
    null,
    false,
    true,
    -1,
    0,
    0.0,
    -0.0,  // Deliberately return the same hash as 0.0, because php has 0.0 === -0.0 and it's equivalent in *almost* all ways, apart from 1 / -0.0
    'str',
    new stdClass(),
    function () {},
    STDIN,
    INF,
    -INF,
    NAN,
    fdiv($zero, $zero),
    [NAN],
];

foreach ($values as $value) {
    dump_strict_equals($value, $value);
}
$aNAN = [];
$nan = NAN;
$aNAN[] = &$nan;
dump_strict_equals([NAN], $aNAN);

echo "Not equals\n";
dump_strict_equals(NAN, INF);
dump_strict_equals([NAN], [INF]);

?>
--EXPECTF--
Teds\strict_equals(NULL, NULL): true
Teds\strict_equals(false, false): true
Teds\strict_equals(true, true): true
Teds\strict_equals(-1, -1): true
Teds\strict_equals(0, 0): true
Teds\strict_equals(0.0, 0.0): true
Teds\strict_equals(-0.0, -0.0): true
Teds\strict_equals('str', 'str'): true
Teds\strict_equals((object) array(
), (object) array(
)): true
Teds\strict_equals(%SClosure::__set_state(array(
)), %SClosure::__set_state(array(
))): true
Teds\strict_equals(NULL, NULL): true
Teds\strict_equals(INF, INF): true
Teds\strict_equals(-INF, -INF): true
Teds\strict_equals(NAN, NAN): true
Teds\strict_equals(NAN, NAN): true
Teds\strict_equals(array (
  0 => NAN,
), array (
  0 => NAN,
)): true
Teds\strict_equals(array (
  0 => NAN,
), array (
  0 => NAN,
)): true
Not equals
Teds\strict_equals(NAN, INF): false
Teds\strict_equals(array (
  0 => NAN,
), array (
  0 => INF,
)): false
