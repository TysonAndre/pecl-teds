--TEST--
binary_search test
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
        echo "In compare with side effects\n";
        $a[0] = null;  // Change reference target to null.
        var_dump($a);
        // Binary search runs on the array with copy-on-write, so there is another comparison
        unset($a[1]);
        $a[1] = 'not used';
        echo "Value:\n";
        var_dump($value);
        echo "Target:\n";
        var_dump($target);
        $value = new stdClass();
        $target = new stdClass();

        return -1;
    });
    echo "Result: " . json_encode($result) . "\n";
    var_dump($a);
}
test1();
--EXPECT--
In test1
In compare with side effects
array(2) {
  [0]=>
  &NULL
  [1]=>
  &NULL
}
Value:
object(HasDestructor)#1 (0) {
}
Target:
string(11) "v_unrelated"
In __destruct
In compare with side effects
array(2) {
  [0]=>
  &NULL
  [1]=>
  string(8) "not used"
}
Value:
NULL
Target:
string(11) "v_unrelated"
Result: {"found":false,"key":null,"value":null}
array(2) {
  [0]=>
  NULL
  [1]=>
  string(8) "not used"
}