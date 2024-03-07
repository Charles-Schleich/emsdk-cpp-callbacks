import Module from "./wasm/cpp-wasm.js"



interface Module {

    // Intitialize Runtime
    onRuntimeInitialized(): Promise<any>,

    // So that we can read and write into the WASM binary
    writeArrayToMemory(array: Uint8Array, buffer: number): null,

    // This is the original function signature of cwrap and im leaving it here to 
    cwrap(ident: string, returnType: string, argTypes: string[], opts: any): any,
    cwrap(ident: string, returnType: string, argTypes: string[]): any,

    // Add Function, Accepts any function and needs a function Signature, More info Belows
    addFunction(func: (...arg: any) => any, sig: string): any,

    // Async Callbacks with Emscripten Automagically
    callback_test(...arg: any): any,
    callback_test_async(...arg: any): any,
    pass_arr_cpp(...arg: any): any,
    // 
    api: any
}

let mod_instance: Module;

// C way of doing it !
export async function wasmmodule(): Promise<Module> {
    if (!mod_instance) {
        mod_instance = await Module();
        mod_instance.onRuntimeInitialized = async () => {
            const api = {
                malloc: mod_instance.cwrap("malloc", "number", ["number"]),
                //
                fn_no_args: mod_instance.cwrap("fn_no_args", "number", []),
                fn_args: mod_instance.cwrap("fn_args", "number", []),
                // Callbacks 
                fn_call_cpp_callback_js: mod_instance.cwrap("fn_call_cpp_callback_js", "", ["number"]),
                fn_call_js: mod_instance.cwrap("fn_call_js", "number", [], { async: true }),
            };
            mod_instance.api = api;
        };
        await mod_instance.onRuntimeInitialized()
    }
    return mod_instance
}


function ts_callback(num: number): number {
    console.log("    TS CALLBACK: ", num);
    return 10 + num;
}

async function async_ts_callback(num: number): Promise<number> {
    console.log("    ASYNC TS CALLBACK: ", num);
    return 25 + num;
}

export class DEV {

    static C_FN_CALLS: string = " \
 ██████               ███████ ███    ██      ██████  █████  ██      ██      ███████ \n \
██                    ██      ████   ██     ██      ██   ██ ██      ██      ██      \n \
██          █████     █████   ██ ██  ██     ██      ███████ ██      ██      ███████ \n \
██                    ██      ██  ██ ██     ██      ██   ██ ██      ██           ██ \n \
 ██████               ██      ██   ████      ██████ ██   ██ ███████ ███████ ███████ ";

    static async call_functions_C_style() {

        console.log(DEV.C_FN_CALLS);
        console.log("Start : C method of Calling Functions");

        const WasmModule: Module = await wasmmodule();

        const arr = new Uint8Array([65, 66, 67, 68]);
        var dataPtr = WasmModule.api.malloc(arr.length);
        WasmModule.writeArrayToMemory(arr, dataPtr);
        console.log("dataPtr: ", dataPtr);

        // TS -> C : Function Calls 

        await WasmModule.api.fn_call_js();

        await WasmModule.api.fn_no_args();

        await WasmModule.api.fn_args(100, dataPtr, 4);

        // Callback Signature Argument formatting for addFunction
        // Example: `vi` - return void fn(i32)
        // 'v': void type
        // 'i': 32-bit integer type
        // 'j': 64-bit integer type (currently does not exist in JavaScript)
        // 'f': 32-bit float type
        // 'd': 64-bit float type

        console.log("Call C function added via addFunction");
        let fp_sync = WasmModule.addFunction(ts_callback, 'ii');
        console.log("addFunction Pointer:", fp_sync)
        let ret_sync = await WasmModule.api.fn_call_cpp_callback_js(fp_sync);
        console.log("addFunction return: ", ret_sync);
        console.log("=====================================");

    }

