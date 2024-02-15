
import Module from "./wasm/cpp-wasm.js"




// TODO : Clean up Any's with proper types
interface Module {
    stringToUTF8OnStack(x: string): any,
    _zw_default_config(clocator: any): any,
    onRuntimeInitialized(): Promise<any>,
    registerJSCallback(callback: any): number,
    writeArrayToMemory(array: Uint8Array, buffer: number): any, // TODO: Returns None ? 
    // Horrible horrible horribleness
    cwrap(...arg: any): any,
    ccall(...arg: any): any,
    addFunction(...arg: any): any,
    api: any
}

let mod_instance: Module;

export async function wasmmodule(): Promise<Module> {
    if (!mod_instance) {
        mod_instance = await Module();
        mod_instance.onRuntimeInitialized = async () => {
            const api = {
                //
                _test_call_js_callback: mod_instance.cwrap("test_call_js_callback", "number", [], { async: true }),
                // registerJSCallback
                _register_rm_callback: mod_instance.cwrap("register_rm_callback", "void", ["number"], { async: true }),
                // To allocate memory
                test_call: mod_instance.cwrap("test_call", "number", ["number", "number", "number"], { async: true }),
                fn_no_args: mod_instance.cwrap("fn_no_args", "number", [], { async: true }),
                fn_args: mod_instance.cwrap("fn_args", "number", [], { async: true }),
                fn_call_cpp_callback_js: mod_instance.cwrap("fn_call_cpp_callback_js", "", ["number"], { async: true }),
                // 
                malloc: mod_instance.cwrap("malloc", "number", ["number"],),
                // Horrible
                addFunction: mod_instance.cwrap("addFunction", "number", ["any"],),
                // invoke_function_pointer: mod_instance.cwrap("invoke_function_pointer", "number", ["number"],)
                fn_call_js: mod_instance.cwrap("fn_call_js", "number", [], { async: true }),
            };
            mod_instance.api = api;

        };
        await mod_instance.onRuntimeInitialized()
    }
    return mod_instance
}



// Callback Signature Argument formatting for addFunction
// Example: `vi` - return void fn(i32)
// 'v': void type
// 'i': 32-bit integer type
// 'j': 64-bit integer type (currently does not exist in JavaScript)
// 'f': 32-bit float type
// 'd': 64-bit float type


function ts_callback(num: number): number {
    console.log("TS CALLBACK: " , num );
    return 10 + num;
}

export class DEV {

    static async runme(): Promise<number> {

        const WasmModule: Module = await wasmmodule();
        // 
        console.log("TS");
        const arr = new Uint8Array([65, 66, 67, 68]); 
        var dataPtr = WasmModule.api.malloc(arr.length);
        WasmModule.writeArrayToMemory(arr, dataPtr);
        
        console.log("PTR", dataPtr);
        console.log("Calling WasmModule.api.fn_no_args");
        await WasmModule.api.fn_no_args();
        console.log("Calling WasmModule.api.fn_args");
        await WasmModule.api.fn_args(100, dataPtr, 4);
        console.log("Calling WasmModule.api.fn_call_js");
        // await WasmModule.api.fn_call_js();

        // Via API does not work
        // console.log("WasmModule.api.addFunction(ts_callback);");
        // WasmModule.api.addFunction(ts_callback);
        console.log("WasmModule.addFunction(ts_callback);");

        let fp_num = WasmModule.addFunction(ts_callback,'ii');
        console.log("fp_num", fp_num);
        
        let return_val  = await WasmModule.api.fn_call_cpp_callback_js(fp_num);
        console.log("Callback Value ", return_val);
        


        // GABRIELES METHOD OF ADDING A FUNCTION
        // var fn_callback = await WasmModule.api.registerJSCallback(ts_callback);
        // console.log("JS Callback id")
        // console.log(fn_callback)
        // var fn_callback = await WasmModule.api.registerJSCallback(ts_callback);
        // console.log("JS Callback id")
        // console.log(fn_callback)
        // var fn_callback = await WasmModule.api.registerJSCallback(ts_callback);
        // console.log("JS Callback id")
        // console.log(fn_callback)


        // var cbFunc = function() {
        //     console.log("Hi, this is a cb");
        // }

        // WasmModule.ccall('cbTest', 'number', ['number'], [cbFunc]);        

        // (ident, returnType, argTypes, args, opts)

        // var pointer = WasmModule.addFunction(function() { 
        //     console.log('I was called from C world!'); 
        //   });

        // WasmModule.ccall('invoke_function_pointer', 'number', ['number'], [pointer]);
        // WasmModule.removeFunction(pointer);
        
        // Module.ccall('invoke_function_pointer', 'number', ['number'], [pointer]);
        
        // 
        // var pointer = Runtime.addFunction(function() { 
        //      console.log('I was called from C world!'); 
        // });
        // Module.ccall('invoke_function_pointer', 'number', ['number'], [pointer]);
        // Runtime.removeFunction(pointer);

          
        // WasmModule
        // await WasmModule.api.test_call(dataPtr, arr.length, ts_callback);
        // await WasmModule.api.test_call(dataPtr, arr.length, ts_callback);
        console.log("--??????");
        
        console.log("--DEV End");

        return 10;
    }
}
