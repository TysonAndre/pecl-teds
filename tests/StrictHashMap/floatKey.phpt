--TEST--
Teds\StrictHashMap floating point edge cases
--FILE--
<?php

function create_value(string $x) { return "value_$x"; }

$it = new Teds\StrictHashMap([]);
$it[0.0] = create_value('a');
$it[-0.0] = create_value('override -0.0 === 0.0');
echo "Add array of signed zero\n";
$it[[0.0]] = create_value('a');
$it[[-0.0]] = create_value('override [-0.0] === [0.0]');
$it[0] = create_value('different_int');
echo "Add nan\n";
$it[NAN] = create_value('nan');
$zero = 0;
$it[fdiv($zero, $zero)] = create_value('nan_override');

$it[[NAN]] = create_value('nan_array');
$it[[fdiv($zero, $zero)]] = create_value('nan_array_override');
var_dump($it);
?>
--EXPECT--
Add array of signed zero
Add nan
object(Teds\StrictHashMap)#1 (5) {
  [0]=>
  array(2) {
    [0]=>
    float(0)
    [1]=>
    string(27) "value_override -0.0 === 0.0"
  }
  [1]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      float(0)
    }
    [1]=>
    string(31) "value_override [-0.0] === [0.0]"
  }
  [2]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    string(19) "value_different_int"
  }
  [3]=>
  array(2) {
    [0]=>
    float(NAN)
    [1]=>
    string(18) "value_nan_override"
  }
  [4]=>
  array(2) {
    [0]=>
    array(1) {
      [0]=>
      float(NAN)
    }
    [1]=>
    string(24) "value_nan_array_override"
  }
}