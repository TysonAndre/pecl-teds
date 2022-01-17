--TEST--
Teds\SortedStrictSet is an IteratorAggregate
--FILE--
<?php

$it = new Teds\SortedStrictSet(['discarded_first' => 'x', 'discardedsecond' => (object)['key' => 'value']]);
foreach ($it as $k1 => $v1) {
    foreach ($it as $k2 => $v2) {
        if ($k1 !== $v1 || $k2 !== $v2) {
            echo "should match\n";
        }
        printf("v1=%s v2=%s\n", json_encode($k1), json_encode($k2), json_encode($v1), json_encode($v2));
    }
}
?>
--EXPECT--
v1="x" v2="x"
v1="x" v2={"key":"value"}
v1={"key":"value"} v2="x"
v1={"key":"value"} v2={"key":"value"}