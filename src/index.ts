
import Module from "./wasm/cpp-wasm.js"

// TODO : Clean up Any's with proper types
interface Module {
    stringToUTF8OnStack(x: string): any,
    _zw_default_config(clocator: any): any,
    onRuntimeInitialized(): Promise<any>,
    registerJSCallback(callback: any): number,
    writeArrayToMemory(array: Uint8Array, buffer: number): any, // TODO: Returns None ? 
    cwrap(...arg: any): any,
    // This doesnt look like it works
    // _malloc(...arg:any) : any,
    api: any
}

let mod_instance: Module;

export async function wasmmodule(): Promise<Module> {
    if (!mod_instance) {
        mod_instance = await Module();
        mod_instance.onRuntimeInitialized = async () => {
            const api = {

                _test_call_js_callback: mod_instance.cwrap("test_call_js_callback", "number", [], { async: true }),
                _register_rm_callback: mod_instance.cwrap("register_rm_callback", "void", ["number"], { async: true }),
                // To allocate memory
                // _z_malloc: mod_instance.cwrap("z_malloc", "number", ["number"], { async: true }),
                fn_no_args: mod_instance.cwrap("fn_no_args", "number", [], { async: true }),
                fn_args: mod_instance.cwrap("fn_args", "number", [], { async: true }),
                // _fn_no_args: mod_instance.cwrap("_fn_no_args", "number", [], { async: true }),
                // _test_call: mod_instance.cwrap("_test_call", "number", ["number", "number", "number"], { async: true }),
                test_call: mod_instance.cwrap("test_call", "number", ["number", "number", "number"], { async: true }),
                // 
                malloc: mod_instance.cwrap("malloc", "number", ["number"],),

            };
            mod_instance.api = api;

        };
        await mod_instance.onRuntimeInitialized()
    }
    return mod_instance
}

// async function ts_callback(): Promise<number> {
//     console.log("TS CALLBACK ");
//     return 10;
// }

export class DEV {

    static async runme(): Promise<number> {

        const WasmModule: Module = await wasmmodule();

        // const arr = new Uint8Array([65, 66, 67, 68]);

        // let dataPtr = await WasmModule.api._z_malloc(arr.length);

        // WasmModule.writeArrayToMemory(arr, dataPtr);

        // let cb = async function () {
        //     var c = 0;
        //     while (c < 10) {
        //         console.log("What is my purpose ? You log");
        //         console.log("Oh my god");
        //         c++
        //     }
        // };

        // var buf = Module.malloc(10);
        // 
        const arr = new Uint8Array([65, 66, 67, 68]); 
        var dataPtr = WasmModule.api.malloc(arr.length);
        WasmModule.writeArrayToMemory(arr, dataPtr);
        
        console.log("PTR", dataPtr);
        //
        console.log("--DEV");
        console.log("Calling WasmModule.api.fn_no_args"); 
        await WasmModule.api.fn_no_args();
        console.log("Calling WasmModule.api.fn_args"); 
        await WasmModule.api.fn_args(100, dataPtr, 4);
        
        
        // WasmModule
        // await WasmModule.api.test_call(dataPtr, arr.length, ts_callback);
        // await WasmModule.api.test_call(dataPtr, arr.length, ts_callback);
        console.log("--??????");
        
        console.log("--DEV End");

        return 10;
    }
}