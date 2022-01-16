--TEST--
Teds\StrictMap constructed from array
--FILE--
<?php

$it = new Teds\StrictMap();
$it[[]] = new stdClass();
$it[[]] = new stdClass();
$it[0] = 123;
$it['0'] = 'extra';
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}
var_dump($it);
echo "After removing key\n";
unset($it[0]);
printf("count=%d\n", count($it));
var_dump($it);
foreach ($it as $key => $value) {
    printf("Key: %s\nValue: %s\n", var_export($key, true), var_export($value, true));
}

?>
--EXPECT--
Key: array (
)
Value: (object) array(
)
Key: 0
Value: 123
Key: '0'
Value: 'extra'
object(Teds\StrictMap)#1 (3) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
  [1]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(123)
  }
  [2]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(5) "extra"
  }
}
After removing key
count=2
object(Teds\StrictMap)#1 (2) {
  [0]=>
  array(2) {
    [0]=>
    array(0) {
    }
    [1]=>
    object(stdClass)#3 (0) {
    }
  }
  [1]=>
  array(2) {
    [0]=>
    string(1) "0"
    [1]=>
    string(5) "extra"
  }
}
Key: array (
)
Value: (object) array(
)
Key: '0'
Value: 'extra'