--TEST--
Teds\IntVector isset
--FILE--
<?php
$it = new Teds\IntVector([0, 1, 987]);
foreach ([0, 1, 2, 3, -1, '1', PHP_INT_MIN, PHP_INT_MAX, 1.0, false, true] as $offset) {
    printf("offset=%s isset=%s empty=%s value=%s\n", json_encode($offset), json_encode(isset($it[$offset])), json_encode(empty($it[$offset])), json_encode($it[$offset] ?? null));
}
?>
--EXPECTF--
offset=0 isset=true empty=true value=0
offset=1 isset=true empty=false value=1
offset=2 isset=true empty=false value=987
offset=3 isset=false empty=true value=null
offset=-1 isset=false empty=true value=null
offset="1" isset=true empty=false value=1
offset=-%d isset=false empty=true value=null
offset=%d isset=false empty=true value=null
offset=1 isset=true empty=false value=1
offset=false isset=true empty=true value=0
offset=true isset=true empty=false value=1