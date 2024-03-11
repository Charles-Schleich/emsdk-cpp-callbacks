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
#include <unistd.h>

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
void looper_main()
{
  while (!should_quit)
  {
    queue.execute();
    sched_yield();
  }
}

//
void returner_main()
{
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

// Passing these in as pointers as interegers

EMSCRIPTEN_KEEPALIVE
void fn_args(int a, int str_ptr, int length)
{
  printf("------ fn_args ------\n");
  std::cout << "The value of 'num' is " << a;
  std::cout << std::endl;

  char(*pointer) = reinterpret_cast<char(*)>(str_ptr); // Recommended by Pierre Avital, /s

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
      console.log('    JS - inside C++ EM_ASM Macro!'););
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


void test_proxy_async(emscripten::val cb) {
  std::cout << "Testing async proxying\n";

  int shared_var = 0;

  std::mutex mutex;
  std::condition_variable cond;
  std::thread::id executor;
  
  std::cout << "  Main Thread ID: " << executor <<" \n";

  // Proxy to ourselves.
  queue.proxyAsync(pthread_self(), [&]() {
    shared_var = 1;
    executor = std::this_thread::get_id();
  });

  assert(shared_var == 0);
  queue.execute();
  assert(shared_var == 1);
  assert(executor == std::this_thread::get_id());

  // Proxy to looper.
  {
    queue.proxyAsync(looper.native_handle(), [&]() {
      // Lock mutex 
      {
        std::unique_lock<std::mutex> lock(mutex);
        shared_var = 2;
        std::cout << "    Proxy looper Work: " << shared_var <<" \n";

      }
      executor = std::this_thread::get_id();
      std::cout << "    Proxy Looper Thread ID: " << executor <<" \n";
      cond.notify_one();
    });
    std::unique_lock<std::mutex> lock(mutex);
    cond.wait(lock, [&]() { return shared_var == 2; });
    assert(executor == looper.get_id());
  } // Mutex Unlocks here ? 

  // Proxy to returner.
  {
    queue.proxyAsync(returner.native_handle(), [&]() {
      {
        std::unique_lock<std::mutex> lock(mutex);
        shared_var = 3;
        std::cout << "    Proxy returner Work: " << shared_var <<" \n";
      }
      executor = std::this_thread::get_id();
      std::cout << "    Proxy returner Thread ID: " << executor <<" \n";
      cond.notify_one();
    });

    std::unique_lock<std::mutex> lock(mutex);

    cond.wait(lock, [&]() { return shared_var == 3; });
    
    assert(executor == returner.get_id());
  }  


  // 
  for(int loop = 0; loop < 50; loop++) {
    std::cout << loop << "\n";
    std::cout << "shared_var: " << shared_var << "\n";
    sleep(1);
  }
  //
}


int callback_test_async_proxying(emscripten::val cb)
{
  printf("------ callback_test_async_proxying ------\n");

  looper = std::thread(looper_main);
  returner = std::thread(returner_main);

  printf("------ test_proxy_async ------\n");
  test_proxy_async(cb);
  printf("------ test_proxy_async ------\n");

  // test_proxy_sync();
  // test_proxy_sync_with_ctx();
  // test_proxy_callback();
  // test_proxy_callback_with_ctx();

  should_quit = true;
  looper.join();

  pthread_cancel(returner.native_handle());
  returner.join();

  std::cout << "done\n";

  return 10;
}



// Macro to Expose Functions Automagically
EMSCRIPTEN_BINDINGS(my_module)
{
  // C
  emscripten::function("fn_no_args", &fn_no_args);
  emscripten::function("fn_call_js", &fn_call_js);
  emscripten::function("fn_args", &fn_args);

  // C++

  emscripten::function("fn_call_cpp_callback_js", &fn_call_cpp_callback_js);
  emscripten::function("callback_test", &callback_test);
  emscripten::function("callback_test_async", &callback_test_async);
  emscripten::function("callback_test_async_proxying", &callback_test_async_proxying);
  emscripten::function("pass_arr_cpp", &pass_arr_cpp);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////