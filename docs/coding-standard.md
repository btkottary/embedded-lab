# Coding standard

Based on the free **Barr Group Embedded C Coding Standard (BARR-C:2018)**, with MISRA-C habits where they're cheap.
Formatting is enforced by `.clang-format`; CI fails if code isn't formatted.

## Formatting (automatic)
- 4-space indent, no tabs, 100 columns, braces on their own line, `extern "C" {` stays on one line.
- Format a file: `clang-format -i path/file.c`. Never format `third_party/`.

## Naming
| Thing | Style | Example |
|---|---|---|
| Functions | `module_verb_noun` | `ringbuf_put`, `board_led_set` |
| Variables, parameters | `snake_case` | `byte_count` |
| File-static variables | `s_` prefix | `s_leds` |
| Macros, constants | `UPPER_CASE` | `STEP_MS`, `LED_COUNT` |
| Types | `_t` suffix | `ringbuf_t`, `led_t` |

## Rules
1. Every public function is declared in the module header, with a Doxygen comment.
2. `static` everything that isn't part of the public API.
3. Use fixed-width integer types (`uint8_t`, `uint32_t`) for registers, protocols and sizes on the wire.
4. Unsigned literals get a `U` suffix (`100U`). Avoid mixing signed and unsigned.
5. Always use braces, even for one-line `if`/`for` bodies. Parenthesise each part of a compound condition.
6. No `malloc`/`free` in firmware, and no recursion.
7. Check every HAL/driver return value. Call `board_panic()` only when nothing sensible can be done.
8. ISRs are short: set a flag or push to a ring buffer, and do the work in the main loop.
9. Use `volatile` only for memory shared between ISR and main, or for hardware registers.
10. No magic numbers. Name them, and cite the datasheet/RM section for hardware values.
11. Headers have include guards, include only what they need, and never define storage.
12. Builds are warning-free: `lab_warnings` turns on `-Wall -Wextra -Werror` and more for our code.

## Tests
- Hardware-independent code (`libs/`, `drivers/`) gets Unity tests in `tests/`.
- Test names describe behaviour: `test_full_buffer_rejects_put`.
- A bug fix comes with a test that fails without the fix.
