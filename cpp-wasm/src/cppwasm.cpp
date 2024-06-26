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

//  To call js callback from function pointer
//  extern void call_js_callback(int, uint8_t *, int);

// Expose Interface To TS
EMSCRIPTEN_DECLARE_VAL_TYPE(CallbackType);


// The worker threads we will use. `looper` sits in a loop, continuously
// processing work as it becomes available, while `returner` returns to the JS
// event loop each time it processes work.
// std::thread looper;
// std::thread returner;

// The queue used to send work to both `looper` and `returner`.
// emscripten::ProxyingQueue queue;

// Whether `looper` should exit.
std::atomic<bool> should_quit{false};

//
// void looper_main()
// {
//     while (!should_quit)
//     {
//         queue.execute();
//         sched_yield();
//     }
// }

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

// void test_proxy_async(emscripten::val cb)
// {
//     std::cout << "Testing async proxying\n";

//     int shared_var = 0;

//     std::mutex mutex;
//     std::condition_variable cond;
//     std::thread::id executor;

//     std::cout << "  Main Thread ID: " << executor << " \n";

//     // Proxy to ourselves.
//     queue.proxyAsync(pthread_self(), [&]()
//                      {
//     shared_var = 1;
//     executor = std::this_thread::get_id(); });

//     assert(shared_var == 0);
//     queue.execute();
//     assert(shared_var == 1);
//     assert(executor == std::this_thread::get_id());

//     // Proxy to looper.
//     {
//         queue.proxyAsync(looper.native_handle(), [&]()
//                          {
//       // Lock mutex 
//       {
//         std::unique_lock<std::mutex> lock(mutex);
//         shared_var = 2;
//         std::cout << "    Proxy looper Work: " << shared_var <<" \n";

//       }
//       executor = std::this_thread::get_id();
//       std::cout << "    Proxy Looper Thread ID: " << executor <<" \n";
//       cond.notify_one(); });

//         std::unique_lock<std::mutex> lock(mutex);
//         cond.wait(lock, [&]()
//                   { return shared_var == 2; });
//         assert(executor == looper.get_id());
//     } // Mutex Unlocks here ?

//     // Proxy to returner.
//     {

//         queue.proxyAsync(returner.native_handle(), [&]()
//                          {
//       {
//         std::unique_lock<std::mutex> lock(mutex);
//         shared_var = 3;
//         std::cout << "    Proxy returner Work: " << shared_var <<" \n";
//       }
//       executor = std::this_thread::get_id();
//       std::cout << "    Proxy returner Thread ID: " << executor <<" \n";
//       cond.notify_one(); });

//         std::unique_lock<std::mutex> lock(mutex);

//         cond.wait(lock, [&]()
//                   { return shared_var == 3; });

//         assert(executor == returner.get_id());
//     }

//     //
//     // for(int loop = 0; loop < 50; loop++) {
//     //   std::cout << loop << "\n";
//     //   std::cout << "shared_var: " << shared_var << "\n";
//     //   sleep(1);
//     // }
//     //
// }

// void test_proxy_callback_with_ctx(emscripten::val cb)
// {
//     std::cout << "Testing callback_with_ctx proxying\n";

//     int i = 0;
//     thread_local int j = 0;
//     std::thread::id executor;

//     // Proxy to ourselves.
//     queue.proxyCallbackWithCtx(
//         pthread_self(),
//         [&](auto ctx)
//         {
//             i = 1;
//             executor = std::this_thread::get_id();
//             std::cout << "    Proxy Ourselves Thread ID: " << executor << " \n";
//             ctx.finish();
//         },
//         [&]()
//         { j = 1; },
//         {});
//     assert(i == 0);
//     queue.execute();
//     assert(i == 1);
//     assert(executor == std::this_thread::get_id());
//     assert(j == 1);

//     // Proxy to looper.
//     {
//         queue.proxyCallbackWithCtx(
//             // target
//             looper.native_handle(),
//             // function<void(ProxyingCtx)> &&func,
//             [&](auto ctx)
//             {
//                 i = 2;
//                 executor = std::this_thread::get_id();
//                 // CANNOT Run anything INSIDE HERE ! CAUSES Program to explode.
//                 // std::cout << "    looper : i value:" << i << " thread_id: " << executor << "\n";
//                 // cb(10);

//                 ctx.finish();
//             },
//             // function<void()> &&callback,
//             [&]()
//             { j = 2; },
//             // function<void()> &&cancel
//             {});

//         // TODO: Add a way to wait for work before executing it.
//         int looper_while = 0;
//         while (j != 2)
//         {
//             std::cout << "looper while loop " << looper_while << "\n";
//             queue.execute();
//         }
//         assert(i == 2);
//         assert(executor == looper.get_id());
//     }

//     // Proxy to returner.
//     {
//         queue.proxyCallbackWithCtx(
//             // target
//             returner.native_handle(),
//             // function<void(ProxyingCtx)> &&func,
//             [&](auto ctx)
//             {
//                 i = 3;
//                 executor = std::this_thread::get_id();
//                 std::cout << "    returner : i value:" << i << " thread_id: " << executor << "\n";
//                 cb(10); // pthread error
//                 std::cout << "    returner : i value:" << i << " thread_id: " << executor << "\n";

