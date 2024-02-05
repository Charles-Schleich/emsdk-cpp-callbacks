import * as cppwasm from "../../../esm"



async function main() {
    console.log("Start");

    var x = await cppwasm.DEV.runme();

    console.log(x);
    
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