--TEST--
Teds\SortedStrictSet unserialize sorted
--FILE--
<?php

call_user_func(function () {
    $values = [];
    for ($i = 0; $i < 8; $i++) {
        $values[] = "v$i";
    }
    $it = new Teds\SortedStrictSet($values);
    $ser = serialize($it);
    echo "$ser\n";
    echo json_encode(unserialize($ser)), "\n";
    echo json_encode(unserialize(str_replace('v1', 'v0', $ser))), "\n";
    $data = 'O:20:"Teds\SortedStrictSet":8:{i:0;s:2:"v7";i:1;s:2:"v0";i:2;s:2:"v6";i:3;s:2:"v3";i:4;s:2:"v2";i:5;s:2:"v5";i:6;s:2:"v1";i:7;s:2:"v4";}';
    echo json_encode(unserialize($data)), "\n";
});
?>
--EXPECT--
O:20:"Teds\SortedStrictSet":8:{i:0;s:2:"v0";i:1;s:2:"v1";i:2;s:2:"v2";i:3;s:2:"v3";i:4;s:2:"v4";i:5;s:2:"v5";i:6;s:2:"v6";i:7;s:2:"v7";}
["v0","v1","v2","v3","v4","v5","v6","v7"]
["v0","v2","v3","v4","v5","v6","v7"]
["v0","v1","v2","v3","v4","v5","v6","v7"]
