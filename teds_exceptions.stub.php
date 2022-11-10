<?php

/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80000
 */

namespace Teds;

/**
 * An UnsupportedOperationException indicates that a given method is not supported
 * in the given implementation (e.g. modification on an immutable datastructure).
 *
 * This allows callers of methods on interfaces to distinguish `RuntimeException`s
 * thrown by methods for unpredictable reasons from ones caused by a datastructure not supporting it.
 */
class UnsupportedOperationException extends \RuntimeException {
}
