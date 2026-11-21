# drivers/

Drivers for chips *outside* the MCU: sensors, displays, EEPROMs, Ethernet PHYs and so on.

## Rule: a driver never includes the ST HAL

A driver talks to the bus through a small interface struct that the caller fills in. That gives three things:
- The same driver runs on any board, or any MCU.
- You can unit-test it on the PC with a fake bus.
- A client can drop it into their project.

```
drivers/bme280/
├── CMakeLists.txt
├── include/bme280.h      public API + the bus interface
└── src/bme280.c          register logic only - no HAL calls
tests/test_bme280.c       uses a fake bus that records/returns bytes
```

```c
/* include/bme280.h */
typedef struct
{
    /* return 0 on success; provided by the board or app */
    int (*write)(void *ctx, uint8_t reg, const uint8_t *data, size_t len);
    int (*read)(void *ctx, uint8_t reg, uint8_t *data, size_t len);
    void *ctx; /* e.g. the I2C handle + 7-bit address */
} bme280_bus_t;

int bme280_init(bme280_t *dev, const bme280_bus_t *bus);
int bme280_read_temperature(bme280_t *dev, int32_t *centi_celsius);
```

```cmake
# CMakeLists.txt
add_library(bme280 STATIC src/bme280.c)
target_include_directories(bme280 PUBLIC include)
target_link_libraries(bme280 PRIVATE lab_mcu lab_warnings)
```

Then add `add_subdirectory(bme280)` to `drivers/CMakeLists.txt`, and add a `lab_add_test(...)` line in `tests/CMakeLists.txt`.

This is roadmap step 3: "I2C/SPI sensor driver with unit tests". It's the most common freelance gig.
