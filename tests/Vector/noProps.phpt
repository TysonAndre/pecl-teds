--TEST--
Teds\Vector get_mangled_object_vars should be empty in php 8.3+
--SKIPIF--
<?php if (PHP_VERSION_ID < 80300) echo "skip php 8.3+ only\n"; ?>
--FILE--
<?php
$it = new Teds\Vector([new stdClass()]);
var_export($it);
echo "\n", json_encode($it), "\n";
// The real storage of the data is in the C representation.
// Redundant representations are deliberately avoided because they'll waste memory.
// In php 8.2 and older, fields only got added to the properties hash table as a workaround
// to get both debug output and infinite recursion detection on var_export/debug_zval_dump to work.
var_dump(get_mangled_object_vars($it));
?>
--EXPECTF--
\Teds\Vector::__set_state(array(
   0 =>%S
  (object) array(
  ),
))
[{}]
array(0) {
}
