import * as cppwasm from "../../../esm"



async function main() {
    console.log("Start");
    // 
    await cppwasm.DEV.call_functions_C_style();
    // 
    await cppwasm.DEV.call_functions_CPP_style();
    // 
    await cppwasm.DEV.call_CPP_function_with_TS_Callback();
    // 
    await cppwasm.DEV.call_CPP_function_with_TS_Callback_Proxied();
    // 
    console.log("End");

}

main().then(() => console.log("Done")).catch(e => {
    console.log(e)
    throw e
})

function executeAsync(func: any) {
    setTimeout(func, 0);
}

function sleep(ms: number) {
    return new Promise(resolve => setTimeout(resolve, ms));
}