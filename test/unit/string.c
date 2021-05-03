#include "util/string.h"
#include "util/util.h"

#include "test/unit_test.h"


// Creation and deletion.


static void test_string_make()
{
    struct string x;
    string_make(&x, 32);
    CHECK(x.size == 32);
    CHECK(x.data != NULL);
    string_free(&x);
    CHECK(x.size == 0);
}


static void test_string_copy()
{
    struct string to_copy = string_literal("abc");
    struct string copy;
    string_copy(&copy, &to_copy);
    CHECK(copy.size == 3);
    CHECK(copy.data != NULL);
    CHECK(copy.data != to_copy.data);
    CHECK(string_is_equal(&copy, &to_copy));
    string_free(&copy);
}


static void test_string_copy_cstr()
{
    char const cstr[] = "abc";
    struct string copy;
    string_copy_cstr(&copy, cstr);
    CHECK(copy.size == 3);
    CHECK(copy.data != NULL);
    CHECK(copy.data != cstr);
    CHECK(string_is_equal_cstr(&copy, cstr));
    string_free(&copy);
}


// Comparing.


static void test_string_is_same()
{
    struct string one = {NULL, 0};
    struct string two = {NULL, 0};
    CHECK(string_is_same(&one, &two) == true);

    char cstr[] = "abc";
    one = (struct string){cstr, 3};
    CHECK(string_is_same(&one, &two) == false);
    two = (struct string){cstr, 3};
    CHECK(string_is_same(&one, &two) == true);

    char cstr_copy[] = "abc";
    one = (struct string){cstr_copy, 3};
    CHECK(string_is_same(&one, &two) == false);
}


static void test_string_is_equal()
{
    struct string one = {"abc", 3};
    struct string two = {"abc", 3};
    CHECK(string_is_equal(&one, &two) == true);
    CHECK(string_is_equal_cstr(&one, "abc") == true);

    one.size = 2;
    CHECK(string_is_equal(&one, &two) == false);
    CHECK(string_is_equal_cstr(&one, "abc") == false);
    one.size = 3;

    one.data = "def";
    CHECK(string_is_equal(&one, &two) == false);
    CHECK(string_is_equal_cstr(&one, "abc") == false);
    CHECK(string_is_equal_cstr(&one, "def") == true);
}


static void test_string_starts_with()
{
    struct string one = {"abc", 3};
    CHECK(string_starts_with_cstr(&one, "") == true);
    CHECK(string_starts_with_cstr(&one, "a") == true);
    CHECK(string_starts_with_cstr(&one, "ab") == true);
    CHECK(string_starts_with_cstr(&one, "abc") == true);
    CHECK(string_starts_with_cstr(&one, "abcd") == false);
    CHECK(string_starts_with_cstr(&one, "b") == false);
}


static void test_string_end_with()
{
    struct string one = {"abc", 3};
    CHECK(string_end_with_cstr(&one, "") == true);
    CHECK(string_end_with_cstr(&one, "c") == true);
    CHECK(string_end_with_cstr(&one, "bc") == true);
    CHECK(string_end_with_cstr(&one, "abc") == true);
    CHECK(string_end_with_cstr(&one, "abcd") == false);
    CHECK(string_end_with_cstr(&one, "b") == false);
}


// Joining.


static void test_string_join_array()
{
    struct string array[] = {{"abc", 3}, {"def", 3}};
    struct string out;
    string_join_array(&out, array, ARRAY_SIZE(array));
    CHECK(string_is_equal_cstr(&out, "abcdef"));
    string_free(&out);
}


static void test_string_join()
{
    struct string one = {"abc", 3};
    struct string two = {"def", 3};
    struct string out;
    string_join(&out, one, two);
    CHECK(string_is_equal_cstr(&out, "abcdef"));
    string_free(&out);
}


// Slicing.


static void test_string_copy_slice()
{
    struct string one = {"abc", 3};
    struct string out;
    string_copy_slice(&out, &one, 0, 1);
    CHECK(string_is_equal_cstr(&out, "a"));
    string_free(&out);
}


static void test_string_view_slice()
{
    struct string one = {"abc", 3};
    struct string out;
    string_view_slice(&out, &one, 0, 1);
    CHECK(string_is_equal_cstr(&out, "a"));
}


// Splitting.


static void test_string_copy_split()
{
    struct string input = {"abc", 3};
    struct string * out = NULL;
    CHECK(string_copy_split(&out, &input, 'b') == 2);
    CHECK(string_is_equal_cstr(out + 0, "a"));
    CHECK(string_is_equal_cstr(out + 1, "c"));
    // The copies should refer to different memory.
    CHECK(out[0].data != input.data + 0);
    CHECK(out[1].data != input.data + 2);
    free(out);
}


static void test_string_view_split()
{
    {
        struct string input = {"abc", 3};
        struct string * out = NULL;
        CHECK(string_view_split(&out, &input, 'b') == 2);
        // The views should refer to the original memory.
        CHECK(out[0].data == input.data + 0);
        CHECK(out[1].data == input.data + 2);
        // And the views should both have size 1.
        CHECK(out[0].size == 1);
        CHECK(out[1].size == 1);
        free(out);
    }

    {
        // Splitting about a character that doesn't exist in the input string
        // should result in an allocated view of the original string.
        struct string input = {"abc", 3};
        struct string * out = NULL;
        CHECK(string_view_split(&out, &input, 'd') == 1);
        CHECK(out[0].data == input.data);
        CHECK(out[0].size == 3);
        free(out);
    }
}


int main()
{
    test_string_make();
    test_string_copy();
    test_string_copy_cstr();
    test_string_is_same();
    test_string_is_equal();
    test_string_starts_with();
    test_string_end_with();
    test_string_join();
    test_string_join_array();
    test_string_copy_slice();
    test_string_view_slice();
    test_string_copy_split();
    test_string_view_split();
    success("All tests passed :-)");
    return 0;
}