    static CPP_FN_CALLS: string = " \
 ██████ ██████  ██████                ███████ ███    ██      ██████  █████  ██      ██      ███████ \n \
██      ██   ██ ██   ██               ██      ████   ██     ██      ██   ██ ██      ██      ██      \n \
██      ██████  ██████      █████     █████   ██ ██  ██     ██      ███████ ██      ██      ███████ \n \
██      ██      ██                    ██      ██  ██ ██     ██      ██   ██ ██      ██           ██ \n \
 ██████ ██      ██                    ██      ██   ████      ██████ ██   ██ ███████ ███████ ███████ ";

    static async call_functions_CPP_style(): Promise<number> {
        console.log(DEV.CPP_FN_CALLS);
        console.log("Start : C++ method of Calling Functions");

        const WasmModule: Module = await wasmmodule();

        const arr = new Uint8Array([65, 66, 67, 68]);
        var dataPtr = WasmModule.api.malloc(arr.length);
        WasmModule.writeArrayToMemory(arr, dataPtr);

        console.log("WasmModule.pass_arr_cpp();");
        let ret_val = await WasmModule.pass_arr_cpp(arr);
        console.log("ret_val: ", ret_val);

        console.log("=====================================");
        return 10;
    }


    static CPP_FN_WITH_TS_CALLBACKS: string = " \
 ██████ ██████  ██████                ███████ ███    ██     ██     ██ ██ ████████ ██   ██     ████████ ███████      ██████  █████  ██      ██      ██████   █████   ██████ ██   ██ ███████ \n \
██      ██   ██ ██   ██               ██      ████   ██     ██     ██ ██    ██    ██   ██        ██    ██          ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██  ██      \n \
██      ██████  ██████      █████     █████   ██ ██  ██     ██  █  ██ ██    ██    ███████        ██    ███████     ██      ███████ ██      ██      ██████  ███████ ██      █████   ███████ \n \
██      ██      ██                    ██      ██  ██ ██     ██ ███ ██ ██    ██    ██   ██        ██         ██     ██      ██   ██ ██      ██      ██   ██ ██   ██ ██      ██  ██       ██ \n \
 ██████ ██      ██                    ██      ██   ████      ███ ███  ██    ██    ██   ██        ██    ███████      ██████ ██   ██ ███████ ███████ ██████  ██   ██  ██████ ██   ██ ███████ ";


    static async call_CPP_function_with_TS_Callback() {
        console.log(DEV.CPP_FN_WITH_TS_CALLBACKS);

        console.log("Start : C++ method of passing Callbacks to CPP code from TypeScript");

        const WasmModule: Module = await wasmmodule();

        console.log("Sync Callback");
        let ret_val = WasmModule.callback_test(ts_callback);
        console.log("Return Value: ", ret_val);

        // CALLBACK ASYNC        
        console.log("Async Callback");
        let ret_val_async_1 = await WasmModule.callback_test_async(async_ts_callback);
        console.log("Return Value: ", ret_val_async_1);

        // CALLBACK ASYNC with promise
        console.log("Async Callback");
        let ret_val_async = await WasmModule.callback_test_async(async_ts_callback);
        console.log("Return Value: ", ret_val_async);
        console.log("=====================================");
    }

    static CPP_FN__TS_CB_PROXY: string = " \
 ██████ ██████  ██████                ███████ ███    ██     ████████ ███████      ██████ ██████      ██████  ██████   ██████  ██   ██ ██    ██ \n \
██      ██   ██ ██   ██               ██      ████   ██        ██    ██          ██      ██   ██     ██   ██ ██   ██ ██    ██  ██ ██   ██  ██  \n \
██      ██████  ██████      █████     █████   ██ ██  ██        ██    ███████     ██      ██████      ██████  ██████  ██    ██   ███     ████   \n \
██      ██      ██                    ██      ██  ██ ██        ██         ██     ██      ██   ██     ██      ██   ██ ██    ██  ██ ██     ██    \n \
 ██████ ██      ██                    ██      ██   ████        ██    ███████      ██████ ██████      ██      ██   ██  ██████  ██   ██    ██    ";


    static async call_CPP_function_with_TS_Callback_Proxied() {
        console.log(DEV.CPP_FN__TS_CB_PROXY);

        console.log("Start : C++ method of passing Callbacks to CPP code from TypeScript");
        // const WasmModule: Module = await wasmmodule();

    }

}
