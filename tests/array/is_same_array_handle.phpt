--TEST--
Test is_same_array_handle implementation
--FILE--
<?php

use function Teds\is_same_array_handle;

function create_array(int $value) {
    return [$value => $value];
}
function test_process_recursive_array(array $data, array $visited = []) {
    $depth = count($visited);
    foreach ($visited as $i => $already_processed) {
        if (is_same_array_handle($data, $already_processed)) {
            echo "Skip depth=$depth value already seen at previous depth=$i\n";
            return;
        }
    }
    $visited[] = $data;
    printf("Start check depth=%d\n", $depth);
    foreach ($data as $key => $inner) {
        echo "Check key=$key\n";
        if (is_array($inner)) {
            test_process_recursive_array($inner, $visited);
        } else {
            echo "Saw non-array: "; var_dump($inner);
        }

    }

    printf("End check depth=%d\n", $depth);
}
call_user_func(function () {
    $x = create_array(123);
    $y = create_array(123);
    $z = $x;
    var_dump(is_same_array_handle($x, $y)); // false
    var_dump(is_same_array_handle($x, $x)); // true
    var_dump(is_same_array_handle($x, $z)); // true
    $x[] = 456;
    var_dump(is_same_array_handle($x, $z)); // false
    $recursive = [];
    $recursive['inner'] = ['other', &$recursive];
    test_process_recursive_array($recursive);
    var_dump($recursive);
});
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
Start check depth=0
Check key=inner
Start check depth=1
Check key=0
Saw non-array: string(5) "other"
Check key=1
Skip depth=2 value already seen at previous depth=0
End check depth=1
End check depth=0
array(1) {
  ["inner"]=>
  array(2) {
    [0]=>
    string(5) "other"
    [1]=>
    *RECURSION*
  }
}