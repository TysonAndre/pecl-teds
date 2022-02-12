--TEST--
Teds\BitSet indexOf/contains
--FILE--
<?php
// discards keys
$it = new Teds\BitSet();
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 1-element\n";
$it = new Teds\BitSet([true]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 1-element false\n";
$it = new Teds\BitSet([false]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
echo "Build 4-element\n";
$it = new Teds\BitSet([true, true, false, false]);
var_dump($it->indexOf(false));
var_dump($it->indexOf(true));
var_dump($it->contains(false));
var_dump($it->contains(true));
?>
--EXPECT--
NULL
NULL
bool(false)
bool(false)
Build 1-element
int(0)
NULL
bool(true)
bool(false)
Build 1-element false
NULL
int(0)
bool(false)
bool(true)
Build 4-element
int(0)
int(2)
bool(true)
bool(true)