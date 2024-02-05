#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

  // TODO: REMOVE
  EMSCRIPTEN_KEEPALIVE
  void test_call(char *pointer, int length, emscripten::val callback)
  {
    printf("------ test_call ------\n");
    // printf("      C test_call Ptr     %p  \n", pointer);
    // printf("      C test_call Ptr num %d \n", pointer);
    // printf("      C test_call Len %d  \n", length);

    int length2 = sizeof(length) / sizeof(char);

    int loop;
    for (loop = 0; loop < length2; loop++)
    {
      printf("C loop: %d : %d \n", loop, pointer[loop]);
    }
    printf("------ END test_call ------\n");

    printf("Call callback().await()\n");
    emscripten::val return_val = callback().await();
    printf("------ callback().await() ------\n");

    return;
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