--TEST--
Teds\Vector isset
--FILE--
<?php
$it = new Teds\Vector([null, false, new stdClass()]);
foreach ([0, 1, 2, 3, -1, '1', PHP_INT_MIN, PHP_INT_MAX, 1.0, false, true] as $offset) {
    printf("offset=%s isset=%s empty=%s value=%s\n", json_encode($offset), json_encode(isset($it[$offset])), json_encode(empty($it[$offset])), json_encode($it[$offset] ?? null));
}
?>
--EXPECTF--
offset=0 isset=false empty=true value=null
offset=1 isset=true empty=true value=false
offset=2 isset=true empty=false value={}
offset=3 isset=false empty=true value=null
offset=-1 isset=false empty=true value=null
offset="1" isset=true empty=true value=false
offset=-%d isset=false empty=true value=null
offset=%d isset=false empty=true value=null
offset=1 isset=true empty=true value=false
offset=false isset=false empty=true value=null
offset=true isset=true empty=true value=false
