/////////////////////////////////////////////////////////////////////////////////
//
//  Args handling.
//  Transform url args to argc & argv for main()
//
/////////////////////////////////////////////////////////////////////////////////  

let params = new URLSearchParams(document.location.search);
let argv = [];

for (const [key, value] of params) {
    let arg = ("-" + key + (value.length ? ("=" + value) : "")).toString();
    //console.log(arg);
    argv.push(arg);
}

Module.arguments = argv;