#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>


TEST_CASE( "MyTest", "[multiplication]" ) {
    CHECK( 4 == (2*2) );
}

TEST_CASE( "MyTest", "[division]" ) {
    CHECK( 2 == (4/2) );
}


int main(int argc, char **argv)
{
    return Catch::Session().run( argc, argv );
}
