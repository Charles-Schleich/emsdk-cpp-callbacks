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

  extern void call_js_callback(int, char *, int);
  extern void remove_js_callback(void *);
  extern void test_call_js_callback();

  // CONTINUE FROM HERE
  // CAN I USE A FUNCTION DEFINED IN cpp-wasm.cpp to callback to a js function ?
  // EMSCRIPTEN_KEEPALIVE
  // EM_JS(void, test_async_function, (), {
  //   Asyncify.handleAsync(async() = > {
  //     console.log("Test Async Function");
  //   });
  // });

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
        console.log('    JS - inside C++ EM_ASM Function!');
    );

    // Get Thread id
    std::cout << std::this_thread::get_id(); // Same as pthread_self()
    std::cout << std::endl;

    return;
  }
  
  // cb : Async Function from JS
  // cb : is a js object, ripe for fuckery 
  int cbTest(emscripten::val cb)
  {
    // Vals generated from C++ Land
    printf("!!! cbTest \n");
    int ret = cb(5).await().as<int>();
    printf("!!! ret %d \n",ret);
    return ret;
  }

  EMSCRIPTEN_BINDINGS(my_module) {
      emscripten::function("cbTest", &cbTest);
  }


// TODO : Investigate
//  https://emscripten.org/docs/api_reference/val.h.html#_CPPv4NK10emscripten10emscripten3valcv8co_awaitEv
  EMSCRIPTEN_KEEPALIVE
  int fn_call_cpp_callback_js(int fp)
  {
    printf("------ fn_call_cpp_callback_js ------\n");
    printf("calling %d \n",fp);

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


  EMSCRIPTEN_KEEPALIVE
  void *call_js_function(void *js_callback_id)
  {
    int js_callback = (int)js_callback_id;
    // Sleep Function
    sleep(3);
    printf("JS callback ID is: %d\n", js_callback);
    call_js_callback(js_callback, "Hello from C", 12);
  }

  EMSCRIPTEN_KEEPALIVE void
  call_js_function_on_another_thread(int js_callback_id)
  {

    pthread_t cb_thr;
    pthread_create(&cb_thr, 0, &call_js_function, (void *)js_callback_id);
  }

  EMSCRIPTEN_KEEPALIVE
  void *test_callback_js(int js_callback_id)
  {
    test_call_js_callback();
  }
}