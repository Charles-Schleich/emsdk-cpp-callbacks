#include <emscripten/bind.h>
#include <emscripten/proxying.h>
#include <emscripten/eventloop.h>
#include <sched.h>

// The worker threads we will use.
// `looper` sits in a loop, continuously processing work as it becomes available:
// `returner` returns to the JS event loop each time it processes work.
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

void test_proxy_callback_with_ctx(emscripten::val cb)
{
    std::cout << "Testing callback_with_ctx proxying\n";

    int i = 0;
    thread_local int j = 0;
    std::thread::id executor;

    // Proxy to ourselves.
    queue.proxyCallbackWithCtx(
        pthread_self(),
        [&](auto ctx)
        {
            i = 1;
            executor = std::this_thread::get_id();
            std::cout << "    Proxy Ourselves Thread ID: " << executor << " \n";
            ctx.finish();
        },
        [&]()
        { j = 1; },
        {});
    assert(i == 0);
    queue.execute();
    assert(i == 1);
    assert(executor == std::this_thread::get_id());
    assert(j == 1);

    // Proxy to looper.
    {
        queue.proxyCallbackWithCtx(
            looper.native_handle(),
            [&](auto ctx)
            {
                i = 2;
                executor = std::this_thread::get_id();
                std::cout << "    Proxy Looper Thread ID: " << executor << " \n";
                ctx.finish();
            },
            [&]()
            { j = 2; },
            {});
        // TODO: Add a way to wait for work before executing it.
        while (j != 2)
        {
            queue.execute();
        }
        assert(i == 2);
        assert(executor == looper.get_id());
    }

    // Proxy to returner.
    {
        queue.proxyCallbackWithCtx(
            returner.native_handle(),
            [&](auto ctx)
            {
                i = 3;
                executor = std::this_thread::get_id();
                std::cout << "    Proxy returner Thread ID: " << executor << " \n";
                auto finish = (void (*)(void *))emscripten_proxy_finish;
                emscripten_async_call(finish, ctx.ctx, 0);
            },
            [&]()
            { j = 3; },
            {});
        while (j != 3)
        {
            queue.execute();
        }
        assert(i == 3);
        assert(executor == returner.get_id());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  Mutex
// The mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads.
// mutex offers exclusive, non-recursive ownership semantics:
//     - A calling thread owns a mutex from the time that it successfully calls either lock or try_lock until it calls unlock.
//     - When a thread owns a mutex, all other threads will block (for calls to lock) or receive a false return value (for try_lock)
//       if they attempt to claim ownership of the mutex.
//     - A calling thread must not own the mutex prior to calling lock or try_lock.
// The behavior of a program is undefined if a mutex is destroyed while still owned by any threads,
// or a thread terminates while owning a mutex. The mutex class satisfies all requirements of Mutex and StandardLayoutType.
// std::mutex is neither copyable nor movable.
std::mutex mutex;
// std::mutex is usually not accessed directly:
//    - std::unique_lock, std::lock_guard, or std::scoped_lock(since C++17) manage locking in a more exception-safe manner.
//    unique_lock is movable, but not copyable
std::unique_lock<std::mutex> lock(mutex);

//  Conditional Variable
// std::condition_variable is a synchronization primitive used with a std::mutex to block one or more threads until
// another thread both modifies a shared variable (the condition) and notifies the std::condition_variable.
std::condition_variable cond;
// wait(lock, pred): blocks the current thread until the condition variable is awakened
// lock 	- 	an lock which must be locked by the calling thread
// pred 	- 	the predicate to check whether the waiting can be completed
cond.wait(lock, [&]()
          { return shared_var == 2; });
cond.notify_one() // notifies one waiting thread

// Proxy Callback with CTX 
bool proxyCallbackWithCtx(
        pthread_t target,
        std::function<void(ProxyingCtx)> &&func,
        std::function<void()> &&callback,
        std::function<void()> &&cancel)
{
}
// Calls emscripten_proxy_callback_with_ctx to execute func and schedule either callback or cancel, 
// returning : true if the function was successfully enqueued 
//           : false otherwise.
