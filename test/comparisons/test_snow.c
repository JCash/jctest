#include <snow/snow.h>

describe(operations) {
    it("multiplication") {
        asserteq_int(4, 2*2);
    }

    it("division") {
        asserteq_int(2, 4/2);
    }
}

snow_main();
