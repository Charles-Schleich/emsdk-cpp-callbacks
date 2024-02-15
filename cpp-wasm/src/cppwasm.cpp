#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

extern "C"
{

  EMSCRIPTEN_KEEPALIVE
  void fn_no_args()
  {
    printf("------ fn_no_args ------\n");
    // Get Thread id
    std::cout << std::this_thread::get_id(); // Same as pthread_self()
    std::cout << std::endl;

    return;
  }

  EMSCRIPTEN_KEEPALIVE
  void fn_args(int a, char *pointer, int length)
  {
    printf("------ fn_args ------\n");
    // printf("------ a ------\n");
    std::cout << "The Value of 'num' is " << a;
    std::cout << std::endl;

    // Output
    int length2 = sizeof(length) / sizeof(char);

    int loop;
    for (loop = 0; loop < length2; loop++)
    {
      printf("C loop: %d : %d \n", loop, pointer[loop]);
    }
    printf("------ END test_call ------\n");
    // Get Thread id
    std::cout << std::this_thread::get_id(); // Same as pthread_self()
    std::cout << std::endl;

    return;
  }

  EM_JS(void, em_js_function, (), {
    console.log('    JS - inside C++ EM_JS Function!');
  });

  EMSCRIPTEN_KEEPALIVE
  void fn_call_js()
  {
    emscripten_run_script("console.log('    JS - inside C++, emscripten_run_script ')");

    em_js_function();

    EM_ASM(
        console.log('    JS - inside C++ EM_ASM Function!'););

    // Get Thread id
    std::cout << std::this_thread::get_id(); // Same as pthread_self()
    std::cout << std::endl;

    return;
  }

  // cb : Async Function from JS
  // cb : is a js object, ripe for fuckery
  int cb_test_async(emscripten::val cb)
  {
    // Vals generated from C++ Land
    printf("!!! cbTest \n");
    int ret = cb(5).await().as<int>();
    printf("!!! ret %d \n", ret);
    return ret;
  }

  int cb_test(emscripten::val cb)
  {
    // Vals generated from C++ Land
    printf("!!! cb_test \n");
    int ret = cb(5).as<int>();
    printf("!!! ret %d \n", ret);
    return ret;
  }

  int arrTest(std::string js_arr)
  {
    // Vals generated from C++ Land
    printf("!!! arrTest \n");
    for (unsigned char item : js_arr)
    {
      std::cout << item << std::endl;
    }
    return 10;
  }

  EMSCRIPTEN_BINDINGS(my_module)
  {
    emscripten::function("cb_test", &cb_test);
    emscripten::function("cb_test_async", &cb_test_async);
    emscripten::function("arrTest", &arrTest);
  }

  // TODO : Investigate
  //  https://emscripten.org/docs/api_reference/val.h.html#_CPPv4NK10emscripten10emscripten3valcv8co_awaitEv
  EMSCRIPTEN_KEEPALIVE
  int fn_call_cpp_callback_js(int fp)
  {
    printf("------ fn_call_cpp_callback_js ------\n");
    printf("calling %d \n", fp);

    // Get Thread id
    std::cout << std::this_thread::get_id() << std::endl; // Same as pthread_self()

    // Function returns int

    int (*f)(int) = reinterpret_cast<int (*)(int)>(fp); // Reommended by Pierre Avital
    int output = f(15);
    printf("------ FINISH fn_call_cpp_callback_js %d ------\n", output);
    return output;
  }

  EMSCRIPTEN_KEEPALIVE
  int fn_async_call_cpp_callback_js(emscripten::val fp)
  {
    return 99;
  }


}