--TEST--
Test reflection information for php 8.1+ only classes
--SKIPIF--
<?php if (PHP_VERSION_ID < 80100) echo "skip php 8.1+ only\n"; ?>
--FILE--
<?php
$x = new ReflectionExtension('teds');
echo "Classes:\n";
$classes = $x->getClasses();
uksort($classes, 'strcmp');
foreach ($classes as $name => $info) {
    if (!$info->isEnum()) {
        continue;
    }
    foreach ([
        ReflectionClass::IS_IMPLICIT_ABSTRACT => 'IS_IMPLICIT_ABSTRACT',
        ReflectionClass::IS_EXPLICIT_ABSTRACT => 'abstract',
        ReflectionClass::IS_FINAL => 'final',
    ] as $modifier => $modifier_name) {
        if ($info->getModifiers() & $modifier) {
            echo "$modifier_name ";
        }
    }
    echo "enum ";
    echo $name;
    $parent = $info->getParentClass();
    if ($parent) {
        echo ' extends ' . $parent->name;
    }

    $interfaces = $info->getInterfaceNames();
    usort($interfaces, 'strcmp');
    if ($interfaces) {
        echo ($info->isInterface() ? ' extends ' : ' implements ') . implode(', ', $interfaces);
    }
    $modifiers = [];
    if ($name !== $info->name) {
        echo " (alias of $info->name)";
    }

    echo "\n";
}
?>
--EXPECT--
Classes:
final enum Teds\EmptyMap implements ArrayAccess, Countable, Iterator, JsonSerializable, Teds\Collection, Teds\Map, Traversable, UnitEnum
final enum Teds\EmptySequence implements ArrayAccess, Countable, Iterator, JsonSerializable, Teds\Collection, Teds\Sequence, Traversable, UnitEnum
final enum Teds\EmptySet implements Countable, Iterator, JsonSerializable, Teds\Collection, Teds\Set, Traversable, UnitEnum