//                 auto finish = (void (*)(void *))emscripten_proxy_finish;
//                 emscripten_async_call(finish, ctx.ctx, 0);
//             },
//             // function<void()> &&callback,
//             [&]()
//             {
//                 std::cout << "    returner Thread Callback Function \n";
//                 // cb(10);
//                 j = 3;
//             },
//             // function<void()> &&cancel
//             {});
//         while (j != 3)
//         {
//             std::cout << "returner while loop"
//                       << "\n";
//             queue.execute();
//         }
//         assert(i == 3);
//         assert(executor == returner.get_id());
//     }

//     std::cout << "i value: " << i << "\n";
//     std::cout << "END Testing callback_with_ctx proxying\n";
// }

// int callback_test_async_proxying(emscripten::val cb)
// {
//     emscripten::ProxyingQueue local_queue;
//     auto parent_thread = pthread_self();
//     std::cout << "beep\n";
//     std::thread looper([=]()
//                        {
//         for (int i = 0; i < 10; i++) {
//             std::cout << "lol " << i << std::endl;
//             sleep(1);
//             queue.proxyAsync(parent_thread, [=](){
//                 cb(i).await();
//             });
//         } });
//     std::cout << "boop\n";
//     // printf("------ callback_test_async_proxying ------\n");

//     // looper = std::thread(looper_main);
//     // returner = std::thread(returner_main);

//     // // printf("------ test_proxy_async ------\n");
//     // // test_proxy_async(cb);
//     // // printf("------ test_proxy_async ------\n");

//     // printf("------ test_proxy_callback_with_ctx ------\n");
//     // test_proxy_callback_with_ctx(cb);
//     // printf("------ test_proxy_callback_with_ctx ------\n");

//     // // printf("------ Call Callback ------\n");
//     // // cb(25);
//     // // printf("------ Call Callback ------\n");

//     // // test_proxy_sync();
//     // // test_proxy_sync_with_ctx();
//     // // test_proxy_callback();
//     // // test_proxy_callback_with_ctx();

//     // should_quit = true;
//     // looper.join();

//     // pthread_cancel(returner.native_handle());
//     // returner.join();

//     // std::cout << "done\n";

//     return 10;
// }


pthread_t main_thread;
em_proxying_queue *proxy_queue = NULL;
std::thread value_producer_thread;

pthread_t worker;
int i = 0;

void run_job(void *arg)
{
  // printf("------ thread %lu: RUN JOB ------\n", pthread_self());
  emscripten::val *cb = (emscripten::val *)arg;
  (*cb)(i);
  i++;
}

static void *worker_main(void *arg)
{
  while (true)
  {
    // printf("------ thread %lu: PROXY JOB ------\n", pthread_self());
    emscripten_proxy_sync(proxy_queue, main_thread, run_job, arg);
    sleep(1);
  }
}

int run_on_event(emscripten::val arg)
{
  // printf("------ thread %lu: run_on_event ------\n", pthread_self());
  main_thread = pthread_self();

  emscripten::val *cb = new emscripten::val(std::move(arg));

  pthread_create(&worker, NULL, worker_main, (void *)cb);

  proxy_queue = em_proxying_queue_create();
  return 0;
}

struct closure_t
{
  void *cb;
  const int *value;
};

void run_callback(void *arg)
{
    // printf("------ thread %lu: RUN CB ------\n", pthread_self());
    closure_t *closure = (closure_t *)arg;
    emscripten::val *cb = (emscripten::val *)closure->cb;

    //   (*cb)(10, (int)closure->sample->payload.start, (int)closure->sample->payload.len);
    (*cb)(10, 11, 12);
 
}


void callback_handler(const int *value, void *arg)
{

  closure_t closure;
  closure.cb = arg;
  closure.value = value;
  emscripten_proxy_sync(proxy_queue, main_thread, run_callback, &closure);
}

// CPP Produes a value periodically !
int call_js_function_periodically(emscripten::val ts_cb)
{
    // Need to move the function pointer to here
    emscripten::val *ts_cb_ptr = new emscripten::val(std::move(ts_cb));
    
    std::cout << "Main Thread: " << pthread_self() << std::endl;

    // *callback = z_closure_sample(data_handler, NULL, ts_cb_ptr);
    // spawn new thread here with function
    std::thread value_producer_thread([=]()
                    {
        for (int i = 0; i < 1000; i++) {
            std::cout << "Other thread  :" << pthread_self() << std::endl;

            sleep(1);

            // queue.proxyAsync(parent_thread, [=](){
                // cb(i).await();
            // });
        } }
    );

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
    // emscripten::function("callback_test_async", &callback_test_async);
    // emscripten::function("callback_test_async_proxying", &callback_test_async_proxying);
    emscripten::function("pass_arr_cpp", &pass_arr_cpp);

    // C++ Receiving Typescript Callback
    emscripten::function("call_js_function_periodically", &call_js_function_periodically);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////