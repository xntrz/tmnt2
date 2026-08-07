/////////////////////////////////////////////////////////////////////////////////
//
//  Game menu & launch handling.
//  Starts .wasm depeding on game assets type.
//
/////////////////////////////////////////////////////////////////////////////////        

const ASSETTYPE = {
    NA: 0,
    EU: 1,
    TRIAL: 2,

    UNKNOWN: -1,
}

let assetType = ASSETTYPE.UNKNOWN;

function getWorkerFSMountPath() {
    return "/wfs/";
}

function onWorkerFSReady() {

}

function onWorkerFSMountReady() {
    /* read count of files in .DAT to determine NA or EU assets type */
    postWorkerFSRead(getWorkerFSMountPath() + "TMNT.DAT", 4, 4, 0, 0);
}

function onWorkerFSMountCancel() {
    terminateWorkerFS();
    displaySelectStatus(false);
    displayMenu(true);
}

function onWorkerFSDataReady(data, size, opid, param) {
    if (size <= 0) {
        console.error("failed to read TMNT.DAT");
        displaySelectStatus(true, "Couldn't find TMNT.DAT.\n", true);
        displayMenu(true);
        return;
    }

    const view = new DataView(data.buffer);
    const fileCount = view.getInt32(0, true);

    const fileCountNA = 237;
    const fileCountEU = 293;

    if (fileCount == fileCountNA) {
        assetType = ASSETTYPE.NA;
    } else if (fileCount == fileCountEU) {
        assetType = ASSETTYPE.EU;
    } else {
        assetType = ASSETTYPE.UNKNOWN;
    }

    if (assetType != ASSETTYPE.UNKNOWN) {
        terminateWorkerFS();
        displaySelectStatus(false);
        launch();
    } else {
        terminateWorkerFS();
        displaySelectStatus(true, "Invalid or corrupted game assets.", true);
        displayMenu(true);
    }
}

function getBinPath() {
    if (!isMultiBuild) {
        return "";
    }

    switch (assetType) {
        case ASSETTYPE.EU: return "wasm/full/eu/";
        case ASSETTYPE.NA: return "wasm/full/na/";
        case ASSETTYPE.TRIAL: return "wasm/trial/";
        default: return "";
    }
}

function displaySelectStatus(display, text = "", isError = false) {
    const status = document.getElementById("status-text");
    status.style.display = (display ? "block" : "none");
    status.style.color = (isError ? "red" : "");
    status.innerText = (display ? text : "");
}

function focusCanvas() {
    const canvas = document.querySelector("#canvas");
    if (canvas) {
        canvas.focus();
    }
}

function fixLagOnTouch() {
    /* should be set after wasm launch to prevent for touch presses fps drop
       also added css "touch-action: none;" to canvas */
    window.addEventListener("touchstart", function (e) { e.preventDefault(); }, { passive: false });
    window.addEventListener("touchend", function (e) { e.preventDefault(); }, { passive: false });
    window.addEventListener("touchmove", function (e) { e.preventDefault(); }, { passive: false });
    window.addEventListener("touchcancel", function (e) { e.preventDefault(); }, { passive: false });
}

function displayLoading(display) {
    const status = document.getElementById("status-text");
    status.style.display = (display ? "block" : "none");

    const progress = document.getElementById("progress-bar").parentElement;
    progress.style.display = (display ? "block" : "none");

    if (!display) {
        focusCanvas();
    }
}

function displayMenu(display) {
    const menu = document.querySelector(".buttons-list");
    menu.style.display = (display ? "block" : "none");

    if (!display) {
        focusCanvas();
    }
}

function initMenu() {
    const trial = document.getElementById("btn-trial");
    const full = document.getElementById("btn-full");

    if (isMultiBuild) {
        if (params.has("demo")) {
            full.style.display = "none";
        } else if (params.has("full")) {
            trial.style.display = "none";
        }
    } else if (isTrialBuild) {
        full.style.display = "none";
    }
}

function launch() {
    workerfsMode = WORKERFS_MODE.CCODE;

    script = document.createElement("script");
    script.src = ("./" + getBinPath() + "index.js");

    document.body.appendChild(script);

    fixLagOnTouch();
}

function launchTrial() {
    workerfsMode = WORKERFS_MODE.CCODE;
    assetType = ASSETTYPE.TRIAL;

    displayMenu(false);
    displayLoading(true);

    launch();
}

function launchFull() {
    workerfsMode = WORKERFS_MODE.JS;
    assetType = ASSETTYPE.UNKNOWN;

    displayMenu(false);

    if (params.has("rfs")) {
        launch();
    } else {
        initializeDummyWorkerFS();
        cacheWorkerFSReset();
        setTimeout(function () {
            postWorkerFSMount(getWorkerFSMountPath());
        }, 1000);
        displaySelectStatus(true, "Selecting a game assets directory...");
    }
}

function checkWASMSupport() {
    const result = {
        wasm: false,
        simd: false
    };

    try {
        result.wasm = (typeof WebAssembly === "object") &&
                      (typeof WebAssembly.instantiate === "function");
    } catch {
        result.wasm = false;
    }

    if (result.wasm) {
        try {
            const simdTest = new Uint8Array([
                0, 97, 115, 109,
                1, 0, 0, 0,
                1, 5, 1, 96,
                0, 1, 123,
                3, 2, 1, 0,
                10, 10, 1, 8,
                0, 65, 0,
                253, 15,
                253, 98,
                11
            ]);

            result.simd = WebAssembly.validate(simdTest);
        } catch (e) {
            result.simd = false;
        }
    }

    return result;
}

initMenu();

const feature = checkWASMSupport();

if ((feature.wasm && feature.simd) || params.has("wasm")) {
    displayMenu(true);
} else {
    displaySelectStatus(true, "WASM or WASM SIMD is not supported by this browser", true);
}
