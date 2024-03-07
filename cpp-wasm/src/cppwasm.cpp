// To expose EMSCRIPTEN_BINDINGS
#include <emscripten/bind.h>
// To Expose ProxyingQueue ???
#include <emscripten/proxying.h>
// TODO: For what ? 
#include <emscripten/eventloop.h>
// TODO: Sched for what 
#include <sched.h>
#include <string.h>
// For what ? 
#include <iostream>


// The worker threads we will use. `looper` sits in a loop, continuously
// processing work as it becomes available, while `returner` returns to the JS
// event loop each time it processes work.
std::thread looper;
std::thread returner;

// The queue used to send work to both `looper` and `returner`.
emscripten::ProxyingQueue queue;

// Whether `looper` should exit.
std::atomic<bool> should_quit{false};

// 
void looper_main() {
  while (!should_quit) {
    queue.execute();
    sched_yield();
  }
}

// 
void returner_main() {
  // Return back to the event loop while keeping the runtime alive.
  // Note that we can't use `emscripten_exit_with_live_runtime` here without
  // introducing a memory leak due to way to C++11 threads interact with
  // unwinding. See https://github.com/emscripten-core/emscripten/issues/17091.
  emscripten_runtime_keepalive_push();
}

// TODO:Continue from here
// https://github.com/emscripten-core/emscripten/blob/main/test/pthread/test_pthread_proxying_cpp.cpp
// https://emscripten.org/docs/api_reference/proxying.h.html?highlight=proxying

// TODO : Do i need these in a C interface ? 
// Can i abstract out the C++
extern "C"
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  ██████                ██████  █████  ██      ██      ██████   █████   ██████ ██   ██ ███████ 
// ██                    ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██  ██      
// ██          █████     ██      ███████ ██      ██      ██████  ███████ ██      █████   ███████ 
// ██                    ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██       ██ 
//  ██████                ██████ ██   ██ ███████ ███████ ██████  ██   ██  ██████ ██   ██ ███████ 
                                                                                              
  EMSCRIPTEN_KEEPALIVE
  void fn_no_args()
  {
    printf("------ fn_no_args ------\n");
    std::cout << "Thread ID" << std::this_thread::get_id() << std::endl; // Same as pthread_self()
    
    return;
  }

  EMSCRIPTEN_KEEPALIVE
  void fn_args(int a, char *pointer, int length)
  {
    printf("------ fn_args ------\n");
    std::cout << "The value of 'num' is " << a;
    std::cout << std::endl;

    // Output
    int length2 = sizeof(length) / sizeof(char);

    int loop;
    for (loop = 0; loop < length2; loop++)
    {
      printf("    C loop: %d : %d \n", loop, pointer[loop]);
    }
  }


//  ██████ ██████  ██████                 ██████  █████  ██      ██      ██████   █████   ██████ ██   ██ ███████ 
// ██      ██   ██ ██   ██               ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██  ██      
// ██      ██████  ██████      █████     ██      ███████ ██      ██      ██████  ███████ ██      █████   ███████ 
// ██      ██      ██                    ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██       ██ 
//  ██████ ██      ██                     ██████ ██   ██ ███████ ███████ ██████  ██   ██  ██████ ██   ██ ███████ 

  EM_JS(void, em_js_function, (), {
    console.log('    JS - inside C++ EM_JS Macro!');
  });


  EMSCRIPTEN_KEEPALIVE
  void fn_call_js()
  {
    printf("------ fn_call_js ------\n");
    
    emscripten_run_script("console.log('    JS - inside C++, emscripten_run_script ')");

    em_js_function();

    EM_ASM(
        console.log('    JS - inside C++ EM_ASM Macro!');
    );
  }

  EMSCRIPTEN_KEEPALIVE
  int fn_call_cpp_callback_js(int fp)
  {
    printf("------ fn_call_cpp_callback_js ------\n");
    printf("calling %d \n", fp);

    // Get Thread id
    std::cout << std::this_thread::get_id() << std::endl; // Same as pthread_self()

    int (*f)(int) = reinterpret_cast<int (*)(int)>(fp); // Recommended by Pierre Avital, /s
    
    int output = f(15);

    printf("------ FINISH fn_call_cpp_callback_js %d ------\n", output);

    return output;
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // C++ Way of Calling Callbacks

  // cb : Async Function from JS
  // cb : is a js object, ripe for any and all JS fuckery
  int callback_test_async(emscripten::val cb)
  {
    printf("------ callback_test_async ------\n");
   
    int ret = cb(5).await().as<int>();

    return ret;
  }

  int callback_test(emscripten::val cb)
  {
    printf("------ callback_test ------\n");

    int ret = cb(5).as<int>();

    printf("   ret val: %d \n", ret);
    
    return ret;
  }

  int pass_arr_cpp(std::string js_arr)
  {

    printf("------ pass_arr_cpp ------\n");
    for (unsigned char item : js_arr)
    {
      std::cout << item << std::endl;
    }
    return 10;
  }

//  ██████ ██████  ██████                ██████  ██████   ██████  ██   ██ ██    ██ ██ ███    ██  ██████      
// ██      ██   ██ ██   ██               ██   ██ ██   ██ ██    ██  ██ ██   ██  ██  ██ ████   ██ ██           
// ██      ██████  ██████      █████     ██████  ██████  ██    ██   ███     ████   ██ ██ ██  ██ ██   ███     
// ██      ██      ██                    ██      ██   ██ ██    ██  ██ ██     ██    ██ ██  ██ ██ ██    ██     
//  ██████ ██      ██                    ██      ██   ██  ██████  ██   ██    ██    ██ ██   ████  ██████      
                                                                                                          
                                                                                                          

  int callback_test_async_proxying(emscripten::val cb)
  {
    printf("------ callback_test_async_proxying ------\n");
   
    int ret = cb(5).await().as<int>();

    return ret;
  }




  // Macro to Expose Functions Automagically
  EMSCRIPTEN_BINDINGS(my_module)
  {
    emscripten::function("callback_test", &callback_test);
    emscripten::function("callback_test_async", &callback_test_async);
    emscripten::function("pass_arr_cpp", &pass_arr_cpp);
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}