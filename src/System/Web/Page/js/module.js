/////////////////////////////////////////////////////////////////////////////////
//
//  Module handling.
//
//  Callbacks prototypes & description for C code:
//  
//      void onMainModuleReady(void)
//          Fires when main module fully loaded and ready.
//
/////////////////////////////////////////////////////////////////////////////////        

var Module = {
    preRun: [],

    postRun: function () {
        if (Module._onMainModuleReady) {
            Module._onMainModuleReady();
        }
    },

    locateFile: function (path, prefix) {
        if (path.endsWith(".data") ||
            path.endsWith(".wasm")) {
            const binPath = getBinPath();
            return binPath + path;
        }
        return "./" + prefix + path;
    },

    setStatus: function (text) {
        const status = document.getElementById("status-text");
        const progress = document.getElementById("progress-bar");

        const match = text.match(/([0-9\.]+)\/([0-9\.]+)/);

        if (match) {
            const loaded = parseFloat(match[1]);
            const total = parseFloat(match[2]);

            if (total > 0) {
                const value = Math.min(100, Math.round((loaded / total) * 100));

                progress.style.width = value + "%";
                status.innerText = "LOADING DATA: " + value + "%";
            }
        } else {
            if (!text && progress) {
                displayLoading(false);
            }
        }
    },

    canvas: (function () {
        var canvas = document.getElementById("canvas");
        return canvas;
    })(),

    print: (function () {
        return function (text) {
            text = Array.prototype.slice.call(arguments).join(" ");
            console.log(text);
        };
    })(),

    printErr: function (text) {
        text = Array.prototype.slice.call(arguments).join(" ");
        console.error(text);
    },
}