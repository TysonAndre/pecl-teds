--TEST--
binary_search test edge case
--FILE--
<?php
class HasDestructor {
    public function __destruct() {
        echo "In __destruct\n";
    }
}
function create_key(string $x) {
    return "v$x";
}
function test1() {
    echo "In test1\n";
    $i = 0;
    $a = [new HasDestructor()];
    $a[1] = &$a[0];
    $result = Teds\binary_search($a, create_key('_unrelated'), function ($target, $value) use (&$a) {
        echo "In compare with side effects replacing original reference\n";
        $a[0] = create_key('_newval');  // Change reference target to a string.
        var_dump($a);

        return 0;
    });
    // If there are references in the array and those references are changed during the binary search,
    // this will not crash, but the result will return the latest value of the original reference.
    echo "Result: ";
    var_dump($result);
    echo "Changing original array does not change binary_search result\n";
    $a[0] = 'other';
    var_dump($result);
}
test1();
?>
--EXPECT--
In test1
In compare with side effects replacing original reference
array(2) {
  [0]=>
  &string(8) "v_newval"
  [1]=>
  &string(8) "v_newval"
}
In __destruct
Result: array(3) {
  ["found"]=>
  bool(true)
  ["key"]=>
  int(1)
  ["value"]=>
  string(8) "v_newval"
}
Changing original array does not change binary_search result
array(3) {
  ["found"]=>
  bool(true)
  ["key"]=>
  int(1)
  ["value"]=>
  string(8) "v_newval"
}